AC_INIT([ghangtux], [0.1], [patriciasc@openismus.com],
        [ghangtux], [http://openismus.com/])
AC_PREREQ([2.59])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([1.11 -Wall -Werror no-define])
AC_CONFIG_HEADERS([src/config.h])
AM_PROG_CC_C_O
PKG_CHECK_MODULES([DEP], [gtk+-2.0 >= 2.20 gio-2.0 >= 2.24 glib-2.0 >= 2.24])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
