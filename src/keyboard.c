#include "gtk/gtk.h"
#include "keyboard.h"

/* Start: Signals. */
enum {
  KEY_CLICKED,
  LAST
};

static guint keyboard_signals[LAST] = { 0 };
/* End: Signals. */

static void keyboard_class_init          (KeyboardClass *klass);
static void keyboard_init                (Keyboard      *keyboard);
static void keyboard_key_set_insensitive (GtkWidget *key, gpointer keyboard);

/* Initializes the fields of the widget's class structure, 
 * and sets up any signals for the class. */
static void
keyboard_class_init (KeyboardClass *klass)
{
   keyboard_signals[KEY_CLICKED] = g_signal_new (
                                       "key_clicked",
                                       G_TYPE_FROM_CLASS (G_OBJECT_CLASS (klass)),
                                       G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
                                       G_STRUCT_OFFSET (KeyboardClass,
                                                        key_clicked),
                                       NULL, 
                                       NULL,                
                                       g_cclosure_marshal_VOID__CHAR,
                                       G_TYPE_NONE, 
                                       1, 
                                       G_TYPE_CHAR);
}

/* Initializes the object structure. */
static void
keyboard_init (Keyboard *keyboard)
{
   gint i;
   gint j;
   gchar ascci_char = 65;   /* ascci A = 65 */
   gfloat xyaling = 0.50;
   const gchar *label;

  
   gtk_table_resize (GTK_TABLE (keyboard), N_ROWS, N_COLS);
   gtk_table_set_homogeneous (GTK_TABLE (keyboard), TRUE);

   for (i=0; i<N_ROWS; i++)
   {
      for (j=0; j<N_COLS; j++)      
      {
         label = g_strdup_printf("%c",ascci_char);
         keyboard->button[i][j] = gtk_button_new_with_mnemonic (label);

         gtk_button_set_use_underline (GTK_BUTTON (keyboard->button[i][j]), TRUE);
         gtk_button_set_focus_on_click (GTK_BUTTON (keyboard->button[i][j]), TRUE);
         gtk_button_set_alignment ( GTK_BUTTON (keyboard->button[i][j]), xyaling, xyaling); 

         gtk_table_attach_defaults (GTK_TABLE (keyboard), keyboard->button[i][j], 
                                    j, j+1, i, i+1);
         g_signal_connect (G_OBJECT (keyboard->button[i][j]), "clicked",
                           G_CALLBACK (keyboard_key_set_insensitive),
                           (gpointer) keyboard);

         gtk_widget_show (keyboard->button[i][j]);
         ascci_char++;
      }
   }
}

/* Looks for a letter in the displayed sentence. */
static void
keyboard_key_set_insensitive (GtkWidget *key, gpointer keyboard)
{
   const gchar *key_name = NULL;

   gtk_widget_set_sensitive (key, FALSE);

   /* Forwards clicked signal to keyboard widgets signal. */
   key_name = gtk_button_get_label(GTK_BUTTON (key));
   g_signal_emit (keyboard, 
                  keyboard_signals[KEY_CLICKED],
                  0,
                  key);
}

/****************************
 * START: User's functions. *
 * **************************/

/* When first called, tells Glib about the new class, and gets an ID that
 * uniquely identifies the class. Upon subsequent calls, it just returns the ID. */
GType
keyboard_get_type (void)
{
   static GType keyboard_type = 0;

   if (!keyboard_type)
   {
      static const GTypeInfo keyboard_info =
      {
         sizeof (KeyboardClass),                /* class_size */
         NULL,                                  /* base_init */
         NULL,                                  /* base_finalize */
         (GClassInitFunc) keyboard_class_init,  /* class_init */
         NULL,                                  /* class_finalize */
         NULL,                                  /* class_data */
         sizeof (Keyboard),                     /* instance_size */
         0,                                     /* n_preallocs */
         (GInstanceInitFunc) keyboard_init,     /* instace_init */
         NULL                                   /* value_table */
      };

      keyboard_type = g_type_register_static (GTK_TYPE_TABLE, "Keyboard", &keyboard_info, 0);
   }

   return keyboard_type;
}

/* Function that the user calls to create the object. */
GtkWidget*
keyboard_new ()
{
  return GTK_WIDGET (g_object_new (keyboard_get_type (), NULL));
}

/* Sets the keyboard to sensitive or not sensitive. */
void
keyboard_set_sensitive (Keyboard *keyboard,
                             gboolean sensitive)
{
   gint i;
   gint j;

   for (i=0; i<N_ROWS; i++)
   {
      for (j=0; j<N_COLS; j++)      
      {
         gtk_widget_set_sensitive (keyboard->button[i][j], sensitive);
      }
   }
}

/**************************
 * END: User's functions. *
 * ************************/
