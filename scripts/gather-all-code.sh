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

PROJECT_NAME=libGitWrap
BASE_GIT_URL=git@github.com:/macgitver
GIT_REPO_URL=$BASE_GIT_URL/${PROJECT_NAME}.git

BASE=${BASE:-`pwd`}
REF=${REF:-development}
TMP=$BASE/tmp
GIT_SRC=$BASE/git_src

echo "Gathering code for ${PROJECT_NAME}"
echo "Base      => $BASE"
echo "Ref       => $REF"
echo "Tmp       => $TMP"
echo "GIT_SRC   => $GIT_SRC"
echo ""
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

echo " * Wipe out temporary directory"
rm -rf $TMP
mkdir $TMP

# extract files from git repository
# $1 => path to main repository's work-tree
# $2 => relative path to submodule (. for main repository)
# $3 => prefix-directory in the destination archive
extract () {
    wd=$1
    repo=$2
    dest=$3/$repo
    cd $wd/$repo
    echo " * adding prefix $dest to archive"
    git archive --prefix $dest/ --format tar $REF | ( cd $TMP && tar xf - )
}

# reduce the files from libgit2 that we bundle to a bare minimum that is required
# legally and for the build.
cp $GIT_SRC/scripts/libgit2.export.attributes $GIT_SRC/.git/modules/libGitWrap/libgit2/info/attributes

# extract all the necessary source files to the temporary directory
extract $GIT_SRC . ${PROJECT_NAME}

for sm in $(git submodule status --recursive | awk '{ print $2 }') ; do
    extract $GIT_SRC $sm ${PROJECT_NAME}
done

echo "----------------------------------------------------------------"
echo "- DONE!"
echo "- You'll find the source for \"${PROJECT_NAME}\" here:"
echo "- ${TMP}"
echo "----------------------------------------------------------------"
