#! /bin/bash

# Clones or fetches a git repository into a destination directory
# and updates the submodules on the given reference
#
# Attention:
# This script is meant to be called by an automated build. Local changes will be overwritten!

GIT_REPO_URL=$1
GIT_SRC=$2
REF=$3

echo " * getting sources from ${GIT_REPO_URL}"
if ! [ -d $GIT_SRC ] ; then
    mkdir -p $GIT_SRC && cd $GIT_SRC
    git clone ${GIT_REPO_URL} .
    git checkout $REF
else
    cd $GIT_SRC
    git fetch origin
    git update-ref refs/heads/$REF origin/$REF
    git checkout -f $REF
fi

echo " * Init and update submodules"
cd $GIT_SRC
git submodule update --init --recursive
git submodule foreach --recursive 'git reset --hard && git clean -dfx'
