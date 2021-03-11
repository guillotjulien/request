/* request-url-bar.c
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
#include <uriparser/Uri.h>

#include "request-url-bar.h"

#define RANGE(x)  (int) ((x).afterLast - (x).first)

typedef struct _RequestURLBarPrivate RequestURLBarPrivate;

struct _RequestURLBar {
    GtkBox parent_instance;

    /* Template widgets */
    GtkComboBoxText * http_verb_selector;
    GtkEntry * url_bar;
    GtkButton * send_button;

    RequestURLBarPrivate * priv;
};

struct _RequestURLBarClass {
    GtkBoxClass parent_class;
};

struct _RequestURLBarPrivate {
    gint64 request_start_time;
};

// G_DEFINE_TYPE(RequestURLBar, request_url_bar, GTK_TYPE_BOX);
G_DEFINE_TYPE_WITH_CODE (RequestURLBar, request_url_bar, GTK_TYPE_BOX, G_ADD_PRIVATE (RequestURLBar));

RequestURLBar * request_url_bar_new (void) {
    return g_object_new (REQUEST_TYPE_URL_BAR, NULL);
}

static void request_url_bar_on_request_start (SoupMessage * msg, gpointer data) {
    (void) msg;
    RequestURLBar * self = data;
    g_return_if_fail (self != NULL);

    RequestURLBarPrivate * priv = request_url_bar_get_instance_private (self);

    if (priv->request_start_time != ((gint64) 0)) {
        return;
    }

    priv->request_start_time = g_get_real_time ();

    g_signal_emit_by_name (self, REQUEST_STARTED_SIGNAL, msg);
}

static void request_url_bar_on_request_end (SoupMessage * msg, gpointer data) {
    (void) msg;
    RequestURLBar * self = data;
    g_return_if_fail (self != NULL);

    RequestURLBarPrivate * priv = request_url_bar_get_instance_private (self);

    // request_partials_request_bar_set_request_duration (self);

    priv->request_start_time = (gint64) 0;

    // request_partials_request_bar_toggle_request_result_bar_visibility (self);

    g_signal_emit_by_name (self, REQUEST_COMPLETED_SIGNAL, msg);
}

static void request_url_bar_on_request_submitted (GtkWidget * widget, gpointer data) {
    (void) widget;

    GtkEntryBuffer * entry_buffer;
    gchar * verb = NULL;
    gchar * url = NULL;

    RequestURLBar * self = data;
    if (data == NULL)
        return; // FIXME: Crash

    entry_buffer = gtk_entry_get_buffer (self->url_bar);
    if (gtk_entry_buffer_get_length (entry_buffer) == 0)
        return;

    const gchar * buffer_text = gtk_entry_buffer_get_text (entry_buffer);
    if (buffer_text == NULL)
        return; // FIXME: Crash, this is not normal to have a buffer with a length, but not getting any text

    url = g_strdup (buffer_text);

    // FIXME: Remove whitespace before and after URL

    UriUriA uri;
    const char * errorPos;
    if (uriParseSingleUriA (&uri, url, &errorPos) != URI_SUCCESS) {
        printf ("Error position: %s\n", errorPos);
        return;
    }

    gchar * scheme;
    if (RANGE (uri.scheme) == '\0') { // FIXME: prefixing with only "//"" or "/"" will result in an invalid URL.
        g_info ("No scheme found, autoprefixing with http.\n");

        const char * prefix = "http://";
        gchar * prefixed_url = g_strconcat (prefix, url, NULL);

        g_free (url);
        url = prefixed_url;
        scheme = "http"; // FIXME: Use an Enum or a const
    } else {
        gchar extracted_scheme[RANGE (uri.scheme) + 1];
        memcpy (extracted_scheme, url, RANGE (uri.scheme));
        extracted_scheme[RANGE (uri.scheme)] = '\0';

        scheme = extracted_scheme;

        // printf("Extracted: %s\n", extracted_scheme);
        printf ("Detected Scheme: %s\n", scheme); // FIXME: Why in hell is scheme empty outside of condition when we don't print it first???
    }

    if (strcmp (scheme, "http") != 0 && strcmp (scheme, "https") != 0) {
        printf ("Invalid scheme: %s\n", scheme);
        // TODO: Return error to the view
        return;
    }

    verb = gtk_combo_box_text_get_active_text (self->http_verb_selector);
    if (verb == NULL)
        return; // FIXME: Crash

    printf ("URL: %s | Verb: %s\n", url, verb);
    SoupURI * request_uri = soup_uri_new (url);
    if (!SOUP_URI_VALID_FOR_HTTP (request_uri)) {
        printf ("Invalid URI\n");
        // TODO: Return error to the view
        return;
    }

    SoupSession * session = soup_session_new (); // FIXME: Make me available in the struct
    SoupMessage * message = soup_message_new (verb, url);

    SoupLogger * logger = soup_logger_new (SOUP_LOGGER_LOG_HEADERS, -1); // FIXME: Init logger at widget init
    soup_session_add_feature (session, SOUP_SESSION_FEATURE (logger));
    g_object_unref (logger);

    g_signal_connect_object (message, "starting", G_CALLBACK (request_url_bar_on_request_start), self, 0);
    g_signal_connect_object (message, "finished", G_CALLBACK (request_url_bar_on_request_end), self, 0);

    soup_session_send_message (session, message);

    uriFreeUriMembersA (&uri);
    g_object_unref (message);
    g_object_unref (session);
    g_free (verb);
    g_free (url);
}

static void request_url_bar_class_init (RequestURLBarClass * klass) {
    GtkWidgetClass * widget_class = GTK_WIDGET_CLASS (klass);

    gtk_widget_class_set_template_from_resource (widget_class, "/com/github/guillotjulien/request/resources/ui/request-url-bar.ui");
    gtk_widget_class_bind_template_child (widget_class, RequestURLBar, http_verb_selector);
    gtk_widget_class_bind_template_child (widget_class, RequestURLBar, url_bar);
    gtk_widget_class_bind_template_child (widget_class, RequestURLBar, send_button);
}

static void request_url_bar_init (RequestURLBar * self) {
    gtk_widget_init_template (GTK_WIDGET (self));
    gtk_widget_set_size_request (GTK_WIDGET (self), 400, -1);

    // Declare our own signals
    g_signal_new (REQUEST_STARTED_SIGNAL, REQUEST_TYPE_URL_BAR, G_SIGNAL_RUN_LAST, 0, NULL, NULL, g_cclosure_marshal_VOID__OBJECT, G_TYPE_NONE, 1, soup_message_get_type ());
    g_signal_new (REQUEST_COMPLETED_SIGNAL, REQUEST_TYPE_URL_BAR, G_SIGNAL_RUN_LAST, 0, NULL, NULL, g_cclosure_marshal_VOID__OBJECT, G_TYPE_NONE, 1, soup_message_get_type ());

    // Connect widgets signals
    g_signal_connect (self->send_button, "clicked", G_CALLBACK (request_url_bar_on_request_submitted), self);
    g_signal_connect (self->url_bar, "activate", G_CALLBACK (request_url_bar_on_request_submitted), self);
}
