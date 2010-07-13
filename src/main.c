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

#define TUX_IMG_0 "../tux_images/Tux0.png"
#define FILMS_FILE "../films.txt"
#define OBJECTS_FILE "../objects.txt"
#define PERSONS_FILE "../persons.txt"
#define TRANSLATOR "Patricia Santana Cruz"

#define GUI_FILE "ghangtux.glade"
#define UI_FILE "menu.ui"
#define ACTION_GROUP "MainActionGroup"
#define WIN "main_win"
#define VBOX "vbox"
#define MENU "/MainMenu"
#define IMAGE "hangtux_area"
#define STATUSBAR "statusbar"
#define KEYBOARD "keyboard_EN"
#define SENTENCE_LABEL "for_sentence_label"

#define MIN_RANDOM 1
#define MAX_RANDOM 41
#define NUM_IMAGES 8

void main_win_destroy (GtkObject *window, gpointer data);
static void get_sentence_action (GtkRadioAction *raction,
                                 GtkRadioAction *curr_raction, gpointer data);
static void quit_action (GtkAction *action, gpointer data);
static void about_action (GtkAction *action, gpointer data);
void format_sentence_with_letter (GtkButton *button, gpointer data);
static void load_image (const char *file_image);
static void set_keyboard_active (gboolean active);

/* --------------------------------------------------------*/
/* --------------  START: list of actions  ----------------*/
/* --------------------------------------------------------*/

static GtkRadioActionEntry radio_actions[] =
{
  { "FilmsThemesMenuAction", 
    NULL, 
    "_Films", 
    "<Ctrl><Shift>f",
    "Selects a random film", 
    0},

  { "ObjectsThemesMenuAction", 
    NULL, 
    "_Objects", 
    "<Ctrl><Shift>o", 
    "Select a random object", 
    1},

  { "PersonsThemesMenuAction", 
    NULL, 
    "_Person_s", 
    "<Ctrl><Shift>p", 
    "Select a random person", 
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

  { "HelpMenuAction", 
    NULL, 
    "_Help",
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

  { "QuitMenuAction", 
    GTK_STOCK_QUIT, 
    "_Quit", 
    "<Ctrl><Shift>q",
    "Quit the application", 
    G_CALLBACK (quit_action) },

  /* Help menu action. */   
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
typedef struct _Keyboard   Keyboard;

/* Actual display sentence management. */
struct _GameWidget
{
   gchar *sentence;           /* games's sentence */
   gchar *display_sentence;   /* displayed sentence */
   const gchar *valid_chars;  /* user asserted characters */
   GtkLabel *display_label;   /* label for display_sentence */
   GtkImage *image;           /* displayed image */
   gint n_img;                /* current image number */
   gboolean first_game;       /* indicates if the current game is the first */
   GtkWidget *statusbar;      /* game status bar */
   gint scontext;             /* game status bar's context */
};

/* Keyboard management */
struct _Keyboard
{
   GtkContainer *table;
   GList *keys;
   GList *index;
};

Gamewidget gamew;
Keyboard keyboard;

int
main (int argc,
     char **argv)
{
   GtkBuilder *builder = NULL;
   GtkWidget *window = NULL;
   GtkWidget *vbox = NULL;
   GtkWidget *menubar = NULL;
   GtkActionGroup *def_group = NULL;
   GtkUIManager *uimanager = NULL;
   GError *error = NULL;

   gamew.sentence = NULL;
   gamew.display_sentence = NULL;
   gamew.valid_chars = NULL;
   gamew.display_label = NULL;
   gamew.image = NULL;
   gamew.n_img = 0;
   gamew.first_game = 1;
   gamew.statusbar = NULL;
   gamew.scontext = 0;

   keyboard.table = NULL;
   keyboard.keys = NULL;
   keyboard.index = NULL;

   gtk_init (&argc, &argv);
   
   /* Setting up the builder. */
   builder = gtk_builder_new();

   if (!gtk_builder_add_from_file (builder, GUI_FILE, &error))
   {
     g_message ("%s", error->message);
     g_error_free (error);
     return 1;
   }

   window = GTK_WIDGET (gtk_builder_get_object (builder,WIN));
   vbox = GTK_WIDGET (gtk_builder_get_object (builder, VBOX));
   gamew.display_label = GTK_LABEL (gtk_builder_get_object (builder, SENTENCE_LABEL));
   gamew.image = GTK_IMAGE (gtk_builder_get_object (builder, IMAGE));
   gamew.statusbar = GTK_WIDGET (gtk_builder_get_object (builder, STATUSBAR));
   keyboard.table = GTK_CONTAINER (gtk_builder_get_object (builder, KEYBOARD));
   keyboard.keys = gtk_container_get_children (keyboard.table);
   keyboard.index = keyboard.keys;

   gtk_builder_connect_signals (builder,NULL);
   g_object_unref (G_OBJECT(builder));

   /* Setting up UI manager. */ 
   def_group = gtk_action_group_new (ACTION_GROUP);
   gtk_action_group_add_actions (def_group, actions, NUM_ACTIONS, NULL);
   gtk_action_group_add_radio_actions (def_group, radio_actions, NUM_RACTIONS,
                                       0, G_CALLBACK (get_sentence_action),  NULL);
 
   uimanager = gtk_ui_manager_new ();
   gtk_ui_manager_insert_action_group (uimanager, def_group, 0);
   
   if (!gtk_ui_manager_add_ui_from_file (uimanager, UI_FILE, &error))
   {
      g_message ("Building menus failed: %s", error->message);
      g_error_free (error);
      return 1;
   }
  
   menubar = gtk_ui_manager_get_widget (uimanager, MENU);
   gtk_window_add_accel_group (GTK_WINDOW (window), 
                               gtk_ui_manager_get_accel_group (uimanager));

   gtk_box_pack_start (GTK_BOX (vbox), menubar, FALSE, FALSE, 1);
   gtk_box_reorder_child (GTK_BOX (vbox), menubar, 0);

   gtk_widget_show_all (window);
   
   /* Prepares game. */
   get_sentence_action(NULL,0,NULL);
   load_image(TUX_IMG_0);
   gamew.n_img ++;

   gtk_main();
   return 0;
} 

/* Destroy main window. */
void 
main_win_destroy (GtkObject *window, 
                  gpointer data)
{
   gtk_main_quit();
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
   gsize length = 0;
   gint random = 0;

   /* Not first game => Reset values and prepares new game. */
   if (!gamew.first_game)
   {
      gamew.valid_chars = NULL;
      load_image(TUX_IMG_0);
      gamew.n_img = 1;
      set_keyboard_active (TRUE);
   }

   /* Status bar context. */
   gamew.scontext = gtk_statusbar_get_context_id (
                    GTK_STATUSBAR (gamew.statusbar),"Statusbar");
   /* Select the file. */
   switch (gtk_radio_action_get_current_value (curr_raction))
   {
      case 0:
         file = g_file_new_for_path (FILMS_FILE);
         gtk_statusbar_push (GTK_STATUSBAR (gamew.statusbar), GPOINTER_TO_INT (data), "Playing theme: Films");
         break;

      case 1:
         file = g_file_new_for_path (OBJECTS_FILE);
         gtk_statusbar_push (GTK_STATUSBAR (gamew.statusbar), GPOINTER_TO_INT (data), "Playing theme: Objects");
         break;

      case 2:
         file = g_file_new_for_path (PERSONS_FILE);
         gtk_statusbar_push (GTK_STATUSBAR (gamew.statusbar), GPOINTER_TO_INT (data), "Playing theme: Persons");
         break;

      default:
         g_print ("Creating GFIle failed\n");
   }
   
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
   gtk_label_set_text (gamew.display_label, gamew.display_sentence);
}

/* Formats the sentence with a new letter for displaying. */
void
format_sentence_with_letter (GtkButton *button, gpointer data)
{
   const gchar *label = NULL;
   gint i = 0;
   gint valid_letter = 0;
   gchar *letter = NULL;
   gchar *markup = NULL;
	
   /* Make button unsensitive. */
   gtk_widget_set_sensitive(GTK_WIDGET(button),FALSE);

   label = gtk_button_get_label (GTK_BUTTON (button)); 
   letter = g_strdup (label);

   /* Looks for the label's letter in the sentence. */
   for (i=0; i!=strlen(gamew.sentence); i++)
   { 
      if (gamew.sentence[i] == letter[1])
      {
          valid_letter = 1;
      }
   }

   /* Add the letter as a valid character and displays it. */
   if (valid_letter == 1)
   {
      gamew.valid_chars = g_strconcat (gamew.valid_chars, letter, NULL);
      gamew.display_sentence = g_strdup (gamew.sentence);
      g_strcanon ( gamew.display_sentence, gamew.valid_chars, '_');
      gtk_label_set_text (gamew.display_label, gamew.display_sentence);
   }
   /* Loads a new image of the Hangtux. */
   else
   { 
      load_image (g_strdup_printf("../tux_images/Tux%i.png",gamew.n_img));
      gamew.n_img ++;

      if (gamew.n_img == NUM_IMAGES)
      {
         gtk_label_set_text (gamew.display_label, " ");
         load_image("../tux_images/final.png");

         /* Change status bar state. */
         gamew.scontext = gtk_statusbar_get_context_id (GTK_STATUSBAR (gamew.statusbar),
                                                        "Statusbar");
         gtk_statusbar_push (GTK_STATUSBAR (gamew.statusbar), 
                             GPOINTER_TO_INT (data), "End of game. Try again!");

         /* Desable the use of the keys. */
         set_keyboard_active (FALSE);
         gamew.first_game = 0;
         g_free (markup);
      }
   }

}

/* Enables or desables keyboardi's keys. */
static void
set_keyboard_active (gboolean active)
{
   while (keyboard.index != NULL)
   {
      gtk_widget_set_sensitive (GTK_WIDGET (keyboard.index->data), active);
      keyboard.index = keyboard.index->next;
   }
   keyboard.index = keyboard.keys;
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

/* Quits the application from the menu. */
static void
quit_action (GtkAction *action,
             gpointer data)
{
   main_win_destroy (NULL,NULL);
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
  logo = gdk_pixbuf_new_from_file (TUX_IMG_0, &error);

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
                                  "(C) 2010 Patricia Santana Cruz");
  gtk_about_dialog_set_comments (GTK_ABOUT_DIALOG (dialog), 
                                 "GHangTux is a variation of the popular Hangman game.");

  /* XXX Need to load the file with the text here */
  /* XXX Use #define for texts!! */
  gtk_about_dialog_set_license (GTK_ABOUT_DIALOG (dialog), 
                                "Free: (TODO: look for the right text).");
  gtk_about_dialog_set_website (GTK_ABOUT_DIALOG (dialog), 
                                "http://github.com/Patriciasc/GHangTux");
  gtk_about_dialog_set_website_label (GTK_ABOUT_DIALOG (dialog), 
                                      "http://github.com/Patriciasc/GHangTux");

  gtk_about_dialog_set_authors (GTK_ABOUT_DIALOG (dialog), authors);
  gtk_about_dialog_set_documenters (GTK_ABOUT_DIALOG (dialog), documenters);
  gtk_about_dialog_set_translator_credits (GTK_ABOUT_DIALOG (dialog), 
                                           TRANSLATOR);
  gtk_about_dialog_set_artists (GTK_ABOUT_DIALOG (dialog), art_work); 


  gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);
}
