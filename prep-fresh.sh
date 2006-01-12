#!/bin/sh

set -x
set -e

autoreconf --install --verbose

test -d BUILD || mkdir BUILD

cd BUILD && ../configure --enable-maintainer-mode \
    --enable-compiler-warnings \
    --cache-file=../configure.cache \
    --enable-dependency-tracking \
    --disable-fast-install \
    --enable-debugging-mode \
    --enable-compiler-coverage \
    --enable-linker-optimisations
