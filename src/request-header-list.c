/* request-header-list.c
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

#include <gtk-4.0/gdk/x11/gdkx.h>
#include <libsoup/soup.h>

#include "request-header-list.h"

typedef struct _RequestHeaderListPrivate RequestHeaderListPrivate;

struct _RequestHeaderList {
    GtkBox parent_instance;

    RequestHeaderListPrivate * priv;
};

struct _RequestHeaderListClass {
    GtkBoxClass parent_class;
};

struct _RequestHeaderListPrivate {
    gboolean is_readonly;
};

G_DEFINE_TYPE_WITH_CODE (RequestHeaderList, request_header_list, GTK_TYPE_BOX, G_ADD_PRIVATE (RequestHeaderList));

static void request_header_list_class_init (RequestHeaderListClass * klass) {
    GtkWidgetClass * widget_class = GTK_WIDGET_CLASS (klass);

    gtk_widget_class_set_template_from_resource (widget_class, "/com/github/guillotjulien/request/resources/ui/request-header-view.ui");
    // gtk_widget_class_bind_template_child (widget_class, RequestResponseBar, request_code_label);
}

static void request_header_list_init (RequestHeaderList * self) {
    gtk_widget_init_template (GTK_WIDGET (self));
}

RequestHeaderList * request_header_list_new (gboolean is_readonly) {
    RequestHeaderList * self = g_object_new (REQUEST_TYPE_HEADER_LIST, NULL);

    self->priv->is_readonly = is_readonly;

    return self;
}