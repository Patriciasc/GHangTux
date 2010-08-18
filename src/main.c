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

#include "ghangtux_ui.h"
#include "ghangtux_management.h"
#include "main.h"

int
main (int argc,
     char *argv[])
{
   Gamewidget gamew = gh_management_gamew_init();

   gtk_init (&argc, &argv);
  
   /* Internationalization i18n. */
   gh_management_import_languages ();
 
   /* Set up the builder. */
   gh_management_set_builder (&gamew);
   
   /* Set up application UI. */ 
   gh_ui_init (&gamew);
  
   /* Prepare first game with FILM theme */
   gh_management_game_init (&gamew);

   return 0;
} 
