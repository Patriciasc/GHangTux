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
#include "main.h"

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

/* Selects a random sentence or word from the game files. */
void
gh_utils_get_sentence (Gamewidget *gamew)
{
   GFile *file = NULL;
   GError *error = NULL;
   GFileInputStream *fstream = NULL; 
   GDataInputStream *stream = NULL;
   static gchar *sys_file = NULL;
   gchar *markup = NULL;
   gsize length = 0;
   gint random = 0;
   
   gh_keyboard_set_sensitive (GHANGTUX_KEYBOARD (gamew->keyboard), TRUE);
   gh_utils_load_image(sys_file = gh_utils_get_system_file (TUX_IMG_0), gamew);
   g_free (sys_file);   
   gamew->n_img = 1;

   /* Status bar context. */
   gamew->scontext = gtk_statusbar_get_context_id (
                    GTK_STATUSBAR (gamew->statusbar),"Statusbar");

   /* Select the file. */
   switch (gamew->theme_id)
   {
      case 0:
         markup = gh_utils_format_text_with_markup (_("Guess the FILM by typing letters"), 1); 
         file = g_file_new_for_path (sys_file = gh_utils_get_system_file(FILMS_FILE));
         gtk_statusbar_push (GTK_STATUSBAR (gamew->statusbar), GPOINTER_TO_INT (gamew), _("Playing theme: Films"));
         break;

      case 1:
         markup = gh_utils_format_text_with_markup (_("Guess the OBJECT by typing letters"), 1); 
         file = g_file_new_for_path (sys_file = gh_utils_get_system_file (OBJECTS_FILE));
         gtk_statusbar_push (GTK_STATUSBAR (gamew->statusbar), GPOINTER_TO_INT (gamew), _("Playing theme: Objects"));
         break;

      case 2:
         markup = gh_utils_format_text_with_markup (_("Guess the PERSON by typing letters"), 1); 
         file = g_file_new_for_path (sys_file = gh_utils_get_system_file(PERSONS_FILE));
         gtk_statusbar_push (GTK_STATUSBAR (gamew->statusbar), GPOINTER_TO_INT (gamew), _("Playing theme: Persons"));
         break;

      default:
         g_critical ("Creating GFIle failed\n");
   }
  
   /* Sets label with markup. */ 
   gtk_label_set_markup (GTK_LABEL (gamew->title_label), markup);

   if (!(fstream = g_file_read (file, NULL, &error)))
   {
      g_critical ("Creating file stream error: %s", error->message);
      g_error_free (error);
   }
   
   g_free (markup);

   stream = g_data_input_stream_new (G_INPUT_STREAM(fstream));

   /* Select a random sentence from the file. */
   random = g_random_int_range (MIN_RANDOM, MAX_RANDOM);
   
   while (((gamew->sentence = g_data_input_stream_read_line (stream, &length, NULL, 
            &error)) != NULL) && (random != 0))
   {
      if (error)
      {
         g_critical ("Reading file error: %s\n", error->message);
         g_error_free (error);
      }
      else
      {
         random--;
         g_free (gamew->sentence);
      }
   }
   
   /* Format the sentence for displaying the first time. */
   g_free (gamew->display_sentence);
   gamew->display_sentence = g_strdup (gamew->sentence);
   gamew->valid_chars = " ";
   g_strcanon ( gamew->display_sentence, gamew->valid_chars, '_');
   markup = gh_utils_format_text_with_markup (gamew->display_sentence, 0); 
   gtk_label_set_markup (GTK_LABEL (gamew->display_label), markup);
   
   g_object_unref (file);
   g_object_unref (stream);
   g_object_unref (fstream);
   g_free (sys_file);
   g_free (markup);
}

/* Formats the text with a markup before displaying it. */ 
gchar *
gh_utils_format_text_with_markup (const gchar *text, int type)
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
gh_utils_load_image (const char *file_image, Gamewidget *gamew)
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
