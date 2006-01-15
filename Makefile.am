# Copyright (c) Daniel Silverstone <dsilvers@digital-scurf.org> 2006

# ensure our m4 can be found.
ACLOCAL_AMFLAGS = -I m4

# Ensure the pc.in is included in the dist tar
EXTRA_DIST = libgfshare.pc.in

# Useful maketable binary
noinst_PROGRAMS = gfshare_maketable
gfshare_maketable_SOURCES = src/gfshare_maketable.c

# Assemble the library
lib_LTLIBRARIES = libgfshare.la
libgfshare_la_SOURCES = include/libgfshare.h src/libgfshare.c \
                        libgfshare_tables.h
libgfshare_la_LDFLAGS = -version-info @LTLIBVER@
include_HEADERS = include/libgfshare.h

$(top_srcdir)/src/libgfshare.c: libgfshare_tables.h
libgfshare_tables.h: gfshare_maketable
	./gfshare_maketable > libgfshare_tables.h

# And provide for the pkgconfigness
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libgfshare.pc

# Ensure the C files can find the headers
AM_CFLAGS = -I$(srcdir)/include

# Our programs come next...

bin_PROGRAMS = gfsplit gfcombine

gfsplit_SOURCES = tools/gfsplit.c
gfsplit_LDADD = libgfshare.la

gfcombine_SOURCES = tools/gfcombine.c
gfcombine_LDADD = libgfshare.la

# Ensure our tests get run...
TESTS = test_gfshare_blockwise_simple

check_PROGRAMS = $(TESTS)

test_gfshare_blockwise_simple_SOURCES = tests/test_gfshare_blockwise_simple.c
test_gfshare_blockwise_simple_LDADD = libgfshare.la
test_gfshare_blockwise_simple_LDFLAGS = -static

# When cleaning up, ensure we remove any coverage results and the tables
clean-local: libgfshare-clean-local libgfshare-clean-local-coverage
libgfshare-clean-local:
	rm -f libgfshare_tables.h

libgfshare-clean-local-coverage:
	rm -f src/*.gcno src/*.gcda src/*.bb src/*.bbg src/*.da
	rm -f tests/*.gcno tests/*.gcda tests/*.bb tests/*.bbg tests/*.da

maintainer-clean-local: libgfshare-maintainer-clean-local
libgfshare-maintainer-clean-local:
	rm -f src/*.gcov tests/*.gcov
