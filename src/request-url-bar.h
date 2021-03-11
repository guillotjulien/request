/* request-url-bar.h
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

#pragma once

#include <gtk-4.0/gtk/gtk.h>

G_BEGIN_DECLS

#define REQUEST_TYPE_URL_BAR (request_url_bar_get_type())

G_DECLARE_FINAL_TYPE (RequestURLBar, request_url_bar, REQUEST, URL_BAR, GtkBox)

#define REQUEST_STARTED_SIGNAL "request-started"
#define REQUEST_COMPLETED_SIGNAL "request-completed"

RequestURLBar* request_url_bar_new(void);

G_END_DECLS