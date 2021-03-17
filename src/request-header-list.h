/* request-header-list.h
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

#include "request-double-entry.h"

G_BEGIN_DECLS

#define REQUEST_TYPE_HEADER_LIST (request_header_list_get_type ())
#define REQUEST_TYPE_HEADER_LIST_ROW (request_header_list_row_get_type ())

G_DECLARE_FINAL_TYPE (RequestHeaderList, request_header_list, REQUEST, HEADER_LIST, GObject)
G_DECLARE_FINAL_TYPE (RequestHeaderListRow, request_header_list_row, REQUEST, HEADER_LIST_ROW, GObject)

RequestHeaderListRow * request_header_list_row_new ();

RequestHeaderList * request_header_list_new ();
GtkWidget * request_header_list_get_view (RequestHeaderList * self);
void request_header_list_add_row (RequestHeaderList * self, RequestHeaderListRow * row);

G_END_DECLS
