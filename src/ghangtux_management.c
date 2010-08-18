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
#include "main.h"
#include "config.h"

/* Sets up the end of the game. */
void
gh_management_set_end_game (gpointer data, int winner)
{
   Gamewidget *gamew = (Gamewidget *)data;
   static gchar *sys_file = NULL;
   gchar *markup = NULL;
   
   /* Shows solution. */
   markup = gh_utils_format_text_with_markup (gamew->sentence, 0); 
   gtk_label_set_markup (GTK_LABEL (gamew->display_label), markup);
   g_free (markup);
       
   /* Change status bar state. */
   gamew->scontext = gtk_statusbar_get_context_id (GTK_STATUSBAR (gamew->statusbar),
                                                  "Statusbar");
   if (winner == 0)
   {
      gh_utils_load_image (sys_file = gh_utils_get_system_file("images/Tux7.png"), gamew);
      gtk_statusbar_push (GTK_STATUSBAR (gamew->statusbar), 
                          GPOINTER_TO_INT (gamew), _("End of game. Try again!"));
   }
   else if (winner == 1)
   {
      gh_utils_load_image (sys_file = gh_utils_get_system_file("images/Tux8.png"), gamew);
      gtk_statusbar_push (GTK_STATUSBAR (gamew->statusbar), 
                          GPOINTER_TO_INT (gamew), _("Congratulations!"));
   }
   else
   {
      gh_utils_load_image (sys_file = gh_utils_get_system_file("images/Tux7.png"), gamew);
      gtk_statusbar_push (GTK_STATUSBAR (gamew->statusbar), 
                          GPOINTER_TO_INT (gamew), _("Solution"));
   }

   /* Set title label */
   gtk_label_set_text (gamew->title_label, " "); 

   /* Desable the use of the keys. */
   gh_keyboard_set_sensitive (GHANGTUX_KEYBOARD (gamew->keyboard), FALSE);
   gamew->first_game = FALSE;
}

/* Initializes game the first time. */
void
gh_management_game_init (Gamewidget *gamew)
{
   GtkAction *action = NULL;
   static gchar *init_image_path = NULL;

   action = gtk_action_group_get_action (gamew->def_group, "FilmsThemesMenuAction");

   if (!action)
   {
      g_critical ("Problem loading action for initializing game\n");
   }

   /* Activates "Film" radio action in the menu. */
   gh_ui_activate_radio_action (NULL, GTK_RADIO_ACTION (action), gamew);

   /* Load initial tux image. */
   init_image_path = gh_utils_get_system_file(TUX_IMG_0);
   gh_utils_load_image (init_image_path, gamew);
   gamew->n_img ++;

   /* Free memory and unref GObjects. */
   g_free (init_image_path);
   g_object_unref (gamew->ui_manager);

   gtk_main();
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
     g_free (glade_file);
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

   gtk_builder_connect_signals (gamew->builder,NULL);
   g_object_unref (G_OBJECT(gamew->builder));
}

/* Initialize game's widgets. */
Gamewidget
gh_management_gamew_init ()
{
   Gamewidget gamew ={ NULL, NULL, NULL, 
                        NULL, NULL, NULL, 
                        NULL, NULL, NULL,
                        NULL, NULL, NULL,
                        NULL, NULL, NULL,
                        0,    TRUE, NULL,
                        NULL, 0, 0, NULL};
  return gamew;
}

/* Import languages for the application. */
void
gh_management_import_languages ()
{
   bindtextdomain(GETTEXT_PACKAGE, PROGRAMNAME_LOCALEDIR);
   bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
   textdomain(GETTEXT_PACKAGE);
}
