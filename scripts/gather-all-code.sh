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

SCRIPT_DIR=`dirname $0`

PROJECT_NAME=libGitWrap
BASE_GIT_URL=git@github.com:/macgitver
GIT_REPO_URL=$BASE_GIT_URL/$PROJECT_NAME.git

REF=${REF:-development}

BASE=${BASE:-`pwd`}
TMP=$BASE/tmp
GIT_SRC=$BASE/git_src

echo "Gathering code for $PROJECT_NAME"
echo "Base      => $BASE"
echo "Ref       => $REF"
echo "Tmp       => $TMP"
echo "GIT_SRC   => $GIT_SRC"
echo ""



# extract files from git repository
# $1 => path to main repository's work-tree
# $2 => relative path to submodule (. for main repository)
# $3 => prefix-directory in the destination folder
export-git-src () {
    wd=$1
    repo=$2
    dest=$3/$repo
    cd $wd/$repo
    echo " * adding prefix $dest to archive"
    git archive --prefix $dest/ --format tar $REF | ( cd $TMP && tar xf - )
}


# get the project sources
$SCRIPT_DIR/get-git-repo.sh $GIT_REPO_URL $GIT_SRC $REF

echo " * Wipe out temporary directory"
rm -rf $TMP
mkdir $TMP

# reduce the files exported from libgit2 to the minimum required to build the bundle
cp $SCIPT_DIR/libgit2.export.attributes $GIT_SRC/.git/modules/libGitWrap/libgit2/info/attributes

# extract all the necessary source files to the temporary directory
export-git-src $GIT_SRC . $PROJECT_NAME

cd $GIT_SRC
for sm in $(git submodule status --recursive | awk '{ print $2 }') ; do
    export-git-src $GIT_SRC $sm $PROJECT_NAME
done

echo "----------------------------------------------------------------"
echo "- DONE!"
echo "- You'll find the source for \"$PROJECT_NAME\" here:"
echo "- $TMP"
echo "----------------------------------------------------------------"
