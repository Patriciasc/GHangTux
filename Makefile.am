AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

AM_CPPFLAGS = $(DEP_CFLAGS)

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
dist_icon48x48_DATA = data/icons/ghangtux.png
icon200x200dir = $(pkgdatadir)/icons
dist_icon200x200_DATA = data/icons/icon200x200.png

themesdir = $(pkgdatadir)/themes
dist_themes_DATA = data/themes/persons.txt \
                   data/themes/objects.txt \
                   data/themes/films.txt 

ghangtux_LDADD = $(DEP_LIBS)

#desktop = $(datadir)/applications
#desktop_in_files = ghangtux.desktop.in
#desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)


dist_noinst_SCRIPTS = autogen.sh
