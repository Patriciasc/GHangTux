AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

AM_CPPFLAGS = $(DEP_CFLAGS)
bin_PROGRAMS = ghangtux
ghangtux_SOURCES = src/main.c 

imagesdir = $(pkgdatadir)/images
dist_images_DATA = data/images/Tux0.png \
                   data/images/Tux1.png \
						 data/images/Tux2.png \
                   data/images/Tux3.png \
                   data/images/Tux4.png \
                   data/images/Tux5.png \
                   data/images/Tux6.png \
                   data/images/Tux7.png 

uidir = $(pkgdatadir)/ui
dist_ui_DATA = data/ui/ghangtux.glade \
               data/ui/menu.ui

# iconsdir -- coming soon --
# iconsdir = $(pkgdatadir)/icons
# dist_icon16_DATA = data/icons/?
# dist_icon22_DATA = data/icons/?

themesdir = $(pkgdatadir)/themes
dist_themes_DATA = data/themes/persons.txt \
                   data/themes/objects.txt \
                   data/themes/films.txt 

ghangtux_LDADD = $(DEP_LIBS)

dist_noinst_SCRIPTS = autogen.sh
