/*
 * ########################################################################
 * # File: GHangtux - ghangtux_keyboard.c                                 #
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
#include "ghangtux_keyboard.h"

#define N_ROWS 2
#define N_COLS 13
#define GHANGTUX_KEYBOARD_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GHANGTUX_KEYBOARD_TYPE, GHangtuxKeyboardPrivate))

G_DEFINE_TYPE (GHangtuxKeyboard,ghangtux_keyboard, GTK_TYPE_TABLE)

struct _GHangtuxKeyboardPrivate
{
   GtkWidget *button[N_ROWS][N_COLS];
};

static void ghangtux_keyboard_key_set_insensitive (GtkWidget *key, gpointer keyboard);

/* Signals. */
enum {
  KEY_CLICKED,
  LAST
};

static guint keyboard_signals[LAST] = { 0 };
/* Initializes the fields of the widget's class structure, 
 * and sets up any signals for the class. */
static void
ghangtux_keyboard_class_init (GHangtuxKeyboardClass *klass)
{
   keyboard_signals[KEY_CLICKED] = g_signal_new (
                                       "key_clicked",
                                       G_TYPE_FROM_CLASS (G_OBJECT_CLASS (klass)),
                                       G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
                                       G_STRUCT_OFFSET (GHangtuxKeyboardClass,
                                                        key_clicked),
                                       NULL, 
                                       NULL,                
                                       g_cclosure_marshal_VOID__CHAR,
                                       G_TYPE_NONE, 
                                       1, 
                                       G_TYPE_CHAR);
   g_type_class_add_private(klass, sizeof(GHangtuxKeyboardPrivate));
}

/* Initializes the object structure. */
static void
ghangtux_keyboard_init (GHangtuxKeyboard *keyboard)
{
   gint i;
   gint j;
   gchar ascii_char = 65;   /* ascci A = 65 */
   gfloat xyalign = 0.50;
   keyboard->priv = GHANGTUX_KEYBOARD_GET_PRIVATE(keyboard);

  
   gtk_table_resize (GTK_TABLE (keyboard), N_ROWS, N_COLS);
   gtk_table_set_homogeneous (GTK_TABLE (keyboard), TRUE);

   for (i=0; i<N_ROWS; i++)
   {
      for (j=0; j<N_COLS; j++)      
      {
         keyboard->priv->button[i][j] = gtk_button_new_with_label (&ascii_char);

         gtk_button_set_use_underline (GTK_BUTTON (keyboard->priv->button[i][j]), TRUE);
         gtk_button_set_focus_on_click (GTK_BUTTON (keyboard->priv->button[i][j]), TRUE);
         gtk_button_set_alignment ( GTK_BUTTON (keyboard->priv->button[i][j]), xyalign, xyalign); 

         gtk_table_attach_defaults (GTK_TABLE (keyboard), keyboard->priv->button[i][j], 
                                    j, j+1, i, i+1);
         g_signal_connect (G_OBJECT (keyboard->priv->button[i][j]), "clicked",
                           G_CALLBACK (ghangtux_keyboard_key_set_insensitive),
                           (gpointer) keyboard);

         gtk_widget_show (keyboard->priv->button[i][j]);
         ascii_char++;
      }
   }
}

/* Looks for a letter in the displayed sentence. */
static void
ghangtux_keyboard_key_set_insensitive (GtkWidget *key, gpointer keyboard)
{
   const gchar *key_name = NULL;

   gtk_widget_set_sensitive (key, FALSE);

   /* Forwards clicked signal to keyboard widgets signal. */
   key_name = gtk_button_get_label(GTK_BUTTON (key));
   g_signal_emit (keyboard, 
                  keyboard_signals[KEY_CLICKED],
                  0,
                  key_name[0]);
}

/****************************
 * START: User's functions. *
 * **************************/

/* Function that the user calls to create the object. */
GtkWidget*
ghangtux_keyboard_new ()
{
  return GTK_WIDGET (g_object_new (GHANGTUX_KEYBOARD_TYPE, NULL));
}

/* Sets the keyboard to sensitive or not sensitive. */
void
ghangtux_keyboard_set_sensitive (GHangtuxKeyboard *keyboard,
                             gboolean sensitive)
{
   gint i;
   gint j;
   keyboard->priv = GHANGTUX_KEYBOARD_GET_PRIVATE(keyboard);

   for (i=0; i<N_ROWS; i++)
   {
      for (j=0; j<N_COLS; j++)      
      {
         gtk_widget_set_sensitive (keyboard->priv->button[i][j], sensitive);
      }
   }
}

/**************************
 * END: User's functions. *
 * ************************/
