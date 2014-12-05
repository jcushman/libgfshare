#!/bin/sh

set -ex

TOPPWD=$(pwd)

TEMPDIR=$(mktemp -t tmp.XXXXXXXXXX)
rm $TEMPDIR
mkdir $TEMPDIR

cleanup ()
{
  cd "$TOPPWD"
  rm -rf $TEMPDIR
}

trap cleanup 0

cp -R * $TEMPDIR
cd $TEMPDIR
rm -rf .git

autoreconf --install --verbose

./configure

make distcheck

cp libgfshare*.tar.* "$TOPPWD"

cd "$TOPPWD"

