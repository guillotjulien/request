/* request-response-bar.h
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

#define REQUEST_TYPE_RESPONSE_BAR (request_response_bar_get_type())

G_DECLARE_FINAL_TYPE (RequestResponseBar, request_response_bar, REQUEST, RESPONSE_BAR, GtkBox)

RequestResponseBar* request_response_bar_new(void);
void request_response_bar_on_message_begin (SoupMessage *msg, RequestResponseBar *self);
void request_response_bar_on_message_received (SoupMessage *msg, RequestResponseBar *self);

G_END_DECLS