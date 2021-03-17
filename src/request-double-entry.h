/* request-double-entry.h
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

#define REQUEST_TYPE_DOUBLE_ENTRY (request_double_entry_get_type ())
#define REQUEST_TYPE_DOUBLE_ENTRY_DATA (request_double_entry_data_get_type ())

G_DECLARE_FINAL_TYPE (RequestDoubleEntry, request_double_entry, REQUEST, DOUBLE_ENTRY, GtkBox)


#define DOUBLE_ENTRY_CHANGED_SIGNAL "changed"
#define DOUBLE_ENTRY_ACTIVATE_TOGGLED "activate-toggled"
#define DOUBLE_ENTRY_DELETE_SIGNAL "delete"

RequestDoubleEntry * request_double_entry_new (const gchar * label, const gchar * value, gboolean is_readonly);
void request_double_entry_set_label (RequestDoubleEntry * self, const gchar * label);
void request_double_entry_set_value (RequestDoubleEntry * self, const gchar * value);
void request_double_entry_set_is_read_only (RequestDoubleEntry * self, gboolean is_readonly);

G_END_DECLS
