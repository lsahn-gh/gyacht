/* gyacht-container-list-view.c
 *
 * Copyright 2019 Yi-Soo An <yisooan@fedoraproject.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "gyacht-container-list-view.h"
#include "gyacht-container-private.h"
#include "gyacht-container-service.h"
#include "gyacht-debug.h"
#include "gyacht-macros.h"

#include <glib/gi18n.h>

struct _GyachtContainerListView
{
  GtkBox                  parent_instance;

  /* Widgets */
  GtkListBox              *list_box;

  GyachtContainerService  *service;
};

G_DEFINE_TYPE (GyachtContainerListView, gyacht_container_list_view, GTK_TYPE_BOX)


static void
internal_clear_list_box_rows (GyachtContainerListView *self)
{
  GList *children, *iter;

  children = gtk_container_get_children (GTK_CONTAINER (self->list_box));
  for (iter = children; iter != NULL; iter = g_list_next (iter))
    gtk_widget_destroy (GTK_WIDGET (iter->data));
  g_list_free (children);
}

static GtkWidget *
internal_create_row (GyachtContainer *container)
{
  GtkWidget *row = NULL;
  GtkWidget *grid = NULL;
  GtkWidget *widget = NULL;
  gconstpointer buffer = NULL;
  PangoAttrList *attrlist = NULL;
  PangoAttribute *attr = NULL;

  /* Row */
  row = gtk_list_box_row_new ();
  /* Set data if need */

  /* Grid */
  grid = gtk_grid_new ();
  gtk_widget_set_margin_start (grid, 6);
  gtk_widget_set_margin_end (grid, 6);
  gtk_widget_set_margin_top (grid, 6);
  gtk_widget_set_margin_bottom (grid, 6);
  gtk_grid_set_column_spacing (GTK_GRID(grid), 12);
  gtk_grid_set_row_spacing (GTK_GRID(grid), 6);

  /* Name */
  buffer = gyacht_container_get_name (container);
  widget = gtk_label_new (buffer);
  gtk_label_set_ellipsize (GTK_LABEL (widget), PANGO_ELLIPSIZE_END);
  gtk_widget_set_hexpand (widget, TRUE);
  gtk_label_set_xalign (GTK_LABEL (widget), 0);

  attrlist = pango_attr_list_new ();
  attr = pango_attr_weight_new (PANGO_WEIGHT_SEMIBOLD);
  pango_attr_list_insert (attrlist, attr);
  gtk_label_set_attributes (GTK_LABEL (widget), attrlist);
  pango_attr_list_unref (attrlist);

  gtk_grid_attach (GTK_GRID (grid), widget, 0, 0, 2, 1);

  /* Id */
  buffer = gyacht_container_get_id (container);
  widget = gtk_label_new (buffer);
  gtk_label_set_ellipsize (GTK_LABEL (widget), PANGO_ELLIPSIZE_END);
  gtk_label_set_xalign (GTK_LABEL (widget), 0);
  gtk_widget_set_sensitive (widget, FALSE);

  gtk_grid_attach (GTK_GRID (grid), widget, 0, 1, 2, 1);

  /* ??? */
  widget = gtk_label_new ("");
  gtk_grid_attach (GTK_GRID (grid), widget, 0, 2, 2, 1);

  /* Image name */
  buffer = gyacht_container_get_image_name (container);
  widget = gtk_label_new (buffer);
  gtk_label_set_xalign (GTK_LABEL (widget), 1);
  gtk_widget_set_sensitive (widget, FALSE);

  gtk_grid_attach (GTK_GRID (grid), widget, 1, 3, 1, 1);

  /* Created */
  buffer = gyacht_container_get_calendar_date (container);
  widget = gtk_label_new (buffer);
  gtk_label_set_xalign (GTK_LABEL (widget), 0);

  gtk_grid_attach (GTK_GRID (grid), widget, 0, 3, 1, 1);

  /* Separator */
  widget = gtk_separator_new (GTK_ORIENTATION_VERTICAL);
  gtk_grid_attach (GTK_GRID (grid), widget, 2, 0, 1, 4);

  /* Button */
  widget = gtk_button_new_from_icon_name ("preferences-other", GTK_ICON_SIZE_BUTTON);
  gtk_widget_set_valign (widget, GTK_ALIGN_CENTER);
  g_object_set_data (G_OBJECT (widget), "row", row);
  gtk_widget_set_tooltip_text (widget, _("Open the information dialog on the container"));
  /*g_signal_connect (G_OBJECT (widget), "clicked", NULL, self);*/
  gtk_button_set_relief (GTK_BUTTON (widget), GTK_RELIEF_NONE);

  gtk_grid_attach (GTK_GRID (grid), widget, 3, 0, 1, 4);

  gtk_container_add (GTK_CONTAINER (row), grid);
  gtk_widget_show_all (row);

  return row;
}

static void
internal_set_rows_foreach_cb (gpointer data,
                              gpointer user_data)
{
  GyachtContainerListView *self = GYACHT_CONTAINER_LIST_VIEW (user_data);
  GyachtContainer *container = GYACHT_CONTAINER (data);
  GtkWidget *row = NULL;

  row = internal_create_row (container);

  gtk_list_box_insert (GTK_LIST_BOX (self->list_box), row, -1);
}

static void
internal_container_list_set_rows (GyachtContainerListView *self)
{
  GSequence *containers = NULL;

  internal_clear_list_box_rows (self);

  containers = gyacht_container_service_get_containers (self->service);
  if (containers)
    g_sequence_foreach (containers, internal_set_rows_foreach_cb, self);
}

static void
internal_box_header_func (GtkListBoxRow *row,
                          GtkListBoxRow *before,
                          gpointer       user_data)
{
  GtkWidget *cur;

  if (!before)
    {
      gtk_list_box_row_set_header (row, NULL);
      return;
    }

  cur = gtk_list_box_row_get_header (row);
  if (!cur)
    {
      cur = gtk_separator_new (GTK_ORIENTATION_HORIZONTAL);
      gtk_widget_show (cur);
      gtk_list_box_row_set_header (row, cur);
    }
}

/* --- GObject --- */
static void
gyacht_container_list_view_finalize (GObject *object)
{
  GyachtContainerListView *self = GYACHT_CONTAINER_LIST_VIEW (object);

  GYACHT_TRACE_ENTRY;

  if (self->service)
    g_signal_handlers_disconnect_by_func (self->service,
                                          internal_container_list_set_rows,
                                          self);
  g_clear_object (&self->service);

  GYACHT_TRACE_EXIT;

  G_OBJECT_CLASS (gyacht_container_list_view_parent_class)->finalize (object);
}

static void
gyacht_container_list_view_class_init (GyachtContainerListViewClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->finalize = gyacht_container_list_view_finalize;

  gtk_widget_class_set_template_from_resource (widget_class,
                                               GYACHT_UI_PREFIX "gyacht-container-list-view.ui");
  gtk_widget_class_bind_template_child (widget_class, GyachtContainerListView, list_box);
}

static void
gyacht_container_list_view_init (GyachtContainerListView *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));

  gtk_list_box_set_header_func (GTK_LIST_BOX (self->list_box),
                                internal_box_header_func,
                                NULL, NULL);

  self->service = gyacht_container_service_new (RUN_LEVEL_USER);
  g_signal_connect_swapped (self->service,
                            "list-updated",
                            G_CALLBACK (internal_container_list_set_rows),
                            self);
}
