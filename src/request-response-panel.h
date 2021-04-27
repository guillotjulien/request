/* request-response-panel.h
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

#include "request-header-list.h"
#include "request-source-view.h"

G_BEGIN_DECLS

#define REQUEST_TYPE_RESPONSE_PANEL (request_response_panel_get_type ())

G_DECLARE_FINAL_TYPE (RequestResponsePanel, request_response_panel, REQUEST, RESPONSE_PANEL, GObject)

RequestResponsePanel * request_response_panel_new (void);
GtkWidget * request_response_panel_get_view (RequestResponsePanel * self);
RequestHeaderList * request_response_panel_get_header_list_view (RequestResponsePanel * self);
RequestSourceView * request_response_panel_get_source_view (RequestResponsePanel * self);
void request_response_panel_set_headers (RequestResponsePanel * self, GSList * headers);

G_END_DECLS
