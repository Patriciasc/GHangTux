/*
 * ########################################################################
 * # File: GHangtux - ghangtux_management.c                               #
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
#include "config.h"

/* Initializes game the first time with the FILM theme. */
void
gh_management_game_init (Gamewidget *gamew)
{
   GtkAction *action = NULL;

   /* Load action. */
   action = gtk_action_group_get_action (gamew->def_group, "FilmsThemesMenuAction");

   if (!action)
   {
      g_critical ("Problem loading action for initializing game\n");
   }

   /* Activate "Film" radio action in the menu. */
   gh_ui_activate_radio_action (NULL, GTK_RADIO_ACTION (action), gamew);

   gtk_main();
}

/* Initialize game's widgets. */
Gamewidget
gh_management_gamew_init ()
{
   Gamewidget gamew ={ NULL, NULL, NULL,  /* Builder,          UI Manager,     Toplevel window */ 
                       NULL, NULL, NULL,  /* Vertical box,     Vertical Box 2, Menubar */
                       NULL, NULL, NULL,  /* Toolbar,          Eventbox,       Game's sentence */ 
                       NULL, NULL, NULL,  /* Display sentence, Asserted chars, Display label   */
                       NULL, NULL, NULL,  /* Title label,      Display image,  Action group */
                       0,    NULL, NULL,  /* Current image,    Keyboard,       Status bar */
                       0,    0,    NULL}; /* Context,          Current theme,  Logo */
  return gamew;
}

/* Sets up the end of the game. */
void
gh_management_set_end_game (gpointer data,
                            int winner)
{
   Gamewidget *gamew = (Gamewidget *)data;
   static gchar *file_path = NULL;
   gchar *markup = NULL;
   
   /* Desable the use of the keys. */
   gh_keyboard_set_sensitive (GHANGTUX_KEYBOARD (gamew->keyboard), FALSE);
 
   /* Set title label. */
   gtk_label_set_text (gamew->title_label, " "); 

   /* Shows solution. */
   markup = gh_utils_format_text_with_markup (gamew->sentence, 0); 
   gtk_label_set_markup (GTK_LABEL (gamew->display_label), markup);
   g_free (markup);
       
   /* Change status bar state. */
   gamew->scontext = gtk_statusbar_get_context_id (GTK_STATUSBAR (gamew->statusbar),
                                                  "Statusbar");
   if (winner == 0)
   {
      gh_utils_load_image (file_path = gh_utils_get_system_file("images/Tux7.png"), gamew);
      gtk_statusbar_push (GTK_STATUSBAR (gamew->statusbar), 
                          GPOINTER_TO_INT (gamew), _("End of game. Try again!"));
   }
   else if (winner == 1)
   {
      gh_utils_load_image (file_path = gh_utils_get_system_file("images/Tux8.png"), gamew);
      gtk_statusbar_push (GTK_STATUSBAR (gamew->statusbar), 
                          GPOINTER_TO_INT (gamew), _("Congratulations!"));
   }
   else
   {
      gh_utils_load_image (file_path = gh_utils_get_system_file("images/Tux7.png"), gamew);
      gtk_statusbar_push (GTK_STATUSBAR (gamew->statusbar), 
                          GPOINTER_TO_INT (gamew), _("Solution"));
   }
   g_free (file_path);
}

/* Set up GtkBuilder. */
void
gh_management_set_builder (Gamewidget *gamew)
{
   static gchar *glade_file = NULL;
   GError *error = NULL;
   GdkColor image_bg = {0, 50535, 55535, 60535};

   /* Setting up the builder. */
   gamew->builder = gtk_builder_new();
  
   /* Load glade file. */
   glade_file = gh_utils_get_system_file (GUI_FILE);
   if (!gtk_builder_add_from_file (gamew->builder, glade_file, &error))
   {
     g_critical ("Building builder failed: %s\n", error->message);
     g_error_free (error);
   }
   g_free (glade_file);

   /* Load game widgets. */
   gamew->window = GTK_WIDGET (gtk_builder_get_object (gamew->builder,WIN));
   gamew->vbox = GTK_WIDGET (gtk_builder_get_object (gamew->builder, VBOX));
   gamew->display_label = GTK_LABEL (gtk_builder_get_object (gamew->builder, SENTENCE_LABEL));
   gamew->title_label = GTK_LABEL (gtk_builder_get_object (gamew->builder, TITLE_LABEL));
   gamew->image = GTK_IMAGE (gtk_builder_get_object (gamew->builder, IMAGE));
   gamew->statusbar = GTK_WIDGET (gtk_builder_get_object (gamew->builder, STATUSBAR));
   gamew->vbox2 = GTK_WIDGET (gtk_builder_get_object (gamew->builder, VBOX2));
   gamew->eventbox = GTK_WIDGET (gtk_builder_get_object (gamew->builder, EVENTBOX));
   gtk_widget_modify_bg (gamew->eventbox, 0, &image_bg);

   /* Connect signals. */
   gtk_builder_connect_signals (gamew->builder,NULL);

   g_object_unref (G_OBJECT(gamew->builder));
}

/* Selects a random sentence or word from the game files. */
void
gh_management_new_game_start (Gamewidget *gamew)
{
   GFile *file = NULL;
   GError *error = NULL;
   GFileInputStream *fstream = NULL; 
   GDataInputStream *stream = NULL;
   static gchar *file_path = NULL;
   gchar *markup = NULL;
   gsize length = 0;
   gint random = 0;
  
   /* Activate keyboard to let player play. */ 
   gh_keyboard_set_sensitive (GHANGTUX_KEYBOARD (gamew->keyboard), TRUE);

   /* Load start image for new game */
   file_path = gh_utils_get_system_file (TUX_IMG_0);
   gh_utils_load_image(file_path, gamew);
   g_free (file_path);   
   
   /* Initialize image counter. */
   gamew->n_img = 1;

   /* Prepare status bar context. */
   gamew->scontext = gtk_statusbar_get_context_id (GTK_STATUSBAR (gamew->statusbar),
                                                   "Statusbar");
   /* Select the file and status bar message depending on the selected theme. */
   switch (gamew->theme_id)
   {
      case 0:
         markup = gh_utils_format_text_with_markup (_("Guess the FILM by typing letters"), 1); 
         file = g_file_new_for_path (file_path = gh_utils_get_system_file(FILMS_FILE));
         gtk_statusbar_push (GTK_STATUSBAR (gamew->statusbar), 
                             GPOINTER_TO_INT (gamew), _("Playing theme: Films"));
         break;

      case 1:
         markup = gh_utils_format_text_with_markup (_("Guess the OBJECT by typing letters"), 1); 
         file = g_file_new_for_path (file_path = gh_utils_get_system_file (OBJECTS_FILE));
         gtk_statusbar_push (GTK_STATUSBAR (gamew->statusbar),
                             GPOINTER_TO_INT (gamew), _("Playing theme: Objects"));
         break;

      case 2:
         markup = gh_utils_format_text_with_markup (_("Guess the PERSON by typing letters"), 1); 
         file = g_file_new_for_path (file_path = gh_utils_get_system_file(PERSONS_FILE));
         gtk_statusbar_push (GTK_STATUSBAR (gamew->statusbar),
                             GPOINTER_TO_INT (gamew), _("Playing theme: Persons"));
         break;

      default:
         g_critical ("Creating GFIle failed\n");
   }
   g_free (file_path);
  
   /* Sets label with markup. */ 
   gtk_label_set_markup (GTK_LABEL (gamew->title_label), markup);
   g_free (markup);

   /* Create stream for reading the file. */
   if (!(fstream = g_file_read (file, NULL, &error)))
   {
      g_critical ("Creating file stream error: %s", error->message);
      g_error_free (error);
   }
   g_object_unref (file);
   
   stream = g_data_input_stream_new (G_INPUT_STREAM(fstream));
   g_object_unref (fstream);

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
   g_object_unref (stream);
   
   /* Format the sentence for displaying. */
   g_free (gamew->display_sentence);
   gamew->display_sentence = g_strdup (gamew->sentence);
   gamew->valid_chars = " ";
   g_strcanon ( gamew->display_sentence, gamew->valid_chars, '_');
   
   /* Format markup for new game. */
   markup = gh_utils_format_text_with_markup (gamew->display_sentence, 0); 
   gtk_label_set_markup (GTK_LABEL (gamew->display_label), markup);
}
