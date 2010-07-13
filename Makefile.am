AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

AM_CPPFLAGS = $(DEP_CFLAGS)
bin_PROGRAMS = ghangtux
ghangtux_SOURCES = src/main.c 

ghangtux_LDADD = $(DEP_LIBS)

dist_noinst_SCRIPTS = autogen.sh
