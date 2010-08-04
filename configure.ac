#############################################################################
# File: GHangtux - configure.ac                                             #
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
#############################################################################

AC_INIT([ghangtux], [1.1], [patriciasc@openismus.com],
        [ghangtux], [http://openismus.com/])
AC_PREREQ([2.59])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([1.11 -Wall -Werror no-define])
AC_CONFIG_HEADERS([src/config.h])

# Start Intltools
IT_PROG_INTLTOOL([0.41])

AC_SUBST([GETTEXT_PACKAGE], [$PACKAGE_NAME])
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE], ["$GETTEXT_PACKAGE"],
                   [The domain to use with gettext])
AM_GLIB_GNU_GETTEXT

AC_SUBST([PROGRAMNAME_LOCALEDIR], [${datadir}/locale])
#End Intltools

AM_PROG_CC_C_O
PKG_CHECK_MODULES([DEP], [gtk+-2.0 >= 2.20 gio-2.0 >= 2.24 glib-2.0 >= 2.24 gmodule-export-2.0])

DK_ARG_ENABLE_WARNINGS([GHANGTUX_WARNING_FLAGS],
                       [-Wall -w1],
                       [-pedantic -Wall -Wextra -w1],
                       [GTK GDK GDK_PIXBUF G])

AC_CONFIG_FILES([Makefile po/Makefile.in])
AC_OUTPUT
