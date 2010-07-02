// ############################################################################
// Author: Patricia Santana Cruz                                              #
// Version: 0.1                                                               #  
// Description: Game variation of Hangman                                     #
// Compile me with:                                                           #
// gcc -g main.c -o hangtux `pkg-config --cflags --libs gtk+-2.0 gmodule-2.0` #
// ############################################################################

#include <gtk/gtk.h>
#include <glib.h>
#include <gio/gio.h>

#define GUI_FILE "ghangtux.glade"
#define UI_FILE "menu.ui"
#define ACTION_GROUP "MainActionGroup"
#define WIN "main_win"
#define VBOX "vbox"
#define MENU "/MainMenu"
#define FILMS_FILE "../films.txt"
#define OBJECTS_FILE "../objects.txt"
#define PERSONS_FILE "../persons.txt"
#define MIN_RANDOM 1
#define MAX_RANDOM 40

void main_win_destroy (GtkObject *window, gpointer data);
static void get_sentence_action (GtkRadioAction *raction,
                                 GtkRadioAction *curr_raction, gpointer data);
static void quit_action (GtkAction *action, gpointer data);
static void about_action (GtkAction *action, gpointer data);
static void format_sentence (gchar *sentence, gchar *letter);

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

int
main(int argc,
     char **argv)
{
   GtkBuilder *builder = NULL;
   GtkWidget *window = NULL;
   GtkWidget *vbox = NULL;
   GtkWidget *menubar = NULL;
   GtkActionGroup *def_group = NULL;
   GtkUIManager *uimanager = NULL;
   GError *error = NULL;

   gtk_init (&argc, &argv);

   builder = gtk_builder_new();

   if (!gtk_builder_add_from_file (builder, GUI_FILE, &error))
   {
     g_message ("%s", error->message);
     g_error_free (error);
     return 1;
   }

   window = GTK_WIDGET (gtk_builder_get_object (builder,WIN));
   vbox = GTK_WIDGET (gtk_builder_get_object (builder, VBOX));

   gtk_builder_connect_signals (builder,NULL);
   g_object_unref (G_OBJECT(builder));

   /* Setting up UI manager. */ 
   def_group = gtk_action_group_new (ACTION_GROUP);
   gtk_action_group_add_actions (def_group, actions, NUM_ACTIONS, NULL);
   gtk_action_group_add_radio_actions (def_group, radio_actions, NUM_RACTIONS,
                                       -1, G_CALLBACK (get_sentence_action),  NULL);
 
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

//XXX REMEBER TO FREE MEMORY or CLOSE FILES, ETC
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
   gchar *sentence = NULL;
   gsize length = 0;
   gint random = 0;

   switch (gtk_radio_action_get_current_value (curr_raction))
   {
      case 0:
         file = g_file_new_for_path (FILMS_FILE);
         break;

      case 1:
         file = g_file_new_for_path (OBJECTS_FILE);
         break;

      case 2:
         file = g_file_new_for_path (PERSONS_FILE);
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

   /* Select a random film from the file. */
   random = g_random_int_range (MIN_RANDOM, MAX_RANDOM);
   g_print ("random value = %i\n", random);

   while (((sentence = g_data_input_stream_read_line (stream, &length, NULL, 
            &error)) != NULL) && (random != 0))
   {
      random--;
   }
   
   format_sentence (sentence, " ");
}

/* Formats the sentence or word for display. */
static void
format_sentence (gchar *sentence, gchar *letter)
{
   g_print ("%s\n", sentence);   
}

/* Quits the application from the menu. */
static void
quit_action (GtkAction *action,
             gpointer data)
{
   g_print ("\n--------------------------------------------------------------------\n\n");
   g_print ("\n- Leaving the application. Thank you for playing with GHangTux 0.1 -\n\n");
   g_print ("\n--------------------------------------------------------------------\n\n");
   main_win_destroy (NULL,NULL);
}

/* Shows an about dialog from the application. */
static void
about_action (GtkAction *action,
              gpointer data){
   g_print ("about\n");
}
