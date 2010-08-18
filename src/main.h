/*
 * ########################################################################
 * # File: GHangtux - main.h                                              #
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

#ifndef MAIN_H__
#define MAIN_H__

#include <gtk/gtk.h>

typedef struct _GameWidget Gamewidget;

/* Game widgets. */
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

#endif /* MAIN_H__ */
