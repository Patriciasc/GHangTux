/*
 * ########################################################################
 * # File: GHangtux - ghangtux_utils.c                                    #
 * #                                                                      #
 * # Author: Patricia Santana Cruz  <patriciasc@openismus.com>            #
 * #                                                                      #
 * # Copyright (C) 2010 Openismus GmbH                                    #
 * #                                                                      #
 * # Version: 1.1                                                         #  
 * #                                                                      #
 * # Description: A variation of the Hangman game.                        #
 * #                                                                      #
 * # GHangtux is free software; you can redistribute it and/or modify     #
 * # it under the terms of the GNU General Public License as published by #
 * # the Free Software Foundation; either version 3 of the License,       #
 * # or (at your option) any later version.                               #
 * #                                                                      #
 * # GHangtux  is distributed in the hope that it will be useful,         #
 * # but WITHOUT ANY WARRANTY; without even the implied warranty of       #
 * # MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU    #
 * # General Public License for more details.                             #
 * #                                                                      #
 * # You should have received a copy of the GNU General Public License    #
 * # along with GHangtux. If not, see <http://www.gnu.org/licenses/>.     #
 * ########################################################################
*/

#include <gtk/gtk.h>
#include <glib.h>
#include <glib/gi18n.h>
#include <gio/gio.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <string.h>
#include <stdlib.h>
#include "config.h"
#include "ghangtux_keyboard.h"
#include "ghangtux_ui.h"
#include "ghangtux_utils.h"
#include "ghangtux_management.h"
#include "ghangtux.h"

/* Gets player's system files. */
gchar *
gh_utils_get_system_file (const gchar *filename)
{
   gchar *pathname = NULL;
   const gchar* const *system_data_dirs = NULL;
   
   /* Iterate over array of strings to find system data files. */
   for (system_data_dirs = g_get_system_data_dirs (); *system_data_dirs != NULL; system_data_dirs++)
   {
      pathname = g_build_filename (*system_data_dirs, PACKAGE_TARNAME, filename, NULL);
      if (g_file_test (pathname, G_FILE_TEST_EXISTS))
      {
         break;
      }
      else
      {
         g_free (pathname);
         pathname = NULL;
      }
   }

   return pathname;
}

/* Formats the text with a markup before displaying it. */ 
gchar *
gh_utils_format_text_with_markup (const gchar *text,
                                  int type)
{
   gchar *markup = NULL;

   switch (type)
   {
      case 0:    /* Display label */
         markup = g_strdup_printf("<span size=\"large\" font_weight=\"ultrabold\">%s</span>", text);
         break;
      case 1:    /* Title label */
         markup = g_strdup_printf("<span size=\"small\">%s</span>", text);
         break;
      default:
         g_critical ("Problem setting markups\n");
   }

   return markup;
}

/* Loads an image for the GtkImage central area. */
void
gh_utils_load_image (const char *file_image,
                     Gamewidget *gamew)
{
   GdkPixbuf *tux_image = NULL;
   GError *error = NULL;
   gint result = 0;
   
   tux_image = gdk_pixbuf_new_from_file (file_image, &error);

   if (error)
   {
      if (error->domain == GDK_PIXBUF_ERROR)
         g_critical ("GdkPixbufError: %s\n", error->message);
      else if (error->domain == G_FILE_ERROR)
         g_critical ("GFileError: %s\n", error->message);
      else
         g_critical ("An error in the unexpected domain:%d has occurred!\n", error->domain);
 
      g_error_free (error);
      result = 1;
   }
   
   gtk_image_set_from_pixbuf (gamew->image, tux_image); 
   g_object_unref (G_OBJECT (tux_image));
}

/* Import languages for the application. */
void
gh_utils_import_languages ()
{
   bindtextdomain(GETTEXT_PACKAGE, PROGRAMNAME_LOCALEDIR);
   bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
   textdomain(GETTEXT_PACKAGE);
}
