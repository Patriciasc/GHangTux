/*
 * ########################################################################
 * # File: GHangtux - main.c                                              #
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
#include "keyboard.h"

#define TUX_IMG_0 "images/Tux0.png"
#define FILMS_FILE "themes/films.txt"
#define OBJECTS_FILE "themes/objects.txt"
#define PERSONS_FILE "themes/persons.txt"
#define LOGO "icons/hicolor/200x200/apps/ghangtux.png"

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
#define EVENTBOX "eventbox"

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
static void extern_key_release  (GtkWidget *window, GdkEventKey *event, gpointer data);

/* Auxiliary functions. */
static void  load_image               (const char *file_image);
static gchar *get_system_file         (const gchar *filename);
static void  set_end_game             (gpointer data, int winner);
gchar  *format_text_with_markup       (const gchar *text, int type);

/* --------------------------------------------------------*/
/* --------------  START: list of actions  ----------------*/
/* --------------------------------------------------------*/

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
   GdkPixbuf *logo;            /* application's logo */
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
   GtkWidget *eventbox = NULL;
   GtkActionGroup *def_group = NULL;
   GtkUIManager *ui_manager = NULL;
   GtkRadioAction *raction_init = NULL;
   GError *error = NULL;
   static gchar *sys_file = NULL;
   GdkColor image_bg;

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
   gamew.logo = NULL;

   image_bg.red = 50535;
   image_bg.green = 55535;
   image_bg.blue = 60535;

   gtk_init (&argc, &argv);
  
   /* Internationalization i18n. */
   bindtextdomain(GETTEXT_PACKAGE, PROGRAMNAME_LOCALEDIR);
   bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
   textdomain(GETTEXT_PACKAGE);
   
   /* Setting up the builder. */
   builder = gtk_builder_new();
   
   if (!gtk_builder_add_from_file (builder, sys_file = get_system_file (GUI_FILE), &error))
   {
     g_message ("Building builder failed: %s\n", error->message);
     g_error_free (error);
     return 1;
   }

   g_free (sys_file);

   window = GTK_WIDGET (gtk_builder_get_object (builder,WIN));
   vbox = GTK_WIDGET (gtk_builder_get_object (builder, VBOX));
   gamew.display_label = GTK_LABEL (gtk_builder_get_object (builder, SENTENCE_LABEL));
   gamew.title_label = GTK_LABEL (gtk_builder_get_object (builder, TITLE_LABEL));
   gamew.image = GTK_IMAGE (gtk_builder_get_object (builder, IMAGE));
   gamew.statusbar = GTK_WIDGET (gtk_builder_get_object (builder, STATUSBAR));
   vbox2 = GTK_WIDGET (gtk_builder_get_object (builder, VBOX2));
   eventbox = GTK_WIDGET (gtk_builder_get_object (builder, EVENTBOX));
   gtk_widget_modify_bg (eventbox, 0, &image_bg);
   
    
   /* XXX this is not the definitive image */
   gamew.logo = gdk_pixbuf_new_from_file (sys_file = get_system_file (LOGO), &error);
   
   /* Set the application logo or handle the error. */
   if (error)
   {
      if (error->domain == GDK_PIXBUF_ERROR)
         g_print ("GdkPixbufError: %s\n", error->message);
      else if (error->domain == G_FILE_ERROR)
         g_print ("GFileError: %s\n", error->message);
      else
         g_print ("An error in the unexpected domain:%d has occurred!\n", error->domain);

      g_error_free (error);
   }

   g_free (sys_file);

   gtk_window_set_icon (GTK_WINDOW (window), gamew.logo);

   gtk_builder_connect_signals (builder,NULL);
   g_object_unref (G_OBJECT(builder));

   /* Setting up the UI manager (menu and toolbar). */ 
   def_group = gtk_action_group_new (ACTION_GROUP);
   gtk_action_group_set_translation_domain (def_group, GETTEXT_PACKAGE);
   gtk_action_group_add_actions (def_group, actions, NUM_ACTIONS, NULL);
   gtk_action_group_add_radio_actions (def_group, radio_actions, NUM_RACTIONS,
                                       0, G_CALLBACK (get_sentence_action),  NULL);
 
   ui_manager = gtk_ui_manager_new ();
   gtk_ui_manager_insert_action_group (ui_manager, def_group, 0);
   
   if (!gtk_ui_manager_add_ui_from_file (ui_manager, sys_file = get_system_file (UI_FILE), &error))
   {
      g_message ("Building menus failed: %s\n", error->message);
      g_error_free (error);
      return 1;
   }
  
   g_free (sys_file);

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
   /* Using game's keyboard */
   g_signal_connect (gamew.keyboard, "key_clicked",
                     G_CALLBACK (format_sentence_with_letter), NULL);
   /* Using external keyboard */
   g_signal_connect (window, "key-release-event", 
                     G_CALLBACK (extern_key_release), NULL);
   gtk_box_pack_start (GTK_BOX (vbox2), gamew.keyboard, FALSE, FALSE, 0);
   gtk_box_reorder_child (GTK_BOX (vbox2), gamew.keyboard, 3);

   gtk_widget_show_all (window);
  
   /* Preparing game. */
   raction_init = gtk_radio_action_new ("init", "init", "init", "init", 0);
   get_sentence_action(NULL,raction_init,NULL); 
   load_image (sys_file = get_system_file(TUX_IMG_0));

   g_free (sys_file);
   g_object_unref (ui_manager);
   g_object_unref (raction_init);
   gamew.n_img ++;

   gtk_main();
   return 0;
} 

/* G_GNUC_UNUSED is used up this point for desabling
 * warnings when a parameter in a function is not
 * used */

/********************************
 * START: Callbakcs for signals *
 * ******************************/

/* Destroy main window. */
void 
main_win_destroy (G_GNUC_UNUSED GtkObject *window, 
                  G_GNUC_UNUSED gpointer data)
{
   gtk_main_quit();
}

/* Formats the sentence with a new letter for displaying.*/ 
void
format_sentence_with_letter (G_GNUC_UNUSED Keyboard *keyboard, 
                             const gchar key_name,
                             gpointer data)
{
   guint i = 0;
   gint valid_letter = 0;
	gchar *markup = NULL;
   gchar *image = NULL;
   guint length = strlen(gamew.sentence);
  
   g_print("LETRA: %c\n",key_name);
  
   /* Looks for the label's letter in the sentence. */
   for (i=0; i!=length; i++)
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
      g_free (gamew.display_sentence);
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
         image = g_strdup_printf("images/Tux%i.png",gamew.n_img);
         load_image (get_system_file(image));
         gamew.n_img++;
      }
      /* Player loses. */
      else
      {
         set_end_game(data,FALSE);
      }
   }
   
   g_free (image);
   g_free (markup);
}

static void
extern_key_release  (G_GNUC_UNUSED GtkWidget *window, 
                     GdkEventKey *event,
                     G_GNUC_UNUSED gpointer data)
{
   gchar *up_char = g_ascii_strup(event->string,-1);
   format_sentence_with_letter (KEYBOARD (gamew.keyboard), *up_char, NULL);
}

/******************************
 * END: Callbacks for signals *
 * ****************************/

/********************************
 * START: Callbacks for actions *
 * ******************************/

/* Loads a game for the same current theme. */
static void
new_action (G_GNUC_UNUSED GtkAction *action,
            G_GNUC_UNUSED gpointer data)
{
   GtkRadioAction *raction_init = NULL;

   switch (gamew.theme_id)
   {
      case 0:
         raction_init = gtk_radio_action_new ("init", "init", "init", "init", 0);
         break;

      case 1:
         raction_init = gtk_radio_action_new ("init", "init", "init", "init", 1);
         break;

      case 2:
         raction_init = gtk_radio_action_new ("init", "init", "init", "init", 2);
         break;

      default:
         g_print (" Problem loading new game for same theme\n");
         break;
   }
   
   get_sentence_action(NULL,raction_init,NULL); 
   
   g_object_unref (raction_init);
}

/* Displays the solution for the current game. */
static void
solve_action ( G_GNUC_UNUSED GtkAction *action,
              gpointer data)
{
   set_end_game (data, -1);
}

/* Quits the application from the menu. */
static void
quit_action (G_GNUC_UNUSED GtkAction *action,
             G_GNUC_UNUSED gpointer data)
{
   main_win_destroy (NULL,NULL);
}

/* Selects a random sentence or word from the game files. */
static void
get_sentence_action ( G_GNUC_UNUSED GtkRadioAction *raction,
                     GtkRadioAction *curr_raction,
                     gpointer data)
{
   GFile *file = NULL;
   GError *error = NULL;
   GFileInputStream *fstream = NULL; 
   GDataInputStream *stream = NULL;
   static gchar *sys_file = NULL;
   gchar *markup = NULL;
   gsize length = 0;
   gint random = 0;

   /* Not first game => Reset values and prepares new game. */
   if (!gamew.first_game)
   {
      keyboard_set_sensitive (KEYBOARD (gamew.keyboard), TRUE);
      gamew.valid_chars = NULL;
      load_image(sys_file = get_system_file (TUX_IMG_0));
      gamew.n_img = 1;
   }
 
   g_free (sys_file);   

   /* Status bar context. */
   gamew.scontext = gtk_statusbar_get_context_id (
                    GTK_STATUSBAR (gamew.statusbar),"Statusbar");

   /* Select the file. */
   switch (gtk_radio_action_get_current_value (curr_raction))
   {
      case 0:
         gamew.theme_id = 0;
         markup = format_text_with_markup (_("Guess the FILM by typing letters"), 1); 
         file = g_file_new_for_path (sys_file = get_system_file(FILMS_FILE));
         gtk_statusbar_push (GTK_STATUSBAR (gamew.statusbar), GPOINTER_TO_INT (data), _("Playing theme: Films"));
         break;

      case 1:
         gamew.theme_id = 1;
         markup = format_text_with_markup (_("Guess the OBJECT by typing letters"), 1); 
         file = g_file_new_for_path (sys_file = get_system_file (OBJECTS_FILE));
         gtk_statusbar_push (GTK_STATUSBAR (gamew.statusbar), GPOINTER_TO_INT (data), _("Playing theme: Objects"));
         break;

      case 2:
         gamew.theme_id = 2;
         markup = format_text_with_markup (_("Guess the PERSON by typing letters"), 1); 
         file = g_file_new_for_path (sys_file = get_system_file(PERSONS_FILE));
         gtk_statusbar_push (GTK_STATUSBAR (gamew.statusbar), GPOINTER_TO_INT (data), _("Playing theme: Persons"));
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
   
   g_free (markup);

   stream = g_data_input_stream_new (G_INPUT_STREAM(fstream));

   /* Select a random sentence from the file. */
   random = g_random_int_range (MIN_RANDOM, MAX_RANDOM);
   
   while (((gamew.sentence = g_data_input_stream_read_line (stream, &length, NULL, 
            &error)) != NULL) && (random != 0))
   {
      if (error)
      {
         g_message ("Building menus failed: %s\n", error->message);
         g_error_free (error);
      }
      else
      {
         random--;
         g_free (gamew.sentence);
      }
   }
   
   /* Format the sentence for displaying the first time. */
   g_free (gamew.display_sentence);
   gamew.display_sentence = g_strdup (gamew.sentence);
   gamew.valid_chars = " ";
   g_strcanon ( gamew.display_sentence, gamew.valid_chars, '_');
   markup = format_text_with_markup (gamew.display_sentence, 0); 
   gtk_label_set_markup (GTK_LABEL (gamew.display_label), markup);
   
   g_object_unref (file);
   g_object_unref (stream);
   g_object_unref (fstream);
   g_free (sys_file);
   g_free (markup);
}

/* Shows an about dialog from the application. */
static void
about_action ( G_GNUC_UNUSED GtkAction *action,
               G_GNUC_UNUSED gpointer data){

  GtkWidget *dialog;

  const gchar *authors[] = {
    "Patricia Santana Cruz", 
    NULL
  };

  const gchar *documenters[] = {
    "Patricia Santana Cruz",
    NULL
  };
  
  const gchar *art_work[] = {
     "Tux images Copyright Wikimedia:\nhttp://commons.wikimedia.org/wiki/File:Tux-G2.png. Jan Vansteenkiste.",
     NULL
  };
     
  dialog = gtk_about_dialog_new ();
  
  /* Set logo. */
  gtk_about_dialog_set_logo (GTK_ABOUT_DIALOG (dialog), gamew.logo);

  /* Set application data that will be displayed in the main dialog. */
  gtk_about_dialog_set_program_name (GTK_ABOUT_DIALOG (dialog), "GHangTux");
  gtk_about_dialog_set_version (GTK_ABOUT_DIALOG (dialog), "1.1");
  gtk_about_dialog_set_copyright (GTK_ABOUT_DIALOG (dialog), 
                                  " Copyright (C) 2010 Openismus GmbH");
  gtk_about_dialog_set_comments (GTK_ABOUT_DIALOG (dialog), 
                                 _("GHangTux is a variation of the popular Hangman game."));

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

  g_object_unref (gamew.logo);

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

   if (error)
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
   g_object_unref (G_OBJECT (tux_image));
}

/* Sets up the end of the game. */
static void
set_end_game (gpointer data, int winner)
{
   static gchar *sys_file = NULL;
   gchar *markup = NULL;
   
   /* Shows solution. */
   markup = format_text_with_markup (gamew.sentence, 0); 
   gtk_label_set_markup (GTK_LABEL (gamew.display_label), markup);
   g_free (markup);
       
   /* Change status bar state. */
   gamew.scontext = gtk_statusbar_get_context_id (GTK_STATUSBAR (gamew.statusbar),
                                                  "Statusbar");
   if (winner == 0)
   {
      load_image (sys_file = get_system_file("images/Tux7.png"));
      gtk_statusbar_push (GTK_STATUSBAR (gamew.statusbar), 
                          GPOINTER_TO_INT (data), _("End of game. Try again!"));
   }
   else if (winner == 1)
   {
      load_image (sys_file = get_system_file("images/Tux8.png"));
      gtk_statusbar_push (GTK_STATUSBAR (gamew.statusbar), 
                          GPOINTER_TO_INT (data), _("Congratulations!"));
   }
   else
   {
      load_image (sys_file = get_system_file("images/Tux7.png"));
      gtk_statusbar_push (GTK_STATUSBAR (gamew.statusbar), 
                          GPOINTER_TO_INT (data), _("Solution"));
   }

   /* Set title label */
   gtk_label_set_text (gamew.title_label, " "); 

   /* Desable the use of the keys. */
   keyboard_set_sensitive (KEYBOARD (gamew.keyboard), FALSE);
   gamew.first_game = 0;
}

/* Formats the text with a markup before displaying it. */ 
gchar *
format_text_with_markup (const gchar *text, int type)
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
         g_print ("Problem setting markups\n");
   }

   return markup;
}

/****************************
 * END: Auxiliary functions *
 * **************************/
