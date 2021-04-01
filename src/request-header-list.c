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
#include "request-double-entry.h"

struct _RequestHeaderListRow {
    GObject parent_instance;

    gchar * label;
    gchar * value;
    gboolean is_readonly;

    RequestHeaderList * container;
};

struct _RequestHeaderList {
    GObject parent_instance;

    GListModel * store;

    GtkWidget * scroll_view;
    GtkWidget * list_view;
};

struct _RequestHeaderListClass {
    GtkBoxClass parent_class;
};

G_DEFINE_TYPE (RequestHeaderListRow, request_header_list_row, G_TYPE_OBJECT);
G_DEFINE_TYPE (RequestHeaderList, request_header_list, G_TYPE_OBJECT);

static void request_header_list_row_class_init (RequestHeaderListRowClass * klass) {
    (void) klass;
}

static void request_header_list_row_init (RequestHeaderListRow * self) {
    (void) self;
}

RequestHeaderListRow * request_header_list_row_new (RequestHeaderList * container, gchar * label, gchar * value, gboolean is_readonly) {
    RequestHeaderListRow * self = g_object_new (REQUEST_TYPE_HEADER_LIST_ROW, NULL);

    self->label = label;
    self->value = value;
    self->is_readonly = is_readonly;
    self->container = container;

    return self;
}

static void request_header_list_class_init (RequestHeaderListClass * klass) {
    (void) klass;
}

static GListModel * request_header_list_get_initial_list (void) {
    GListStore * store = g_list_store_new (REQUEST_TYPE_HEADER_LIST_ROW);

    // TODO: On resume, restore headers

    return G_LIST_MODEL (store);
}

static guint request_header_list_get_row_position (RequestHeaderListRow * self) {
    guint position;

    GListStore * store = G_LIST_STORE (self->container->store);
    gboolean found = g_list_store_find (store, self, &position);
    g_return_if_fail (found == TRUE);

    return position;
}

static void on_row_changed_signal (RequestDoubleEntry * row, gpointer data) {
    (void) row;
    RequestHeaderListRow * self = data;

    // When we change the last row, we adds a new row on edit
    if (request_header_list_get_row_position (self) == g_list_model_get_n_items (self->container->store) - 1) {
        request_header_list_add_row (self->container, request_header_list_row_new (self->container, "", "", FALSE));
    }
}

static void on_row_delete_signal (RequestDoubleEntry * row, gpointer data) {
    (void) row;
    RequestHeaderListRow * self = data;
    GListStore * store = G_LIST_STORE (self->container->store);
    guint position = request_header_list_get_row_position (self);

    g_list_store_remove (store, position);
}

static void on_setup_listitem (GtkSignalListItemFactory * factory, GtkListItem * list_item) {
    (void) factory;

    gtk_list_item_set_child (list_item, GTK_WIDGET (request_double_entry_new ("", "", FALSE)));
}

static void on_bind_listitem (GtkSignalListItemFactory * factory, GtkListItem * list_item) {
    (void) factory;

    GtkWidget * entries = gtk_list_item_get_child (list_item);
    RequestHeaderListRow * row = gtk_list_item_get_item (list_item);

    g_return_if_fail (GTK_IS_WIDGET (entries));

    request_double_entry_set_label ((RequestDoubleEntry *) entries, row->label);
    request_double_entry_set_value ((RequestDoubleEntry *) entries, row->value);
    request_double_entry_set_is_read_only ((RequestDoubleEntry *) entries, row->is_readonly);

    if (!row->is_readonly) {
        g_signal_connect (entries, DOUBLE_ENTRY_CHANGED_SIGNAL, G_CALLBACK (on_row_changed_signal), row);
        g_signal_connect (entries, DOUBLE_ENTRY_DELETE_SIGNAL, G_CALLBACK (on_row_delete_signal), row);
    }
}

/**
 * Cleans signals when widget gets recycled.
 */
static void on_unbind_listitem (GtkSignalListItemFactory * factory, GtkListItem * list_item) {
    (void) factory;

    RequestHeaderListRow * row = gtk_list_item_get_item (list_item);
    if (row->is_readonly) {
        return;
    }

    GtkWidget * entries = gtk_list_item_get_child (list_item);
    g_return_if_fail (GTK_IS_WIDGET (entries));

    g_signal_handlers_disconnect_by_func (entries, G_CALLBACK (on_row_changed_signal), row);
    g_signal_handlers_disconnect_by_func (entries, G_CALLBACK (on_row_delete_signal), row);
}

static void request_header_list_init (RequestHeaderList * self) {
    GtkListItemFactory * factory = gtk_signal_list_item_factory_new ();
    g_signal_connect (factory, "setup", G_CALLBACK (on_setup_listitem), NULL);
    g_signal_connect (factory, "bind", G_CALLBACK (on_bind_listitem), NULL);
    g_signal_connect (factory, "unbind", G_CALLBACK (on_unbind_listitem), NULL);

    self->store = request_header_list_get_initial_list ();
    self->list_view = gtk_list_view_new (GTK_SELECTION_MODEL (gtk_single_selection_new (self->store)), factory);
    gtk_list_view_set_single_click_activate (GTK_LIST_VIEW (self->list_view), FALSE);

    self->scroll_view = gtk_scrolled_window_new ();
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (self->scroll_view), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_propagate_natural_height (GTK_SCROLLED_WINDOW (self->scroll_view), TRUE);
    gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (self->scroll_view), self->list_view);
    gtk_widget_set_hexpand (GTK_WIDGET (self->scroll_view), TRUE);
    gtk_widget_set_vexpand (GTK_WIDGET (self->scroll_view), TRUE);
}

/**
 * As of 17-March-2021, ListViews scroll is broken when there is too many items
 * (~300 items).
 * However, it work just fine when we have a reasonable amount of items, thus it
 * is still a good fit for the header list.
 *
 * Issue is tracked here: https://gitlab.gnome.org/GNOME/gtk/-/issues/2971
 */
RequestHeaderList * request_header_list_new (void) {
    return g_object_new (REQUEST_TYPE_HEADER_LIST, NULL);
}

GtkWidget * request_header_list_get_view (RequestHeaderList * self) {
    return self->scroll_view;
}

void request_header_list_add_row (RequestHeaderList * self, RequestHeaderListRow * row) {
    g_list_store_append ((GListStore *) self->store, row);
}

void request_header_list_empty (RequestHeaderList * self) {
    g_list_store_remove_all ((GListStore *) self->store);
}
