/* request-source-view.c
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
#include <gtksourceview-5/gtksourceview/gtksource.h>
#include <jansson.h>

#include "request-source-view.h"

struct _RequestSourceView {
    GtkBox parent_instance;

    gboolean is_readonly;
    GtkSourceLanguageManager * language_manager;

    /* Template widgets */
    GtkSourceView * source_view;
    GtkBox * source_toolbar;
    GtkButton * beautify_button;
    GtkComboBox * source_language_selector;
};

struct _RequestSourceViewClass {
    GtkBoxClass parent_class;
};

G_DEFINE_TYPE (RequestSourceView, request_source_view, GTK_TYPE_BOX);

static gchar * request_source_view_get_beautified_json (const gchar * text) {
    json_error_t * error = NULL;
    json_t * json = json_loads (text, JSON_DECODE_ANY, error);

    g_return_if_fail (error == NULL);

    return (gchar *) json_dumps (json, JSON_INDENT (4) | JSON_PRESERVE_ORDER);
}

static gchar * request_source_view_get_minified_json (const gchar * text) {
    json_error_t * error = NULL;
    json_t * json = json_loads (text, JSON_DECODE_ANY, error);

    g_return_if_fail (error == NULL);

    return (gchar *) json_dumps (json, JSON_COMPACT | JSON_PRESERVE_ORDER);
}

static void request_source_view_on_beautify_requested (GtkButton * widget, gpointer data) {
    (void) widget;
    RequestSourceView * self = data;
    g_return_if_fail (self != NULL);

    gchar * text = request_source_view_get_text (self);
    request_source_view_set_text (self, request_source_view_get_beautified_json (text));
}

static void request_source_view_on_source_language_change (GtkComboBox * widget, gpointer data) {
    (void) widget;
    RequestSourceView * self = data;
    g_return_if_fail (self != NULL);

    const gchar * select = (gchar *) gtk_combo_box_get_active_id (self->source_language_selector);
    gtk_widget_set_visible (GTK_WIDGET (self->beautify_button), strcmp (select, "text") != 0);

    // GtkSourceBuffer * buffer = (GtkSourceBuffer *) request_source_view_get_buffer(self);
    // gtk_source_buffer_set_language ()
}

static void request_source_view_class_init (RequestSourceViewClass * klass) {
    GtkWidgetClass * widget_class = GTK_WIDGET_CLASS (klass);

    gtk_widget_class_set_template_from_resource (widget_class, "/com/github/guillotjulien/request/resources/ui/request-source-view.ui");
    gtk_widget_class_bind_template_child (widget_class, RequestSourceView, source_view);
    gtk_widget_class_bind_template_child (widget_class, RequestSourceView, source_toolbar);
    gtk_widget_class_bind_template_child (widget_class, RequestSourceView, beautify_button);
    gtk_widget_class_bind_template_child (widget_class, RequestSourceView, source_language_selector);
};

static void request_source_view_init (RequestSourceView * self) {
    gtk_widget_init_template (GTK_WIDGET (self));

    g_return_if_fail (GTK_IS_WIDGET (self->source_view));
    g_return_if_fail (GTK_IS_WIDGET (self->source_toolbar));
    g_return_if_fail (GTK_IS_WIDGET (self->beautify_button));
    g_return_if_fail (GTK_IS_WIDGET (self->source_language_selector));

    // Connect widgets signals
    g_signal_connect (self->beautify_button, "clicked", G_CALLBACK (request_source_view_on_beautify_requested), self);
    g_signal_connect (self->source_language_selector, "changed", G_CALLBACK (request_source_view_on_source_language_change), self);

    self->language_manager = gtk_source_language_manager_new ();

    // Hide beautify button if needed
    const gchar * select = (gchar *) gtk_combo_box_get_active_id (self->source_language_selector);
    gtk_widget_set_visible (GTK_WIDGET (self->beautify_button), strcmp (select, "text") != 0);
}

RequestSourceView * request_source_view_new (gboolean is_readonly) {
    (void) gtk_source_view_get_type (); // ensure source view type is known before instanciating the widget

    RequestSourceView * self = g_object_new (REQUEST_TYPE_SOURCE_VIEW, NULL);
    request_source_view_set_is_readonly (self, is_readonly);

    return self;
}

void request_source_view_set_is_readonly (RequestSourceView * self, gboolean is_readonly) {
    self->is_readonly = is_readonly;

    gtk_widget_set_visible (GTK_WIDGET (self->source_toolbar), !is_readonly);
    g_object_set (self->source_view, "editable", !is_readonly, NULL);
}

gchar * request_source_view_get_text (RequestSourceView * self) {
    GtkTextIter start, end;
    GtkTextBuffer * buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (self->source_view));

    gtk_text_buffer_get_start_iter (buffer, &start);
    gtk_text_buffer_get_end_iter (buffer, &end);

    gchar * text = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);

    switch (request_source_view_get_content_type (self)) {
        case CONTENT_TYPE_JSON:
            text = request_source_view_get_minified_json (text);
            break;
        case CONTENT_TYPE_XML:
            /* code */
            break;
        case CONTENT_TYPE_YAML:
            /* code */
            break;
        default:
            break;
    }

    return text;
}

void request_source_view_set_text (RequestSourceView * self, gchar * text) {
    // TODO:
    // 1- guess language
    // 2- set language dropdown to the appropriate value

    GtkTextTagTable * text_table = gtk_text_tag_table_new ();
    GtkSourceBuffer * buffer = gtk_source_buffer_new (text_table);
    gtk_text_buffer_set_text ((GtkTextBuffer *) buffer, text, strlen (text));

    gtk_text_view_set_buffer (GTK_TEXT_VIEW (self->source_view), (GtkTextBuffer *) buffer);
}

RequestSourceViewContentType request_source_view_get_content_type (RequestSourceView * self) {
    const gchar * select = (gchar *) gtk_combo_box_get_active_id (self->source_language_selector);
    if (strcmp (select, "json") == 0) {
        return CONTENT_TYPE_JSON;
    }
    if (strcmp (select, "xml") == 0) {
        return CONTENT_TYPE_XML;
    }
    if (strcmp (select, "yaml") == 0) {
        return CONTENT_TYPE_YAML;
    }
    return CONTENT_TYPE_TEXT;
}

void request_source_view_set_content_type (RequestSourceView * self, RequestSourceViewContentType content_type) {
    switch (content_type) {
        case CONTENT_TYPE_JSON:
            gtk_combo_box_set_active_id (self->source_language_selector, "json");
            break;
        case CONTENT_TYPE_XML:
            gtk_combo_box_set_active_id (self->source_language_selector, "xml");
            break;
        case CONTENT_TYPE_YAML:
            gtk_combo_box_set_active_id (self->source_language_selector, "yaml");
            break;
        default:
            gtk_combo_box_set_active_id (self->source_language_selector, "text");
            break;
    }
}
