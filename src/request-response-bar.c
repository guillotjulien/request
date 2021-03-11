/* request-response-bar.c
 *
 * Copyright 2021 Julien Guillot
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtk-4.0/gtk/gtk.h>
#include <libsoup/soup.h>
#include <inttypes.h>

#include "request-response-bar.h"

typedef struct _RequestResponseBarPrivate RequestResponseBarPrivate;

struct _RequestResponseBar {
    GtkBox parent_instance;

    /* Template widgets */
    GtkLabel * request_code_label;
    GtkLabel * request_duration_label;
    GtkLabel * request_size_label;

    RequestResponseBarPrivate * priv;
};

struct _RequestResponseBarClass {
    GtkBoxClass parent_class;
};

struct _RequestResponseBarPrivate {
    gint64 request_start_time;
};

G_DEFINE_TYPE_WITH_CODE (RequestResponseBar, request_response_bar, GTK_TYPE_BOX, G_ADD_PRIVATE (RequestResponseBar));

static void request_response_bar_class_init (RequestResponseBarClass * klass) {
    GtkWidgetClass * widget_class = GTK_WIDGET_CLASS (klass);

    gtk_widget_class_set_template_from_resource (widget_class, "/com/github/guillotjulien/request/resources/ui/request-response-bar.ui");
    gtk_widget_class_bind_template_child (widget_class, RequestResponseBar, request_code_label);
    gtk_widget_class_bind_template_child (widget_class, RequestResponseBar, request_duration_label);
    gtk_widget_class_bind_template_child (widget_class, RequestResponseBar, request_size_label);
}

static void request_response_bar_init (RequestResponseBar * self) {
    gtk_widget_init_template (GTK_WIDGET (self));
    gtk_widget_set_size_request (GTK_WIDGET (self), 400, -1);

    // Validate that we retrieved our widgets
    g_return_if_fail (GTK_IS_WIDGET (self->request_code_label));
    g_return_if_fail (GTK_IS_WIDGET (self->request_duration_label));
    g_return_if_fail (GTK_IS_WIDGET (self->request_size_label));
}

/**
 * Returns size as a string from raw content length and add the appropriate
 * prefix at the end of the string.
 * If the body is compressed, we return the COMPRESSED size.
 */
static char * request_response_bar_get_response_size (goffset raw_length) {
    const char * sizes[] = { "TB", "GB", "MB", "KB", "B" };
    const uint64_t exbibytes = 1024ULL * 1024ULL * 1024ULL * 1024ULL;

    guint64 length = (guint64) floor (raw_length / 8);
    uint64_t multiplier = exbibytes;
    for (int i = 0; i < sizeof (sizes) / sizeof (*(sizes)); i++, multiplier /= 1024) {
        if (length < multiplier)
            continue;
        if (length % multiplier == 0)
            return g_strdup_printf ("%" PRIu64 " %s", length / multiplier, sizes[i]);
        else
            return g_strdup_printf ("%.2f %s", (float) length / multiplier, sizes[i]);
    }

    return "0";
}

RequestResponseBar * request_response_bar_new (void) {
    return g_object_new (REQUEST_TYPE_RESPONSE_BAR, NULL);
}

void request_response_bar_on_message_begin (SoupMessage * msg, RequestResponseBar * self) {
    (void) msg;
    g_return_if_fail (self != NULL);

    RequestResponseBarPrivate * priv = request_response_bar_get_instance_private (self);
    priv->request_start_time = g_get_real_time ();
}

void request_response_bar_on_message_received (SoupMessage * msg, RequestResponseBar * self) {
    g_return_if_fail (msg != NULL);
    g_return_if_fail (self != NULL);
    g_return_if_fail (GTK_IS_WIDGET (self->request_code_label));
    g_return_if_fail (GTK_IS_WIDGET (self->request_duration_label));
    g_return_if_fail (GTK_IS_WIDGET (self->request_size_label));

    RequestResponseBarPrivate * priv = request_response_bar_get_instance_private (self);
    GtkStyleContext * context = gtk_widget_get_style_context (GTK_WIDGET (self->request_code_label));
    gchar * status_code = g_strdup_printf ("%u", msg->status_code);

    if (strcmp (status_code, "2") != 0) { // libsoup return 2 on error
        switch (status_code[0]) {
            case '2':
                gtk_style_context_add_class (context, "success");
                break;
            case '3':
            case '4':
                gtk_style_context_add_class (context, "warning");
                break;
            case '5':
                gtk_style_context_add_class (context, "error");
                break;
        }

        gtk_label_set_label (self->request_code_label, g_strdup_printf ("%s %s", status_code, soup_status_get_phrase (msg->status_code)));
        // gtk_widget_set_tooltip_text (GTK_WIDGET (self->request_code_label), soup_status_get_phrase (msg->status_code));
    } else {
        gtk_label_set_label (self->request_code_label, "error");
        gtk_style_context_add_class (context, "error");
    }

    if (priv->request_start_time == ((gint64) 0)) {
        gtk_label_set_label (self->request_duration_label, "0 ms");
    } else {
        gint64 duration = 0LL;
        duration = (g_get_real_time () - priv->request_start_time) / 1000;
        if (duration > ((gint64) 1000)) {
            gtk_label_set_label (self->request_duration_label, g_strdup_printf ("%g s", (gdouble) (duration / 1000)));
        } else {
            gtk_label_set_label (self->request_duration_label, g_strdup_printf ("%g ms", (gdouble) duration));
        }
    }

    priv->request_start_time = (gint64) 0;

    gtk_label_set_label (self->request_size_label, request_response_bar_get_response_size (msg->response_body->length));

    g_free (status_code);
}