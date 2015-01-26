# GitWrap - Git API for Qt
This library provides an easy to use Qt'ish API, based on the libgit2 library.

If you want to write a Qt application with access to Git repositories, GitWrap is probably what you are looking for.

**Important Note:**

Development and API-Design of this library is mainly driven by the [MacGitver-Project](http://www.macgitver.org). We provide a deprecation mechanism, that avoids unnecessary API-breakage. Sometimes this is not possible though and you have to make adjustments by your own, when updating to a newer development version.

## License
GitWrap is released under the terms of GPL version 2. 

# How to use GitWrap
Method names in GitWrap follow the libgit2 naming conventions in general. There are some methods, that are not available in libgit2. A good example is are GitWrap operations (i.e. `Git::CloneOperation`), which provides extended functionality.

You can actually lookup most keywords in the libgit2 API reference, when they are not documented yet in GitWrap.

Here's an example for the general convention for method names.

```c++
// libgit2 function
git_reference_name()

// GitWrap method
Git::Reference::name()
```

## Handling Errors - Life without Exceptions!
GitWrap doesn't throw a single exception! Instead we provide reliable error handling, that binds closely to libgit2's error system. Sounds complicated? It's pretty easy and you'll probably love it!

Here's how to use it:

```c++
// The general calling convention is:
Result r;

Git::some-function( r, /* other parameters */ );

if ( !r )
{
    // React to the failed result.
    // For example, you can display the descriptive error text.
    showError( r.errorText() );

    // Usually you want to return here.
    return;

    // If you need to continue for whatever reason,
    // the result must be cleared:
    r.clear();
}
```

Here is a more complex example:
```c++
void myFunction()
{
    // provide a path (or sub-path) to your repo
    QStringLiteral repoSubPath("/path/to/repo/or/any/sub-path");

    // Let the show begin!
    Git::Result r;
    Git::Repository repo = 
        Git::Repository::open( r, Git::Repository::discover( r, repoSubPath ) );

    if ( !r )
    {
        qCritical( "Unable to open repository at %s:\nGit error: %s",
                       qPrintable(repoSubPath),
                       qPrintable( r.errorText() ) );
        // return;
    }

    // No need to re-initialize "r"! If you want to, write:
    // r.clear();

    QString activeBranch = repo.currentBranch( r );
    if ( !r )
    {
        qCritical( "Unable to lookup the active branch."
                       "\nGit error: %s", qPrintable( r.errorText() ) );
    }

    // enjoy using GitWrap and go on coding
}
```

## A simple Example in Qt

This example shows how you `git init` with GitWrap.

```c++
#include "libGitWrap/Repository.hpp"
#include "libGitWrap/Result.hpp"

void initRepositoryExample()
{
    Git::Result r;
    QStringLiteral path( "/here/is/my/new/repository" );
    bool bare = false;
    Git::Repository newRepo = Git::Repository::create( path, bare, r );
    if ( !r )
    {
        qCritical( "Unable to create repository at %s."
                       "\nGit error: %s", 
                       qPrintable( path ), qPrintable( r.errorText() ) );
    }
}
```

# How to compile libGitWrap

For now, the only way to install and use GitWrap is compiling from source code.

## Pre-Requisites

To compile GitWrap, you need the following install:

* RAD-Tools (read "Install the RAD-Tools" below)
  * You get them [here]()!
* CMake ( >= 2.8.12 )
* Qt >= 5.3 (Qt4 might work, but you're on your own!)
* The C++ compiler of your choice! We recommend clang/llvm.

## Got it! Now how do I compile it?

These are the build instructions for OSX and Linux.

### Install the RAD-Tools

The RAD-Tools are a collection of CMake extension modules to simplify the build process. Nothing more, nothing less!

```bash
# clone the repo into a path of your choice
git clone https://github.com/cunz-rad/RAD-Tools.git
cd RAD-Tools
mkdir build && cd build
cmake ..
sudo make install
```

### Build and Install GitWrap - The easy way!

The easiest way to build GitWrap is by far using Qt-Creator. Just open the `CMakeLists.txt` file in the project's main directory and build the project. Done!

Hint: Set `-DCMAKE_BUILD_TYPE=Release` before running CMake to create a release build. You can change that any time later!

To install GitWrap, open a Terminal, go to the build directory (should be `Project-Dir/../libGitWrap-build`) and type:

```bash
sudo make install
```

### Build and Install GitWrap - From a Terminal

```bash
# clone the repo into a path of your choice
git clone --recursive https://github.com/macgitver/libGitWrap.git

# You need to provide the path to your Qt5 installation.
# This is an example:
export QTDIR=$HOME/Qt/5.4/gcc_64 
export PATH=$QTDIR/bin:$PATH

# now build & install!
mkdir libGitWrap-build && cd libGitWrap-build
cmake -DCMAKE_BUILD_TYPE=Release -- ../libGitWrap
make
sudo make install
```

# Want to help out?

Sure, all you need is a Github account. Fork the repo and send a pull request.

Create a feature branch with your initials like this:
If your Name is "Hans im Glück", write `git checkout -b hig/my-cool-feature master`

Tip: Don't forget to update the submodules with `git submodule update --init --recursive`


That's all folks! Happy coding with GitWrap!
