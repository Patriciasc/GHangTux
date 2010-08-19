/*
 * ########################################################################
 * # File: GHangtux - ghangtux_ui.c                                       #
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
#include "ghangtux_management.h"
#include "ghangtux_utils.h"
#include "ghangtux.h"

/* ---- Private Methods ------ */
static void  ui_manager_init (Gamewidget *gamew);
static void  keyboard_init   (Gamewidget *gamew);
static void  set_logo        (Gamewidget *gamew);
static void  set_actions     (Gamewidget *gamew);

/* G_GNUC_UNUSED: For desabling warnings when
 * a parameter in a function is not used. */

/* ---- Action Callbacks ------ */
static void new_action      (G_GNUC_UNUSED GtkAction *action, gpointer data);
static void solve_action    (G_GNUC_UNUSED GtkAction *action, gpointer data);
static void quit_action     (G_GNUC_UNUSED GtkAction *action, gpointer data);
static void about_action    (G_GNUC_UNUSED GtkAction *action, gpointer data);

/* --- Signal callbacks --- */
/* G_MODULE_EXPORT: For windows applications to put the 
 * callbakcs in the symbol table. */

G_MODULE_EXPORT static void extern_key_release          (GtkWidget *window, GdkEventKey *event, 
                                                         gpointer data);
G_MODULE_EXPORT static void format_sentence_with_letter (GHangtuxKeyboard *keyboard, 
                                                         const gchar key_name, gpointer data);
G_MODULE_EXPORT void        main_win_destroy            (GtkObject *window, gpointer data);

/* ---------  START: list of actions  -----------*/

static GtkRadioActionEntry radio_actions[] =
{
  { "FilmsThemesMenuAction",     /* action name  */ 
    NULL,                        /* stock id     */
    N_("_Films"),                /* label        */
    "<Ctrl><Shift>f",            /* accelerator  */
    N_("Selects a random film"), /* tooltip      */
    0},                          /* action value */       

  { "ObjectsThemesMenuAction", 
    NULL, 
    N_("Objects"), 
    "<Ctrl><Shift>o", 
    N_("Selects a random object"), 
    1},

  { "PersonsThemesMenuAction", 
    NULL, 
    N_("Persons"), 
    "<Ctrl><Shift>p", 
    N_("Selects a random person"), 
    2}
};

static const guint NUM_RACTIONS = G_N_ELEMENTS (radio_actions);

static GtkActionEntry actions[] =
{
  { "GameMenuAction",            /* action name       */ 
     NULL,                       /* stock id          */
    N_("_Game"),                   /* label             */
    NULL,                        /* accelerator       */
    NULL,                        /* tooltip           */
    NULL                         /* action (callback) */
  },
  
  { "SettingsMenuAction",
    NULL,
    N_("_Settings"),
    NULL,
    NULL,
    NULL
  },

  { "ThemesMenuAction", 
     NULL, 
    N_("_Themes"),
    NULL,
    NULL,
    NULL
  },

  { "HelpMenuAction", 
    NULL, 
    N_("_Help"),
    NULL,
    NULL,
    NULL
  },

  { "NewMenuAction",
    GTK_STOCK_NEW,
    N_("_New"),
    NULL,
    N_("Prepares a new game for the actual theme"),
    G_CALLBACK (new_action)
  },
 
  { "SolveMenuAction",
    GTK_STOCK_APPLY,
    N_("S_olve"),
    "<Ctrl><Shift>q", 
    N_("Displays the solution for the actual game"),
    G_CALLBACK (solve_action)
  },

  { "QuitMenuAction", 
    GTK_STOCK_QUIT, 
    N_("_Quit"), 
    "<Ctrl><Shift>q", 
    N_("Quit the application"), 
    G_CALLBACK (quit_action)
  },

  { "AboutHelpMenuAction", 
    GTK_STOCK_ABOUT, 
    N_("_About"), 
    "<Ctrl><Shift>a", 
    N_("More information about the application"), 
    G_CALLBACK (about_action) }
};

static const guint NUM_ACTIONS = G_N_ELEMENTS (actions);

/* ---------  END: list of actions  -----------*/

/* Set up UI. */
void 
gh_ui_init (Gamewidget *gamew)
{
   /* Load and set application logo. */
   set_logo (gamew);

   /* Set up UI manager: Actions, Menubar and Toolbar */
   ui_manager_init (gamew);
  
   /* Setting up the keyboard. */
   keyboard_init (gamew);

   /* Show game. */
   gtk_widget_show_all (gamew->window);
}

/* Load and set application logo. */
static void 
set_logo (Gamewidget *gamew)
{
   static gchar *logo_path = NULL;
   GError *error = NULL;

   /* XXX This is not the definitive image. */
   /* Looks for the logo in the installation path */
   logo_path = gh_utils_get_system_file (LOGO);
   gamew->logo = gdk_pixbuf_new_from_file (logo_path, &error);
   g_free (logo_path);
   
   /* Handle the error if there was one. */
   if (error)
   {
      if (error->domain == GDK_PIXBUF_ERROR)
         g_warning ("GdkPixbufError: %s\n", error->message);
      else if (error->domain == G_FILE_ERROR)
         g_warning ("GFileError: %s\n", error->message);
      else
         g_warning ("An error in the unexpected domain:%d has occurred!\n", error->domain);

      g_error_free (error);
   }
   
   /* Set application logo. */
   gtk_window_set_icon (GTK_WINDOW (gamew->window), gamew->logo);
}

/* Initialize UI manager: menu and toolbar. */
static void
ui_manager_init (Gamewidget *gamew)
{
   GError *error = NULL;   
   static gchar *ui_path = NULL;

   /* Define action group and set up actions. */
   set_actions (gamew);  

   /* Set up UI manager */
   gamew->ui_manager = gtk_ui_manager_new ();
   gtk_ui_manager_insert_action_group (gamew->ui_manager, gamew->def_group, 0);
   
   ui_path = gh_utils_get_system_file (UI_FILE);
   if (!gtk_ui_manager_add_ui_from_file (gamew->ui_manager, ui_path , &error))
   {
      g_critical ("Building menus failed: %s\n", error->message);
      g_error_free (error);
   }
   g_free (ui_path);

   gtk_window_add_accel_group (GTK_WINDOW (gamew->window), 
                               gtk_ui_manager_get_accel_group (gamew->ui_manager));
   
   /* Load menubar. */
   gamew->menubar = gtk_ui_manager_get_widget (gamew->ui_manager, MENU);
   gtk_box_pack_start (GTK_BOX (gamew->vbox), gamew->menubar, FALSE, FALSE, 0);
   gtk_box_reorder_child (GTK_BOX (gamew->vbox), gamew->menubar, 0);
   
   /* Load toolbar. */
   gamew->toolbar = gtk_ui_manager_get_widget (gamew->ui_manager, TOOLBAR);
   gtk_toolbar_set_style (GTK_TOOLBAR (gamew->toolbar), GTK_TOOLBAR_ICONS);
   gtk_box_pack_start (GTK_BOX (gamew->vbox), gamew->toolbar, FALSE, FALSE, 0);
   gtk_box_reorder_child (GTK_BOX (gamew->vbox), gamew->toolbar, 1);
}

/* Initialize the Keboard. */
static void
keyboard_init (Gamewidget *gamew)
{
   /* New keyboard. */
   gamew->keyboard = gh_keyboard_new();

   /* Using game's keyboard */
   g_signal_connect (gamew->keyboard, "key_clicked",
                     G_CALLBACK (format_sentence_with_letter), gamew);

   /* Using external keyboard */
   g_signal_connect (gamew->window, "key-release-event", 
                     G_CALLBACK (extern_key_release), gamew);

   gtk_box_pack_start (GTK_BOX (gamew->vbox2), gamew->keyboard, FALSE, FALSE, 0);
   gtk_box_reorder_child (GTK_BOX (gamew->vbox2), gamew->keyboard, 3);
}

/* Define action group and set up actions. */
static void
set_actions (Gamewidget *gamew)
{
   /* New action group. */
   gamew->def_group = gtk_action_group_new (ACTION_GROUP);
   
   /* Make string translations possible. */
   gtk_action_group_set_translation_domain (gamew->def_group, GETTEXT_PACKAGE);

   /* Add actions to action group. */
   gtk_action_group_add_actions (gamew->def_group, actions, NUM_ACTIONS, gamew);
   gtk_action_group_add_radio_actions (gamew->def_group, radio_actions, NUM_RACTIONS,
                                       0, G_CALLBACK (gh_ui_activate_radio_action), gamew);
}

/* Activates a radio action from the menu. */
void
gh_ui_activate_radio_action ( G_GNUC_UNUSED GtkRadioAction *raction,
                              GtkRadioAction *curr_raction,
                              gpointer data)
{
   Gamewidget *gamew = (Gamewidget *)data;
    
   /* Sets the current theme */
   gamew->theme_id = gtk_radio_action_get_current_value (curr_raction);
   
   /* Start a new game with the selected theme. */
   gh_management_new_game_start (gamew);
}

/* ---------  START: Action Callbacks  ---------*/

/* Loads a new game for the same current theme. */
static void 
new_action (G_GNUC_UNUSED GtkAction *action,
            gpointer data)
{
   Gamewidget *gamew = (Gamewidget *)data;
   gh_management_new_game_start (gamew);
}

/* Displays the solution for the current game. */
static void
solve_action ( G_GNUC_UNUSED GtkAction *action,
              gpointer data)
{
   gh_management_set_end_game (data, -1);
}

/* Quits the application from the menu. */
static void
quit_action (G_GNUC_UNUSED GtkAction *action,
             gpointer data)
{
   Gamewidget *gamew = (Gamewidget *)data;
   main_win_destroy (GTK_OBJECT (gamew->window), NULL);
}

/* XXX Still does not work */
/* Shows an about dialog from the application. */
static void
about_action ( G_GNUC_UNUSED GtkAction *action,
               gpointer data)
{
   Gamewidget *gamew = (Gamewidget *)data;

   gtk_show_about_dialog (GTK_WINDOW (gamew->window),
   "authors", "Patricia Santana Cruz",
   "program-name", "GHangtux",
   "version", "1.1",
   "copyright", "Copyright (C) 2010 Openismus GmbH",
   "translator-credits", "Patricia Santana Cruz",
   "comments", "GHangTux is a variation of the popular Hangman game."
   "license", "Free: (TODO: look for the right text).",
   "website", "http://github.com/Patriciasc/GHangTux", 
   "website-label", "http://github.com/Patriciasc/GHangTux",
   "documenters", "Patricia Santana Cruz",
   "artists", "Tux images Copyright Wikimedia:\nhttp://commons.wikimedia.org/wiki/File:Tux-G2.png. Jan Vansteenkiste.",
   NULL);
}
/* -----------  END: Action Callbacks  -----------*/

/* -----------  START: Signal Callbacks ----------*/

static void
extern_key_release  (G_GNUC_UNUSED GtkWidget *window, 
                     GdkEventKey *event,
                     gpointer data)
{
   Gamewidget *gamew = (Gamewidget *)data;
   gchar *up_char = g_ascii_strup(event->string,-1);

   format_sentence_with_letter (GHANGTUX_KEYBOARD (gamew->keyboard), *up_char, gamew);
}

/* Formats the sentence with a new letter for displaying.*/ 
static void
format_sentence_with_letter (G_GNUC_UNUSED  GHangtuxKeyboard *keyboard, 
                             const gchar key_name,
                             gpointer data)
{
   guint i = 0;
   gboolean valid_letter = 0;
	gchar *markup = NULL;
   gchar *image = NULL;
   Gamewidget *gamew = (Gamewidget *)data;
   guint length = strlen(gamew->sentence);
  
   /* Looks for the label's letter in the sentence. */
   for (i=0; i!=length; i++)
   { 
      if (gamew->sentence[i] == key_name)
      {
          valid_letter = 1;
      }
   }

   /* Add the letter as a valid character and displays it. */
   if (valid_letter == 1)
   {
      gamew->valid_chars = g_strconcat (gamew->valid_chars, &key_name, NULL);
      g_free (gamew->display_sentence);
      gamew->display_sentence = g_strdup (gamew->sentence);
      g_strcanon ( gamew->display_sentence, gamew->valid_chars, '_');
      markup = gh_utils_format_text_with_markup (gamew->display_sentence, 0); 
      gtk_label_set_markup (GTK_LABEL (gamew->display_label), markup);
      g_free (markup);
      
      /* Player wins. */
      if (g_strcmp0(gamew->sentence, gamew->display_sentence) == 0)
      {
         gh_management_set_end_game(gamew, TRUE);
      }
   }
   else
   { 
      /* Loads a new image of the Hangtux. */ 
      if (gamew->n_img < NUM_IMAGES) 
      {
         image = g_strdup_printf("images/Tux%i.png",gamew->n_img);
         gh_utils_load_image (gh_utils_get_system_file(image), gamew);
         gamew->n_img++;
      }
      /* Player loses. */
      else
      {
         gh_management_set_end_game(gamew,FALSE);
      }
   }
   g_free (image);
}

/* Destroy main window. */
void 
main_win_destroy (G_GNUC_UNUSED GtkObject *window, 
                  G_GNUC_UNUSED gpointer data)
{
   gtk_main_quit();
}

/* -----------  END: Signal Callbacks ----------*/
