/*
 * ########################################################################
 * # File: GHangtux - ghangtux_management.h                               #
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

#ifndef GHANGTUX_MANAGEMENT_H__
#define GHANGTUX_MANAGEMENT_H__

#include "ghangtux_ui.h"

static const gchar TUX_IMG_0[]      = "images/Tux0.png";
static const gchar SENTENCE_LABEL[] = "for_sentence_label";
static const gchar TITLE_LABEL[]    = "for_title_label";
static const gchar WIN[]            = "main_win";
static const gchar VBOX[]           = "vbox";
static const gchar VBOX2[]          = "vbox2";
static const gchar IMAGE[]          = "hangtux_area";
static const gchar STATUSBAR[]      = "statusbar";
static const gchar EVENTBOX[]       = "eventbox";
static const gint NUM_IMAGES        = 7;

void        gh_management_game_init       (Gamewidget *gamew);
Gamewidget  gh_management_gamew_init      ();
void        gh_management_set_end_game    (gpointer data, GameEndCondition winner);
void        gh_management_set_builder     (Gamewidget *gamew);
void        gh_management_new_game_start  (Gamewidget *gamew);

#endif /* GHANGTUX_MANAGEMENT_H__ */
