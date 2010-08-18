/*
 * ########################################################################
 * # File: GHangtux - ghangtux_keyboard.h                                 #
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

#ifndef GHANGTUX_KEYBOARD_H__
#define GHANGTUX_KEYBOARD_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GHANGTUX_KEYBOARD_TYPE            (ghangtux_keyboard_get_type ())
#define GHANGTUX_KEYBOARD(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GHANGTUX_KEYBOARD_TYPE, GHangtuxKeyboard))
#define GHANGTUX_KEYBOARD_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GHANGTUX_KEYBOARD_TYPE, GHangtuxKeyboardClass))
#define IS_GHANGTUX_KEYBOARD(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GHANGTUX_KEYBOARD_TYPE))
#define IS_GHANGTUX_KEYBOARD_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GHANGTUX_KEYBOARD_TYPE))
#define GHANGTUX_KEYBOARD_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GHANGTUX_KEYBOARD_TYPE, GHangtuxKeyboardClass))

typedef struct _GHangtuxKeyboard        GHangtuxKeyboard;
typedef struct _GHangtuxKeyboardClass   GHangtuxKeyboardClass;
typedef struct _GHangtuxKeyboardPrivate GHangtuxKeyboardPrivate;

struct _GHangtuxKeyboard
{
   GtkTable parent_instance;
   /* private */
   GHangtuxKeyboardPrivate *priv;
};

struct _GHangtuxKeyboardClass
{
   GtkTableClass parent_class;

   void (* key_clicked) (GHangtuxKeyboard *keyboard, const gchar key_name);
};

/* used by GHANGTUX_KEYBOARD_TYPE */
/*GType ghangtux_keyboard_get_type (void);*/

/* Method definitions for the user. */
GType          ghangtux_keyboard_get_type        (void);
GtkWidget*     ghangtux_keyboard_new             (void);
void           ghangtux_keyboard_set_sensitive   (GHangtuxKeyboard *keyboard, gboolean sensitive);

G_END_DECLS

#endif /* GHANGTUX_KEYBOARD_H__ */
