/* request-window.c
 *
 * Copyright 2021 Julien Guillot
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * 	http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtk-4.0/gdk/x11/gdkx.h>
#include <libsoup/soup.h>

#include "request-config.h"
#include "request-window.h"
#include "request-url-bar.h"
#include "request-response-bar.h"

struct _RequestWindow {
    GtkApplicationWindow  parent_instance;

    /* Template widgets */
    GtkPaned  *main_grid;

    /* Custom widgets */
    RequestURLBar      *request_url_bar;
    RequestResponseBar *request_response_bar;
};

G_DEFINE_TYPE (RequestWindow, request_window, GTK_TYPE_APPLICATION_WINDOW)

static void on_request_start (RequestWindow *sender, SoupMessage *msg, gpointer data) {
    (void) sender; // We don't use sender directly as it doesn't contain a reference to the widgets of request_response_bar...
    RequestWindow *self = data;

    g_return_if_fail (self != NULL);
    g_return_if_fail (msg != NULL);
    g_return_if_fail (SOUP_IS_MESSAGE (msg));

    printf("Begin Request\n");

    request_response_bar_on_message_begin (msg, self->request_response_bar);
}

static void on_request_complete (RequestWindow *sender, SoupMessage *msg, gpointer data) {
    (void) sender; // We don't use sender directly as it doesn't contain a reference to the widgets of request_response_bar...
    RequestWindow *self = data;

    g_return_if_fail (self != NULL);
    g_return_if_fail (msg != NULL);
    g_return_if_fail (SOUP_IS_MESSAGE (msg));
    g_return_if_fail (GTK_IS_WIDGET (self->request_response_bar));

    printf("End Request\n");

    request_response_bar_on_message_received (msg, self->request_response_bar);
}

static void request_window_class_init (RequestWindowClass *klass) {
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

    gtk_widget_class_set_template_from_resource (widget_class, "/com/github/guillotjulien/request/resources/ui/window.ui");
    gtk_widget_class_bind_template_child (widget_class, RequestWindow, main_grid);
}

static void request_window_init (RequestWindow *self) {
    GtkCssProvider* provider = gtk_css_provider_new();
    gtk_css_provider_load_from_resource(provider, "/com/github/guillotjulien/request/style.css");
    gtk_style_context_add_provider_for_display(gdk_display_get_default(), GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    gtk_widget_init_template (GTK_WIDGET (self));

    request_window_set_paned_view_size (self);

    GtkWidget *left = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *right = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    gtk_paned_set_start_child (self->main_grid, left);
    gtk_paned_set_end_child (self->main_grid, right);

    // Prevent widgets in paned view from shrinking too much
    gtk_paned_set_shrink_start_child (self->main_grid, FALSE);
    gtk_paned_set_shrink_end_child (self->main_grid, FALSE);

    self->request_url_bar = request_url_bar_new ();
    if (self->request_url_bar != NULL) {
        gtk_box_append (GTK_BOX (left), GTK_WIDGET (self->request_url_bar));

        g_signal_connect (self->request_url_bar, REQUEST_STARTED_SIGNAL, G_CALLBACK (on_request_start), self);
        g_signal_connect (self->request_url_bar, REQUEST_COMPLETED_SIGNAL, G_CALLBACK (on_request_complete), self);
	}

    self->request_response_bar = request_response_bar_new ();
    if (self->request_response_bar != NULL) {
        gtk_box_append (GTK_BOX (right), GTK_WIDGET (self->request_response_bar));
	}

    // TODO: Add separator after the URL bar and response bar
}

void request_window_set_paned_view_size (RequestWindow *self) {
    (void) self; // avoid unused parameter warning

    // If we don't have a stored position for our panels, we set it to 50% of the current window size, 
    // else, we set it to the previously known position.

    // TODO: Implement me
}
