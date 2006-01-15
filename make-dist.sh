#!/bin/sh

set -e

bzr diff

bzr export DIST_TREE

cd DIST_TREE

autoreconf --install --verbose

./configure

make dist

cp libgfshare*.tar.* ..

cd ..

rm -rf DIST_TREE
