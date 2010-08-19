/*
 * ########################################################################
 * # File: GHangtux - ghangtux_ui.h                                       #
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
#ifndef GHANGTUX_UI_H__
#define GHANGTUX_UI_H__

#include "ghangtux.h"

static const gchar GUI_FILE[]     = "ui/ghangtux.glade";
static const gchar UI_FILE[]      = "ui/menu.ui";
static const gchar MENU[]         = "/MainMenu";
static const gchar TOOLBAR[]      = "/MainToolbar";
static const gchar LOGO[]         = "icons/hicolor/200x200/apps/ghangtux.png";
static const gchar ACTION_GROUP[] = "MainActionGroup";

/* G_GNUC_UNUSED: for desabling warnings when 
 * a parameter in a function is not used */

void gh_ui_init                  (Gamewidget *gamew);
void gh_ui_activate_radio_action (G_GNUC_UNUSED GtkRadioAction *raction,
                                  GtkRadioAction *curr_raction, gpointer data);

#endif /* GHANGTUX_UI_H__ */
