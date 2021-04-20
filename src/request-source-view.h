/* request-source-view.h
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

#pragma once

#include <gtk-4.0/gtk/gtk.h>

G_BEGIN_DECLS

typedef enum RequestSourceViewContentType {
    CONTENT_TYPE_JSON,
    CONTENT_TYPE_XML,
    CONTENT_TYPE_YAML,
    CONTENT_TYPE_TEXT,
} RequestSourceViewContentType;

#define REQUEST_TYPE_SOURCE_VIEW (request_source_view_get_type ())

G_DECLARE_FINAL_TYPE (RequestSourceView, request_source_view, REQUEST, SOURCE_VIEW, GtkBox)

RequestSourceView * request_source_view_new (gboolean is_readonly);
void request_source_view_set_is_readonly (RequestSourceView * self, gboolean is_readonly);
gchar * request_source_view_get_text (RequestSourceView * self);
void request_source_view_set_text (RequestSourceView * self, gchar * text);
RequestSourceViewContentType request_source_view_get_content_type (RequestSourceView * self);
void request_source_view_set_content_type (RequestSourceView * self, RequestSourceViewContentType content_type);

G_END_DECLS
