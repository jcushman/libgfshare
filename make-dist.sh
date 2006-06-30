#!/bin/sh

set -ex

TOPWD=$(pwd)

cleanup ()
{
  cd $TOPPWD
  rm -rf DIST_TREE
}

bzr diff

trap cleanup 0

bzr export DIST_TREE

cd DIST_TREE

autoreconf --install --verbose

./configure

make dist

cp libgfshare*.tar.* ..

cd $TOPPWD

