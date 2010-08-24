/*
 * ########################################################################
 * # File: GHangtux - ghangtux_utils.h                                    #
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

#ifndef GHANGTUX_UTILS_H__
#define GHANGTUX_UTILS_H__

static const gchar FILMS_FILE[]   = "themes/films.txt";
static const gchar OBJECTS_FILE[] = "themes/objects.txt";
static const gchar PERSONS_FILE[] = "themes/persons.txt";
static const gint  MIN_RANDOM     = 1;
static const gint  MAX_RANDOM     = 41;

gchar *gh_utils_get_system_file          (const gchar *filename);
gchar *gh_utils_format_text_with_markup  (const gchar *text, int type);
void   gh_utils_load_image               (const char *file_image, Gamewidget *gamew);
void   gh_utils_import_languages         ();

#endif /* GHANGTUX_UTILS_H__ */
