AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS} 

AM_CPPFLAGS = $(DEP_CFLAGS) -DPROGRAMNAME_LOCALEDIR=\"${PROGRAMNAME_LOCALEDIR}\"

bin_PROGRAMS = ghangtux
ghangtux_SOURCES = src/main.c \
                   src/keyboard.h \
                   src/keyboard.c 

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

icon48x48dir = $(pkgdatadir)/icons
dist_icon48x48_DATA = data/icons/icon48x48.png
icon200x200dir = $(pkgdatadir)/icons
dist_icon200x200_DATA = data/icons/icon200x200.png

themesdir = $(pkgdatadir)/themes
dist_themes_DATA = data/themes/persons.txt \
                   data/themes/objects.txt \
                   data/themes/films.txt 

ghangtux_LDADD = $(DEP_LIBS)

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

SUBDIRS = po

dist_noinst_DATA = $(desktop_in_files)
dist_noinst_SCRIPTS = autogen.sh \
							 $(INTLTOOL_FILES)
