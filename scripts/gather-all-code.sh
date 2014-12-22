#!/bin/bash

# This script is intended to be run by a Jenkins job.

# if REF is set, this ref will be used on macgitver.git and mgv-full.git
# if not, 'development' will be used. Does ONLY work with branches in mgv-full,

# expected directory structure:

# base_directory     -> `pwd`
#  !
#  +-- gw            -> clone of libGitWrap.git
#  !                    (will be updated and submodules will be init-updated)
#  !
#  +-- tmp           -> work-space. Will be re-created every time the script is run!

SCRIPT_DIR=$(dirname `readlink -f -- $0`)

source $SCRIPT_DIR/includes/git-funcs.sh
source $SCRIPT_DIR/includes/rad-tools.sh

PROJECT_NAME=libGitWrap
BASE_GIT_URL=git@github.com:/macgitver
GIT_REPO_URL=$BASE_GIT_URL/$PROJECT_NAME.git

REF=${REF:-development}

BASE=${BASE:-`pwd`}
TMP=$BASE/tmp
GIT_SRC=$BASE/git_src

echo ""
echo "BASE      => $BASE"
echo "REF       => $REF"
echo "TMP       => $TMP"
echo "GIT_SRC   => $GIT_SRC"
echo ""
echo "Gathering code for $PROJECT_NAME ..."
echo ""

# first clean $TMP dir
echo " * Wipe out temporary directory"
rm -rf $TMP
mkdir $TMP

# install RAD-Tools to $TMP dir
RadTools_Install $BASE/git_rad_tools $TMP &&

# get project sources
Git_Get $GIT_SRC $GIT_REPO_URL $REF &&
Git_SparseCheckout $GIT_SRC .git $REF &&
Git_ForcedModuleUpdate $GIT_SRC &&

# reduce the files exported from libgit2 to the minimum required to build the bundle
cp $SCRIPT_DIR/libgit2.export.attributes $GIT_SRC/.git/modules/libGitWrap/libgit2/info/attributes &&

# extract all the necessary source files to the temporary directory
Git_ExportRepo $GIT_SRC $TMP $PROJECT_NAME &&

echo "----------------------------------------------------------------"
echo "- DONE!"
echo "- You'll find the source for \"$PROJECT_NAME\" here:"
echo "- $TMP"
echo "----------------------------------------------------------------"
