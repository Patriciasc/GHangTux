/*
 * ########################################################################
 * # File: GHangtux - main.c                                              #
 * #                                                                      #
 * # Author: Patricia Santana Cruz  <patriciasc@openismus.com>            #
 * #                                                                      #
 * # Copyright (C) 2010 Openismus GmbH                                    #
 * #                                                                      #
 * # Version: 0.1                                                         #  
 * #                                                                      #
 * # Description: A variation of the Hangman game.                        #
 * #                                                                      #
 * # This program is free software; you can redistribute it and/or modify #
 * # it under the terms of the GNU General Public License as published by #
 * # the Free Software Foundation; either version 3 of the License,       #
 * # or (at your option) any later version.                               #
 * #                                                                      #
 * # This program is distributed in the hope that it will be useful,      #
 * # but WITHOUT ANY WARRANTY; without even the implied warranty of       #
 * # MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU    #
 * # General Public License for more details.                             #
 * #                                                                      #
 * # You should have received a copy of the GNU General Public License    #
 * # along with this program. If not, see <http://www.gnu.org/licenses/>. #
 * ########################################################################
*/

#include <gtk/gtk.h>
#include <glib.h>
#include <gio/gio.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <string.h>
#include <stdlib.h>
#include "config.h"
#include "keyboard.h"

#define TUX_IMG_0 "images/Tux0.png"
#define FILMS_FILE "themes/films.txt"
#define OBJECTS_FILE "themes/objects.txt"
#define PERSONS_FILE "themes/persons.txt"

#define GUI_FILE "ui/ghangtux.glade"
#define UI_FILE "ui/menu.ui"

#define ACTION_GROUP "MainActionGroup"
#define SENTENCE_LABEL "for_sentence_label"
#define TITLE_LABEL "for_title_label"
#define WIN "main_win"
#define VBOX "vbox"
#define VBOX2 "vbox2"
#define MENU "/MainMenu"
#define TOOLBAR "/MainToolbar"
#define IMAGE "hangtux_area"
#define STATUSBAR "statusbar"

#define MIN_RANDOM 1
#define MAX_RANDOM 41
#define NUM_IMAGES 7

/* Callbacks for signals. */
void main_win_destroy            (GtkObject *window, gpointer data);
void format_sentence_with_letter (Keyboard *keyboard, const gchar key_name,
                                  gpointer data);

/* Callbacks for actions. */
static void new_action          (GtkAction *action, gpointer data);
static void solve_action        (GtkAction *action, gpointer data);
static void quit_action         (GtkAction *action, gpointer data);
static void get_sentence_action (GtkRadioAction *raction,
                                 GtkRadioAction *curr_raction, gpointer data);
static void about_action        (GtkAction *action, gpointer data);

/* Auxiliary functions. */
static void  load_image               (const char *file_image);
static gchar *get_system_file         (const gchar *filename);
static void  set_end_game             (gpointer data, int winner);
const gchar  *format_text_with_markup (const gchar *text, int type);

/* --------------------------------------------------------*/
/* --------------  START: list of actions  ----------------*/
/* --------------------------------------------------------*/

static GtkRadioActionEntry radio_actions[] =
{
  { "FilmsThemesMenuAction",    /* action name  */ 
    NULL,                       /* stock id     */
    "_Films",                   /* label        */
    "<Ctrl><Shift>f",           /* accelerator  */
    "Selects a random film",    /* tooltip      */
    0},                         /* action value */       

  { "ObjectsThemesMenuAction", 
    NULL, 
    "_Objects", 
    "<Ctrl><Shift>o", 
    "Selects a random object", 
    1},

  { "PersonsThemesMenuAction", 
    NULL, 
    "_Person_s", 
    "<Ctrl><Shift>p", 
    "Selects a random person", 
    2}
};

static const guint NUM_RACTIONS = G_N_ELEMENTS (radio_actions);

static GtkActionEntry actions[] =
{
  { "GameMenuAction",            /* action name       */ 
     NULL,                       /* stock id          */
    "_Game",                     /* label             */
    NULL,                        /* accelerator       */
    NULL,                        /* tooltip           */
    NULL                         /* action (callback) */
  },
  
  { "SettingsMenuAction",
    NULL,
    "_Settings",
    NULL,
    NULL,
    NULL
  },

  { "ThemesMenuAction", 
     NULL, 
    "_Themes",
    NULL,
    NULL,
    NULL
  },

  { "HelpMenuAction", 
    NULL, 
    "_Help",
    NULL,
    NULL,
    NULL
  },

  { "NewMenuAction",
    GTK_STOCK_NEW,
    "_New",
    "<Ctrl><Shift>n",
    "Prepares a new game for the actual theme",
    G_CALLBACK (new_action)
  },
 
  { "SolveMenuAction",
    GTK_STOCK_APPLY,
    "_Solve",
    "<Ctrl><Shift>s",
    "Displays the solution for the actual game",
    G_CALLBACK (solve_action)
  },

  { "QuitMenuAction", 
    GTK_STOCK_QUIT, 
    "_Quit", 
    "<Ctrl><Shift>q",
    "Quit the application", 
    G_CALLBACK (quit_action)
  },

  { "AboutHelpMenuAction", 
    GTK_STOCK_ABOUT, 
    "_About", 
    "<Ctrl><Shift>a",
    "More information about the application", 
    G_CALLBACK (about_action) }
};

static const guint NUM_ACTIONS = G_N_ELEMENTS (actions);

/* --------------------------------------------------------*/
/* --------------  END: list of actions  ----------------*/
/* --------------------------------------------------------*/

typedef struct _GameWidget Gamewidget;

/* Actual display sentence management. */
struct _GameWidget
{
   gchar *sentence;           /* games's sentence */
   gchar *display_sentence;   /* displayed sentence */
   const gchar *valid_chars;  /* player asserted characters */
   GtkLabel *display_label;   /* label for display_sentence */
   GtkLabel *title_label;     /* title label */
   GtkImage *image;           /* displayed image */
   gint n_img;                /* current image number */
   gboolean first_game;       /* indicates if the current game is the first */
   GtkWidget *keyboard;       /* keyboard */
   GtkWidget *statusbar;      /* game status bar */
   gint scontext;             /* game status bar's context */
   gint theme_id;             /* current theme's identifier */
};

Gamewidget gamew;

int
main (int argc,
     char **argv)
{
   GtkBuilder *builder = NULL;
   GtkWidget *window = NULL;
   GtkWidget *vbox = NULL;
   GtkWidget *vbox2 = NULL;
   GtkWidget *menubar = NULL;
   GtkWidget *toolbar = NULL;
   GtkActionGroup *def_group = NULL;
   GtkUIManager *ui_manager = NULL;
   GtkRadioAction *raction_init = NULL;
   GError *error = NULL;

   gamew.sentence = NULL;
   gamew.display_sentence = NULL;
   gamew.valid_chars = NULL;
   gamew.display_label = NULL;
   gamew.title_label = NULL;
   gamew.image = NULL;
   gamew.n_img = 0;
   gamew.first_game = 1;
   gamew.keyboard = NULL;
   gamew.statusbar = NULL;
   gamew.scontext = 0;

   gtk_init (&argc, &argv);
   
   /* Setting up the builder. */
   builder = gtk_builder_new();
   
   if (!gtk_builder_add_from_file (builder, get_system_file (GUI_FILE), &error))
   {
     g_message ("Building builder failed: %s\n", error->message);
     g_error_free (error);
     return 1;
   }

   window = GTK_WIDGET (gtk_builder_get_object (builder,WIN));
   vbox = GTK_WIDGET (gtk_builder_get_object (builder, VBOX));
   gamew.display_label = GTK_LABEL (gtk_builder_get_object (builder, SENTENCE_LABEL));
   gamew.title_label = GTK_LABEL (gtk_builder_get_object (builder, TITLE_LABEL));
   gamew.image = GTK_IMAGE (gtk_builder_get_object (builder, IMAGE));
   gamew.statusbar = GTK_WIDGET (gtk_builder_get_object (builder, STATUSBAR));
   vbox2 = GTK_WIDGET (gtk_builder_get_object (builder, VBOX2));

   gtk_builder_connect_signals (builder,NULL);
   g_object_unref (G_OBJECT(builder));

   /* Setting up the UI manager (menu and toolbar). */ 
   def_group = gtk_action_group_new (ACTION_GROUP);
   gtk_action_group_add_actions (def_group, actions, NUM_ACTIONS, NULL);
   gtk_action_group_add_radio_actions (def_group, radio_actions, NUM_RACTIONS,
                                       0, G_CALLBACK (get_sentence_action),  NULL);
 
   ui_manager = gtk_ui_manager_new ();
   gtk_ui_manager_insert_action_group (ui_manager, def_group, 0);
   
   if (!gtk_ui_manager_add_ui_from_file (ui_manager, get_system_file (UI_FILE), &error))
   {
      g_message ("Building menus failed: %s\n", error->message);
      g_error_free (error);
      return 1;
   }
  
   gtk_window_add_accel_group (GTK_WINDOW (window), 
                               gtk_ui_manager_get_accel_group (ui_manager));

   menubar = gtk_ui_manager_get_widget (ui_manager, MENU);
   gtk_box_pack_start (GTK_BOX (vbox), menubar, FALSE, FALSE, 0);
   gtk_box_reorder_child (GTK_BOX (vbox), menubar, 0);
   
   toolbar = gtk_ui_manager_get_widget (ui_manager, "/MainToolbar");
   gtk_toolbar_set_style (GTK_TOOLBAR (toolbar), GTK_TOOLBAR_ICONS);
   gtk_box_pack_start (GTK_BOX (vbox), toolbar, FALSE, FALSE, 0);
   gtk_box_reorder_child (GTK_BOX (vbox), toolbar, 1);
   
   /* Setting up the keyboard. */
   gamew.keyboard = keyboard_new();
   g_signal_connect (gamew.keyboard, "key_clicked",
                     G_CALLBACK (format_sentence_with_letter), NULL);
   gtk_box_pack_start (GTK_BOX (vbox2), gamew.keyboard, FALSE, FALSE, 0);
   gtk_box_reorder_child (GTK_BOX (vbox2), gamew.keyboard, 3);

   gtk_widget_show_all (window);
  
   /* Preparing game. */
   raction_init = gtk_radio_action_new ("init", "init", "init", "init", 0);
   get_sentence_action(NULL,raction_init,NULL); 
   load_image(get_system_file(TUX_IMG_0));
   gamew.n_img ++;

   gtk_main();
   return 0;
} 

/********************************
 * START: Callbakcs for signals *
 * ******************************/

/* Destroy main window. */
void 
main_win_destroy (GtkObject *window, 
                  gpointer data)
{
   gtk_main_quit();
}

/* Formats the sentence with a new letter for displaying.*/ 
void
format_sentence_with_letter (Keyboard *keyboard, const gchar key_name, gpointer data)
{
   gint i = 0;
   gint valid_letter = 0;
	const gchar *markup = NULL;

   /* Looks for the label's letter in the sentence. */
   for (i=0; i!=strlen(gamew.sentence); i++)
   { 
      if (gamew.sentence[i] == key_name)
      {
          valid_letter = 1;
      }
   }

   /* Add the letter as a valid character and displays it. */
   if (valid_letter == 1)
   {
      gamew.valid_chars = g_strconcat (gamew.valid_chars, &key_name, NULL);
      gamew.display_sentence = g_strdup (gamew.sentence);
      g_strcanon ( gamew.display_sentence, gamew.valid_chars, '_');
      markup = format_text_with_markup (gamew.display_sentence, 0); 
      gtk_label_set_markup (GTK_LABEL (gamew.display_label), markup);
      
      /* Player wins. */
      if (g_strcmp0(gamew.sentence, gamew.display_sentence) == 0)
      {
         set_end_game(data, TRUE);
      }
   }
   else
   { 
      /* Loads a new image of the Hangtux. */ 
      if (gamew.n_img < NUM_IMAGES) 
      {
         load_image (get_system_file(g_strdup_printf("images/Tux%i.png",gamew.n_img)));
         gamew.n_img++;
      }
      /* Player loses. */
      else
      {
         set_end_game(data,FALSE);
      }
   }
}

/******************************
 * END: Callbacks for signals *
 * ****************************/

/********************************
 * START: Callbacks for actions *
 * ******************************/

/* Loads a game for the same current theme. */
static void
new_action (GtkAction *action,
            gpointer data)
{
   GtkRadioAction *raction_init = NULL;

   switch (gamew.theme_id)
   {
      case 0:
         raction_init = gtk_radio_action_new ("init", "init", "init", "init", 0);
         get_sentence_action(NULL,raction_init,NULL); 
         break;

      case 1:
         raction_init = gtk_radio_action_new ("init", "init", "init", "init", 1);
         get_sentence_action(NULL,raction_init,NULL); 
         break;

      case 2:
         raction_init = gtk_radio_action_new ("init", "init", "init", "init", 2);
         get_sentence_action(NULL,raction_init,NULL); 
         break;

      default:
         g_print (" Problem loading new game for same theme\n");
   }
}

/* Displays the solution for the current game. */
static void
solve_action (GtkAction *action,
              gpointer data)
{
   set_end_game (data, -1);
}

/* Quits the application from the menu. */
static void
quit_action (GtkAction *action,
             gpointer data)
{
   main_win_destroy (NULL,NULL);
}

/* Selects a random sentence or word from the game files. */
static void
get_sentence_action (GtkRadioAction *raction,
                     GtkRadioAction *curr_raction,
                     gpointer data)
{
   GFile *file = NULL;
   GError *error = NULL;
   GFileInputStream *fstream = NULL; 
   GDataInputStream *stream = NULL;
   const gchar *markup = NULL;
   gsize length = 0;
   gint random = 0;

   /* Not first game => Reset values and prepares new game. */
   if (!gamew.first_game)
   {
      keyboard_set_sensitive (KEYBOARD (gamew.keyboard), TRUE);
      gamew.valid_chars = NULL;
      load_image(get_system_file (TUX_IMG_0));
      gamew.n_img = 1;
   }

   /* Status bar context. */
   gamew.scontext = gtk_statusbar_get_context_id (
                    GTK_STATUSBAR (gamew.statusbar),"Statusbar");

   /* Select the file. */
   switch (gtk_radio_action_get_current_value (curr_raction))
   {
      case 0:
         gamew.theme_id = 0;
         markup = format_text_with_markup ("Guess the FILM by typing letters", 1); 
         file = g_file_new_for_path (get_system_file(FILMS_FILE));
         gtk_statusbar_push (GTK_STATUSBAR (gamew.statusbar), GPOINTER_TO_INT (data), "Playing theme: Films");
         break;

      case 1:
         gamew.theme_id = 1;
         markup = format_text_with_markup ("Guess the OBJECT by typing letters", 1); 
         file = g_file_new_for_path (get_system_file (OBJECTS_FILE));
         gtk_statusbar_push (GTK_STATUSBAR (gamew.statusbar), GPOINTER_TO_INT (data), "Playing theme: Objects");
         break;

      case 2:
         gamew.theme_id = 2;
         markup = format_text_with_markup ("Guess the PERSON by typing letters", 1); 
         file = g_file_new_for_path (get_system_file(PERSONS_FILE));
         gtk_statusbar_push (GTK_STATUSBAR (gamew.statusbar), GPOINTER_TO_INT (data), "Playing theme: Persons");
         break;

      default:
         g_print ("Creating GFIle failed\n");
   }
   
   /* Sets label with markup. */ 
   gtk_label_set_markup (GTK_LABEL (gamew.title_label), markup);

   if (!(fstream = g_file_read (file, NULL, &error)))
   {
      g_message ("Creating file stream error: %s", error->message);
      g_error_free (error);
   }

   stream = g_data_input_stream_new (G_INPUT_STREAM(fstream));

   /* Select a random sentence from the file. */
   random = g_random_int_range (MIN_RANDOM, MAX_RANDOM);

   while (((gamew.sentence = g_data_input_stream_read_line (stream, &length, NULL, 
            &error)) != NULL) && (random != 0))
   {
      random--;
   }
   
   /* Format the sentence for displaying the first time. */
   gamew.display_sentence = g_strdup (gamew.sentence);
   gamew.valid_chars = " ";
   g_strcanon ( gamew.display_sentence, gamew.valid_chars, '_');
   markup = format_text_with_markup (gamew.display_sentence, 0); 
   gtk_label_set_markup (GTK_LABEL (gamew.display_label), markup);
   
   g_free (markup);
}

/* Shows an about dialog from the application. */
static void
about_action (GtkAction *action,
              gpointer data){

  GtkWidget *dialog;
  GdkPixbuf *logo;
  GError *error = NULL;

  const gchar *authors[] = {
    "Patricia Santana Cruz", 
    NULL
  };

  const gchar *documenters[] = {
    "Patricia Santana Cruz",
    NULL
  };
  
  const gchar *art_work[] = {
     "Tux images in this game, are based on Wikimedia:\nhttp://commons.wikimedia.org/wiki/File:Tux-G2.png\nby Jan Vansteenkiste.",
     NULL
  };
     
  dialog = gtk_about_dialog_new ();
  
  /* XXX this is not the definitive image */
  logo = gdk_pixbuf_new_from_file (get_system_file (TUX_IMG_0), &error);

  /* Set the application logo or handle the error. */
  if (error == NULL)
    gtk_about_dialog_set_logo (GTK_ABOUT_DIALOG (dialog), logo);
  else
  {
    if (error->domain == GDK_PIXBUF_ERROR)
      g_print ("GdkPixbufError: %s\n", error->message);
    else if (error->domain == G_FILE_ERROR)
      g_print ("GFileError: %s\n", error->message);
    else
      g_print ("An error in the unexpected domain:%d has occurred!\n", error->domain);

    g_error_free (error);
  }

  /* Set application data that will be displayed in the main dialog. */
  gtk_about_dialog_set_program_name (GTK_ABOUT_DIALOG (dialog), "GHangTux");
  gtk_about_dialog_set_version (GTK_ABOUT_DIALOG (dialog), "0.1");
  gtk_about_dialog_set_copyright (GTK_ABOUT_DIALOG (dialog), 
                                  " Copyright (C) 2010 Openismus GmbH");
  gtk_about_dialog_set_comments (GTK_ABOUT_DIALOG (dialog), 
                                 "GHangTux is a variation of the popular Hangman game.");

  /* XXX Need to load the file with the text here */
  gtk_about_dialog_set_license (GTK_ABOUT_DIALOG (dialog), 
                                "Free: (TODO: look for the right text).");
  gtk_about_dialog_set_website (GTK_ABOUT_DIALOG (dialog), 
                                "http://github.com/Patriciasc/GHangTux");
  gtk_about_dialog_set_website_label (GTK_ABOUT_DIALOG (dialog), 
                                      "http://github.com/Patriciasc/GHangTux");

  gtk_about_dialog_set_authors (GTK_ABOUT_DIALOG (dialog), authors);
  gtk_about_dialog_set_documenters (GTK_ABOUT_DIALOG (dialog), documenters);
  gtk_about_dialog_set_translator_credits (GTK_ABOUT_DIALOG (dialog), 
                                           "Patricia Santana Cruz");
  gtk_about_dialog_set_artists (GTK_ABOUT_DIALOG (dialog), art_work); 


  gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);
}

/******************************
 * END: Callbacks for actions *
 * ****************************/

/******************************
 * START: Auxiliary functions *
 * ****************************/

/* Gets player's system files. */
static gchar *
get_system_file (const gchar *filename)
{
   gchar *pathname;
   const gchar* const *system_data_dirs;
   
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

/* Loads an image for the GtkImage central area. */
static void
load_image (const char *file_image)
{
   GdkPixbuf *tux_image = NULL;
   GError *error = NULL;
   
   tux_image = gdk_pixbuf_new_from_file (file_image, &error);

   if (error != NULL)
   {
      if (error->domain == GDK_PIXBUF_ERROR)
         g_print ("GdkPixbufError: %s\n", error->message);
      else if (error->domain == G_FILE_ERROR)
         g_print ("GFileError: %s\n", error->message);
      else
         g_print ("An error in the unexpected domain:%d has occurred!\n", error->domain);
 
      g_error_free (error);
   }
   
   gtk_image_set_from_pixbuf (gamew.image, tux_image); 
}

/* Sets up the end of the game. */
static void
set_end_game (gpointer data, int winner)
{
   const gchar *markup = NULL;
   
   /* Shows solution. */
   markup = format_text_with_markup (gamew.sentence, 0); 
   gtk_label_set_markup (GTK_LABEL (gamew.display_label), markup);
   g_free (markup);
       
   /* Change status bar state. */
   gamew.scontext = gtk_statusbar_get_context_id (GTK_STATUSBAR (gamew.statusbar),
                                                  "Statusbar");
   if (winner == 0)
   {
      load_image (get_system_file("images/Tux7.png"));
      gtk_statusbar_push (GTK_STATUSBAR (gamew.statusbar), 
                          GPOINTER_TO_INT (data), "End of game. Try again!");
   }
   else if (winner == 1)
   {
      load_image (get_system_file("images/Tux8.png"));
      gtk_statusbar_push (GTK_STATUSBAR (gamew.statusbar), 
                          GPOINTER_TO_INT (data), "Congratulations!");
   }
   else
   {
      load_image (get_system_file("images/Tux7.png"));
      gtk_statusbar_push (GTK_STATUSBAR (gamew.statusbar), 
                          GPOINTER_TO_INT (data), "Solution");
   }
      
   /* Set title label */
   gtk_label_set_text (gamew.title_label, " "); 

   /* Desable the use of the keys. */
   keyboard_set_sensitive (KEYBOARD (gamew.keyboard), FALSE);
   gamew.first_game = 0;
}

/* Formats the text with a markup before displaying it. */ 
const gchar *
format_text_with_markup (const gchar *text, int type)
{
   switch (type)
   {
      case 0:    /* Display label */
         g_strdup (g_strdup_printf("<span size=\"large\" font_weight=\"ultrabold\">%s</span>", text));
         break;
      case 1:    /* Title label */
         g_strdup (g_strdup_printf("<span size=\"small\">%s</span>", text));
         break;
      default:
         g_print ("Problem setting markups\n");
   }
}

/****************************
 * END: Auxiliary functions *
 * **************************/
