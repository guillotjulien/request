/* request-double-entry.c
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

#include "request-double-entry.h"

typedef struct _RequestDoubleEntryPrivate RequestDoubleEntryPrivate;

struct _RequestDoubleEntry {
    GtkBox parent_instance;

    gboolean is_initialized;
    gboolean is_enabled;

    /* Template widgets */
    GtkEntry * label;
    GtkEntry * value;
    GtkCheckButton * disable_button;
    GtkButton * delete_button;

    RequestDoubleEntryPrivate * priv;
};

struct _RequestDoubleEntryClass {
    GtkBoxClass parent_class;
};

struct _RequestDoubleEntryPrivate {
    gboolean is_readonly;
};

G_DEFINE_TYPE_WITH_CODE (RequestDoubleEntry, request_double_entry, GTK_TYPE_BOX, G_ADD_PRIVATE (RequestDoubleEntry));

static void request_double_entry_on_entry (GtkEditable * editable, gpointer data) {
    (void) editable;
    RequestDoubleEntry * self = data;
    g_return_if_fail (self != NULL);

    g_signal_emit_by_name (self, DOUBLE_ENTRY_CHANGED_SIGNAL, self);
}

static void request_double_entry_on_delete (GtkButton * button, gpointer data) {
    (void) button;
    RequestDoubleEntry * self = data;
    g_return_if_fail (self != NULL);

    // TODO: Ask for confirmation before deleting the row

    g_signal_emit_by_name (self, DOUBLE_ENTRY_DELETE_SIGNAL, self);
}

static void request_double_entry_on_activate_toggled (GtkCheckButton * button, gpointer data) {
    (void) button;
    RequestDoubleEntry * self = data;
    g_return_if_fail (self != NULL);

    self->is_enabled = gtk_check_button_get_active (button);

    gtk_widget_set_sensitive (GTK_WIDGET (self->label), self->is_enabled);
    gtk_widget_set_sensitive (GTK_WIDGET (self->value), self->is_enabled);
}

static void request_double_entry_class_init (RequestDoubleEntryClass * klass) {
    GtkWidgetClass * widget_class = GTK_WIDGET_CLASS (klass);

    gtk_widget_class_set_template_from_resource (widget_class, "/com/github/guillotjulien/request/resources/ui/request-double-entry.ui");
    gtk_widget_class_bind_template_child (widget_class, RequestDoubleEntry, label);
    gtk_widget_class_bind_template_child (widget_class, RequestDoubleEntry, value);
    gtk_widget_class_bind_template_child (widget_class, RequestDoubleEntry, disable_button);
    gtk_widget_class_bind_template_child (widget_class, RequestDoubleEntry, delete_button);

    // Declare our own signals
    g_signal_new (DOUBLE_ENTRY_CHANGED_SIGNAL, REQUEST_TYPE_DOUBLE_ENTRY, G_SIGNAL_RUN_LAST, 0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
    g_signal_new (DOUBLE_ENTRY_DELETE_SIGNAL, REQUEST_TYPE_DOUBLE_ENTRY, G_SIGNAL_RUN_LAST, 0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
}

static void request_double_entry_init (RequestDoubleEntry * self) {
    gtk_widget_init_template (GTK_WIDGET (self));

    g_return_if_fail (GTK_IS_WIDGET (self->label));
    g_return_if_fail (GTK_IS_WIDGET (self->value));
    g_return_if_fail (GTK_IS_WIDGET (self->disable_button));
    g_return_if_fail (GTK_IS_WIDGET (self->delete_button));

    // Connect widgets signals
    g_signal_connect (self->label, "changed", G_CALLBACK (request_double_entry_on_entry), self);
    g_signal_connect (self->value, "changed", G_CALLBACK (request_double_entry_on_entry), self);
    g_signal_connect (self->delete_button, "clicked", G_CALLBACK (request_double_entry_on_delete), self);
    g_signal_connect (self->disable_button, "toggled", G_CALLBACK (request_double_entry_on_activate_toggled), self);

    self->is_initialized = TRUE;
}

RequestDoubleEntry * request_double_entry_new (const gchar * label, const gchar * value, gboolean is_readonly) {
    RequestDoubleEntry * self = g_object_new (REQUEST_TYPE_DOUBLE_ENTRY, NULL);

    g_return_if_fail (label != NULL);
    g_return_if_fail (value != NULL);

    GtkEntryBuffer * label_buffer = gtk_entry_buffer_new (label, strlen (label) + 1);
    gtk_entry_set_buffer (self->label, label_buffer);

    GtkEntryBuffer * value_buffer = gtk_entry_buffer_new (value, strlen (value) + 1);
    gtk_entry_set_buffer (self->value, value_buffer);

    if (is_readonly) {
        gtk_widget_set_visible (GTK_WIDGET (self->delete_button), FALSE);
        gtk_widget_set_visible (GTK_WIDGET (self->disable_button), FALSE);
        g_object_set (self->label, "editable", FALSE, NULL);
        g_object_set (self->value, "editable", FALSE, NULL);
        gtk_widget_set_can_focus (GTK_WIDGET (self->label), FALSE);
        gtk_widget_set_can_focus (GTK_WIDGET (self->value), FALSE);
    } else {
        gtk_check_button_set_active (self->disable_button, TRUE);
    }

    // self->priv->is_readonly = is_readonly;

    return self;
}

void request_double_entry_set_label (RequestDoubleEntry * self, const gchar * label) {
    g_return_if_fail (GTK_IS_WIDGET (self));

    GtkEntryBuffer * label_buffer = gtk_entry_buffer_new (label, strlen (label) + 1);
    gtk_entry_set_buffer (self->label, label_buffer);
}

void request_double_entry_set_value (RequestDoubleEntry * self, const gchar * value) {
    g_return_if_fail (GTK_IS_WIDGET (self->value));

    GtkEntryBuffer * value_buffer = gtk_entry_buffer_new (value, strlen (value) + 1);
    gtk_entry_set_buffer (self->value, value_buffer);
}

void request_double_entry_set_is_read_only (RequestDoubleEntry * self, gboolean is_readonly) {
    g_return_if_fail (GTK_IS_WIDGET (self->delete_button));
    g_return_if_fail (GTK_IS_WIDGET (self->disable_button));
    g_return_if_fail (GTK_IS_WIDGET (self->label));
    g_return_if_fail (GTK_IS_WIDGET (self->value));

    // TODO: Store if we are readonly or not

    if (is_readonly) {
        gtk_widget_set_visible (GTK_WIDGET (self->delete_button), FALSE);
        gtk_widget_set_visible (GTK_WIDGET (self->disable_button), FALSE);
        g_object_set (self->label, "editable", FALSE, NULL);
        g_object_set (self->value, "editable", FALSE, NULL);
        gtk_widget_set_can_focus (GTK_WIDGET (self->label), FALSE);
        gtk_widget_set_can_focus (GTK_WIDGET (self->value), FALSE);
    } else {
        gtk_check_button_set_active (self->disable_button, TRUE);
    }
}
