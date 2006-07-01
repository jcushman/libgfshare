#!/bin/sh

set -ex

TOPPWD=$(pwd)

TOPTEMPDIR=$(mktemp -p $TOPPWD)
rm $TOPTEMPDIR
mkdir $TOPTEMPDIR

TEMPDIR=$(mktemp -p $TOPTEMPDIR)
rm $TEMPDIR
mkdir $TEMPDIR

cleanup ()
{
  cd $TOPPWD
#  rm -rf $TOPTEMPDIR
}

bzr diff

trap cleanup 0

bzr export $TEMPDIR/DIST_TREE

cd $TEMPDIR/DIST_TREE

autoreconf --install --verbose

./configure

make distcheck

cp libgfshare*.tar.* ..

cd $TOPPWD

