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
AC_CONFIG_FILES([Makefile po/Makefile.in])
AC_OUTPUT
