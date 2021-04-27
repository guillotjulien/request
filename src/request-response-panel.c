/* request-response-panel.c
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

#include "request-response-panel.h"
#include "request-header-list.h"
#include "request-source-view.h"

struct _RequestResponsePanel {
    GObject parent_instance;

    GtkNotebook * container;

    RequestHeaderList * header_list;
    RequestSourceView * source_view;
};

struct _RequestResponsePanelClass {
    GObjectClass parent_class;
};

G_DEFINE_TYPE (RequestResponsePanel, request_response_panel, G_TYPE_OBJECT);

static void request_response_panel_class_init (RequestResponsePanelClass * klass) {
    (void) klass;
}

static void request_response_panel_init (RequestResponsePanel * self) {
    (void) self;
}

RequestResponsePanel * request_response_panel_new (void) {
    RequestResponsePanel * self = (RequestResponsePanel *) g_object_new (REQUEST_TYPE_RESPONSE_PANEL, NULL);
    GtkWidget * notebook = gtk_notebook_new ();
    self->container = GTK_NOTEBOOK (notebook);

    self->header_list = request_header_list_new ();
    g_return_if_fail (self->header_list != NULL);

    self->source_view = request_source_view_new (TRUE);
    g_return_if_fail (self->source_view != NULL);

    // TODO: Create a RequestLabelWithBadge widget
    GtkWidget * body_label = gtk_label_new ("Body"); // FIXME: Handle translations
    GtkWidget * header_list_label = gtk_label_new ("Headers"); // FIXME: Handle translations

    gtk_notebook_append_page (self->container, GTK_WIDGET (self->source_view), GTK_WIDGET (body_label));
    gtk_notebook_append_page (self->container, GTK_WIDGET (request_header_list_get_view (self->header_list)), GTK_WIDGET (header_list_label));

    return self;
}

GtkWidget * request_response_panel_get_view (RequestResponsePanel * self) {
    return GTK_WIDGET (self->container);
}

RequestHeaderList * request_response_panel_get_header_list_view (RequestResponsePanel * self) {
    return self->header_list;
}

RequestSourceView * request_response_panel_get_source_view (RequestResponsePanel * self) {
    return self->source_view;
}

void request_response_panel_set_headers (RequestResponsePanel * self, GSList * headers) {
    request_header_list_empty (self->header_list); // clear previous headers
    while (headers != NULL) {
        RequestHeaderListRow * row = (RequestHeaderListRow *) headers->data;
        headers = headers->next;

        request_header_list_add_row (self->header_list, row);
    }
}
