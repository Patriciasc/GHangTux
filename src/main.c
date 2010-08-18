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
#include "ghangtux_keyboard.h"

static const gchar TUX_IMG_0[] = "images/Tux0.png";
static const gchar FILMS_FILE[] = "themes/films.txt";
static const gchar OBJECTS_FILE[] = "themes/objects.txt";
static const gchar PERSONS_FILE[] = "themes/persons.txt";
static const gchar LOGO[] = "icons/hicolor/200x200/apps/ghangtux.png";

static const gchar GUI_FILE[] = "ui/ghangtux.glade";
static const gchar UI_FILE[] = "ui/menu.ui";

static const gchar ACTION_GROUP[] = "MainActionGroup";
static const gchar SENTENCE_LABEL[] = "for_sentence_label";
static const gchar TITLE_LABEL[] = "for_title_label";
static const gchar WIN[] = "main_win";
static const gchar VBOX[] = "vbox";
static const gchar VBOX2[] = "vbox2";
static const gchar MENU[] = "/MainMenu";
static const gchar TOOLBAR[] = "/MainToolbar";
static const gchar IMAGE[] = "hangtux_area";
static const gchar STATUSBAR[] = "statusbar";
static const gchar EVENTBOX[] = "eventbox";

static const gint MIN_RANDOM = 1;
static const gint MAX_RANDOM = 41;
static const gint NUM_IMAGES = 7;

typedef struct _GameWidget Gamewidget;

/* Callbacks for signals. */
/* G_MODULE_EXPORT: For windows applications to put the 
 * callbakcs in the symbol table. */
G_MODULE_EXPORT void main_win_destroy            (GtkObject *window, gpointer data);
G_MODULE_EXPORT void format_sentence_with_letter (GHangtuxKeyboard *keyboard, const gchar key_name, gpointer data);

/* Callbacks for actions. */
static void new_action            (GtkAction *action, gpointer data);
static void solve_action          (GtkAction *action, gpointer data);
static void quit_action           (GtkAction *action, gpointer data);
static void activate_radio_action (GtkRadioAction *raction,
                                   GtkRadioAction *curr_raction, gpointer data);
static void about_action          (GtkAction *action, gpointer data);
static void extern_key_release    (GtkWidget *window, GdkEventKey *event, gpointer data);

/* Auxiliary functions. */
static void  load_image                (const char *file_image, Gamewidget *gamew);
static gchar *get_system_file          (const gchar *filename);
static void  set_end_game              (gpointer data, int winner);
static gchar *format_text_with_markup  (const gchar *text, int type);
static void  game_init                 (Gamewidget *gamew);
static void  set_logo                  (Gamewidget *gamew);
static void  get_sentence              (Gamewidget *gamew);
static void  set_builder               (Gamewidget *gamew);
static void  ui_init                   (Gamewidget *gamew);
static void  set_actions               (Gamewidget *gamew);
static void  keyboard_init             (Gamewidget *gamew);
Gamewidget   gamew_init                ();

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


/* Actual display sentence management. */
struct _GameWidget
{
   GtkBuilder *builder;       /* Apps builder */
   GtkUIManager *ui_manager;  /* UI Manager */
   GtkWidget *window;         /* Toplevel window */

   GtkWidget *vbox;           /* Vertical box */
   GtkWidget *vbox2;          /* Vertical box 2 */
   GtkWidget *menubar;        /* Menubar */

   GtkWidget *toolbar;        /* Toolbar */
   GtkWidget *eventbox;       /* Eventbox */
   gchar *sentence;           /* Games's sentence */

   gchar *display_sentence;   /* Displayed sentence */
   const gchar *valid_chars;  /* Player asserted characters */
   GtkLabel *display_label;   /* Label for display_sentence */

   GtkLabel *title_label;     /* Title label */
   GtkImage *image;           /* Displayed image */
   GtkActionGroup *def_group; /* Default action group */

   gint n_img;                /* Current image number */
   gboolean first_game;       /* Indicates if the current game is the first */
   GtkWidget *keyboard;       /* Keyboard */

   GtkWidget *statusbar;      /* Game status bar */
   gint scontext;             /* Game status bar's context */
   gint theme_id;             /* Current theme's identifier */
   GdkPixbuf *logo;           /* Application's logo */
};


int
main (int argc,
     char *argv[])
{
   Gamewidget gamew = gamew_init();

   gtk_init (&argc, &argv);
  
   /* Internationalization i18n. */
   bindtextdomain(GETTEXT_PACKAGE, PROGRAMNAME_LOCALEDIR);
   bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
   textdomain(GETTEXT_PACKAGE);
   
   /* Set up the builder. */
   set_builder (&gamew);
   
   /* Set up application UI. */ 
   ui_init (&gamew);
  
   /* Preparing game with FILM theme */
   game_init (&gamew);

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
      markup = format_text_with_markup (gamew->display_sentence, 0); 
      gtk_label_set_markup (GTK_LABEL (gamew->display_label), markup);
      
      /* Player wins. */
      if (g_strcmp0(gamew->sentence, gamew->display_sentence) == 0)
      {
         set_end_game(gamew, TRUE);
      }
   }
   else
   { 
      /* Loads a new image of the Hangtux. */ 
      if (gamew->n_img < NUM_IMAGES) 
      {
         image = g_strdup_printf("images/Tux%i.png",gamew->n_img);
         load_image (get_system_file(image), gamew);
         gamew->n_img++;
      }
      /* Player loses. */
      else
      {
         set_end_game(gamew,FALSE);
      }
   }
   
   g_free (image);
   g_free (markup);
}

static void
extern_key_release  (G_GNUC_UNUSED GtkWidget *window, 
                     GdkEventKey *event,
                     gpointer data)
{
   Gamewidget *gamew = (Gamewidget *)data;
   gchar *up_char = g_ascii_strup(event->string,-1);

   format_sentence_with_letter (GHANGTUX_KEYBOARD (gamew->keyboard), *up_char, gamew);
}

/******************************
 * END: Callbacks for signals *
 * ****************************/

/********************************
 * START: Callbacks for actions *
 * ******************************/

/* Loads a new game for the same current theme. */
static void 
new_action (G_GNUC_UNUSED GtkAction *action,
             gpointer data)
{
   Gamewidget *gamew = (Gamewidget *)data;
   get_sentence (gamew);
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
             gpointer data)
{
   Gamewidget *gamew = (Gamewidget *)data;
   main_win_destroy (GTK_OBJECT (gamew->window), NULL);
}

/* Activates a radio action from the menu. */
static void
activate_radio_action ( G_GNUC_UNUSED GtkRadioAction *raction,
                        GtkRadioAction *curr_raction,
                        gpointer data)
{
   Gamewidget *gamew = (Gamewidget *)data;
    
   /* Sets the current theme */
   gamew->theme_id = gtk_radio_action_get_current_value (curr_raction);
   get_sentence (gamew);
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

/* Loads an image for the GtkImage central area. */
static void
load_image (const char *file_image, Gamewidget *gamew)
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

/* Sets up the end of the game. */
static void
set_end_game (gpointer data, int winner)
{
   Gamewidget *gamew = (Gamewidget *)data;
   static gchar *sys_file = NULL;
   gchar *markup = NULL;
   
   /* Shows solution. */
   markup = format_text_with_markup (gamew->sentence, 0); 
   gtk_label_set_markup (GTK_LABEL (gamew->display_label), markup);
   g_free (markup);
       
   /* Change status bar state. */
   gamew->scontext = gtk_statusbar_get_context_id (GTK_STATUSBAR (gamew->statusbar),
                                                  "Statusbar");
   if (winner == 0)
   {
      load_image (sys_file = get_system_file("images/Tux7.png"), gamew);
      gtk_statusbar_push (GTK_STATUSBAR (gamew->statusbar), 
                          GPOINTER_TO_INT (gamew), _("End of game. Try again!"));
   }
   else if (winner == 1)
   {
      load_image (sys_file = get_system_file("images/Tux8.png"), gamew);
      gtk_statusbar_push (GTK_STATUSBAR (gamew->statusbar), 
                          GPOINTER_TO_INT (gamew), _("Congratulations!"));
   }
   else
   {
      load_image (sys_file = get_system_file("images/Tux7.png"), gamew);
      gtk_statusbar_push (GTK_STATUSBAR (gamew->statusbar), 
                          GPOINTER_TO_INT (gamew), _("Solution"));
   }

   /* Set title label */
   gtk_label_set_text (gamew->title_label, " "); 

   /* Desable the use of the keys. */
   ghangtux_keyboard_set_sensitive (GHANGTUX_KEYBOARD (gamew->keyboard), FALSE);
   gamew->first_game = FALSE;
}

/* Formats the text with a markup before displaying it. */ 
static gchar *
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
         g_critical ("Problem setting markups\n");
   }

   return markup;
}

/* Initializes game the first time. */
static void
game_init (Gamewidget *gamew)
{
   GtkAction *action = NULL;
   static gchar *init_image_path = NULL;

   action = gtk_action_group_get_action (gamew->def_group, "FilmsThemesMenuAction");

   if (!action)
   {
      g_critical ("Problem loading action for initializing game\n");
   }

   /* Activates "Film" radio action in the menu. */
   activate_radio_action (NULL, GTK_RADIO_ACTION (action), gamew);

   /* Load initial tux image. */
   init_image_path = get_system_file(TUX_IMG_0);
   load_image (init_image_path, gamew);
   gamew->n_img ++;

   /* Free memory and unref GObjects. */
   g_free (init_image_path);
   g_object_unref (gamew->ui_manager);

   gtk_main();
}

/* Load and set application logo. */
static void 
set_logo (Gamewidget *gamew)
{
   static gchar *logo_path = NULL;
   GError *error = NULL;

   /* XXX This is not the definitive image. */
   /* Looks for the logo in the installation path */
   logo_path = get_system_file (LOGO);
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

/* Selects a random sentence or word from the game files. */
static void
get_sentence (Gamewidget *gamew)
{
   GFile *file = NULL;
   GError *error = NULL;
   GFileInputStream *fstream = NULL; 
   GDataInputStream *stream = NULL;
   static gchar *sys_file = NULL;
   gchar *markup = NULL;
   gsize length = 0;
   gint random = 0;
   
   ghangtux_keyboard_set_sensitive (GHANGTUX_KEYBOARD (gamew->keyboard), TRUE);
   /* Not first game => Reset values and prepares new game. */
   if (!gamew->first_game)
   {
      gamew->valid_chars = NULL;
      load_image(sys_file = get_system_file (TUX_IMG_0), gamew);
      gamew->n_img = 1;
      g_free (sys_file);   
   }

   /* Status bar context. */
   gamew->scontext = gtk_statusbar_get_context_id (
                    GTK_STATUSBAR (gamew->statusbar),"Statusbar");

   /* Select the file. */
   switch (gamew->theme_id)
   {
      case 0:
         markup = format_text_with_markup (_("Guess the FILM by typing letters"), 1); 
         file = g_file_new_for_path (sys_file = get_system_file(FILMS_FILE));
         gtk_statusbar_push (GTK_STATUSBAR (gamew->statusbar), GPOINTER_TO_INT (gamew), _("Playing theme: Films"));
         break;

      case 1:
         markup = format_text_with_markup (_("Guess the OBJECT by typing letters"), 1); 
         file = g_file_new_for_path (sys_file = get_system_file (OBJECTS_FILE));
         gtk_statusbar_push (GTK_STATUSBAR (gamew->statusbar), GPOINTER_TO_INT (gamew), _("Playing theme: Objects"));
         break;

      case 2:
         markup = format_text_with_markup (_("Guess the PERSON by typing letters"), 1); 
         file = g_file_new_for_path (sys_file = get_system_file(PERSONS_FILE));
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
   markup = format_text_with_markup (gamew->display_sentence, 0); 
   gtk_label_set_markup (GTK_LABEL (gamew->display_label), markup);
   
   g_object_unref (file);
   g_object_unref (stream);
   g_object_unref (fstream);
   g_free (sys_file);
   g_free (markup);
}

/* Set up GtkBuilder. */
static void
set_builder (Gamewidget *gamew)
{
   static gchar *glade_file = NULL;
   GError *error = NULL;
   GdkColor image_bg = {0, 50535, 55535, 60535};

   /* Setting up the builder. */
   gamew->builder = gtk_builder_new();
  
   /* Load glade file. */
   glade_file = get_system_file (GUI_FILE);
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

/* Define action group and set up actions. */
static void
set_actions (Gamewidget *gamew)
{
   gamew->def_group = gtk_action_group_new (ACTION_GROUP);
   gtk_action_group_set_translation_domain (gamew->def_group, GETTEXT_PACKAGE);
   gtk_action_group_add_actions (gamew->def_group, actions, NUM_ACTIONS, gamew);
   gtk_action_group_add_radio_actions (gamew->def_group, radio_actions, NUM_RACTIONS,
                                       0, G_CALLBACK (activate_radio_action),  gamew);
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
   
   ui_path = get_system_file (UI_FILE);
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
   
   /*Load toolbar. */
   gamew->toolbar = gtk_ui_manager_get_widget (gamew->ui_manager, TOOLBAR);
   gtk_toolbar_set_style (GTK_TOOLBAR (gamew->toolbar), GTK_TOOLBAR_ICONS);
   gtk_box_pack_start (GTK_BOX (gamew->vbox), gamew->toolbar, FALSE, FALSE, 0);
   gtk_box_reorder_child (GTK_BOX (gamew->vbox), gamew->toolbar, 1);
}

static void
keyboard_init (Gamewidget *gamew)
{
   gamew->keyboard = ghangtux_keyboard_new();

   /* Using game's keyboard */
   g_signal_connect (gamew->keyboard, "key_clicked",
                     G_CALLBACK (format_sentence_with_letter), gamew);

   /* Using external keyboard */
   g_signal_connect (gamew->window, "key-release-event", 
                     G_CALLBACK (extern_key_release), gamew);

   gtk_box_pack_start (GTK_BOX (gamew->vbox2), gamew->keyboard, FALSE, FALSE, 0);
   gtk_box_reorder_child (GTK_BOX (gamew->vbox2), gamew->keyboard, 3);
}

/* Set up UI. */
static void 
ui_init (Gamewidget *gamew)
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

/* Initialize game's widgets. */
Gamewidget
gamew_init ()
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
/****************************
 * END: Auxiliary functions *
 * **************************/
