#############################################################################
# File: GHangtux - Makefile.am                                              #
#                                                                           #
# Author: Patricia Santana Cruz  <patriciasc@openismus.com>                 #
#                                                                           #  
# Copyright (C) 2010 Openismus GmbH                                         #
# 																									 #
# Version 1.1                                                               #
# 																									 #
# GHangtux is free software; you can redistribute it and/or modify          #
# it under the terms of the GNU General Public License as published         #
# by the Free Software Foundation; either version 3 of the License,         #
# or (at your option) any later version.                                    #
# 																									 #
# GHangtux is distributed in the hope that it will be useful,               #
# but WITHOUT ANY WARRANTY; without even the implied warranty of            #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             #
# GNU General Public License for more details.                              #
# 																									 #
# You should have received a copy of the GNU General Public License         #
# along with GHangtux. If not, see <http://www.gnu.org/licenses/>. 			 #
#  																							    #
# This project does not use recursive make, but a single toplevel Makefile  #
# to build the entire tree (excluding the po subdirectory as gettext comes  #
# with its own build system).  Read Peter Miller's excellent paper to learn #
# why recursive make invocations are both slow and error-prone: 				 #
# http://www.cse.iitb.ac.in/~soumen/teach/cs699a1999/make.html 				 #
#############################################################################

AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS} 
DISTCHECK_CONFIGURE_FLAGS = --enable-warnings=fatal

AM_CPPFLAGS = $(DEP_CFLAGS) -DPROGRAMNAME_LOCALEDIR=\"${PROGRAMNAME_LOCALEDIR}\" $(GHANGTUX_WARNING_FLAGS)
ghangtux_LDADD = $(DEP_LIBS)

SUBDIRS = po

bin_PROGRAMS = ghangtux
ghangtux_SOURCES = src/ghangtux.h \
						 src/ghangtux.c \
                   src/ghangtux_keyboard.h \
                   src/ghangtux_keyboard.c \
                   src/ghangtux_management.h \
                   src/ghangtux_management.c \
                   src/ghangtux_utils.h \
                   src/ghangtux_utils.c \
                   src/ghangtux_ui.h \
                   src/ghangtux_ui.c
                        
                    

imagesdir = $(pkgdatadir)/images
dist_images_DATA = data/images/Tux0.png \
                   data/images/Tux1.png \
						 data/images/Tux2.png \
                   data/images/Tux3.png \
                   data/images/Tux4.png \
                   data/images/Tux5.png \
                   data/images/Tux6.png \
                   data/images/Tux7.png \
                   data/images/Tux8.png

uidir = $(pkgdatadir)/ui
dist_ui_DATA = data/ui/ghangtux.glade \
               data/ui/menu.ui

iconsdir = $(pkgdatadir)/icons/hicolor
appicon48dir = $(iconsdir)/48x48/apps
dist_appicon48_DATA = data/icons/48x48/ghangtux.png
appicon200dir = $(iconsdir)/200x200/apps
dist_appicon200_DATA = data/icons/200x200/ghangtux.png

themesdir = $(pkgdatadir)/themes
dist_themes_DATA = data/themes/persons.txt \
                   data/themes/objects.txt \
                   data/themes/films.txt 

desktopdir = $(datadir)/applications
desktop_in_files = ghangtux.desktop.in
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)
@INTLTOOL_DESKTOP_RULE@

INTLTOOL_FILES = intltool-extract.in \
                 intltool-merge.in \
                 intltool-update.in

DISTCLEANFILES = intltool-extract \
                 intltool-merge \
                 intltool-update\
                 po/.intltool-merge-cache \
					  $(desktop_DATA)

dist_noinst_DATA = $(desktop_in_files)
dist_noinst_SCRIPTS = autogen.sh \
							 $(INTLTOOL_FILES)
