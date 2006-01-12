# Copyright (c) Daniel Silverstone <dsilvers@digital-scurf.org> 2006

# ensure our m4 can be found.
ACLOCAL_AMFLAGS = -I m4

# Ensure the pc.in is included in the dist tar
EXTRA_DIST = libgfshare.pc.in


# Assemble the library
lib_LTLIBRARIES = libgfshare.la
libgfshare_la_SOURCES = include/libgfshare.h src/libgfshare.c
include_HEADERS = include/libgfshare.h

# And provide for the pkgconfigness
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libgfshare.pc

# Ensure the C files can find the headers
AM_CFLAGS = -I$(srcdir)/include

# Ensure our tests get run...
TESTS = test_gfshare

check_PROGRAMS = $(TESTS)

test_gfshare_SOURCES = tests/test_gfshare.c
test_gfshare_LDADD = libgfshare.la
test_gfshare_LDFLAGS = -static

# When cleaning up, ensure we remove any coverage results
clean-local: libgfshare-clean-local
libgfshare-clean-local:
	rm -f src/*.gcno src/*.gcda src/*.bb src/*.bbg src/*.da
	rm -f tests/*.gcno tests/*.gcda tests/*.bb tests/*.bbg tests/*.da

maintainer-clean-local: libgfshare-maintainer-clean-local
libgfshare-maintainer-clean-local:
	rm -f src/*.gcov tests/*.gcov
