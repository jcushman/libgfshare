# Copyright (c) Daniel Silverstone <dsilvers@digital-scurf.org> 2006

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = libgfshare.pc.in

lib_LTLIBRARIES = libgfshare.la

libgfshare_la_SOURCES = include/libgfshare.h src/libgfshare.c

pkgconfigdir = $(libdir)/pkgconfig

pkgconfig_DATA = libgfshare.pc


clean-local: libgfshare-clean-local

libgfshare-clean-local:
	rm -f src/*.gcno src/*.gcda src/*.bb src/*.bbg src/*.da

maintainer-clean-local: libgfshare-maintainer-clean-local

libgfshare-maintainer-clean-local:
	rm -f src/*.gcov
