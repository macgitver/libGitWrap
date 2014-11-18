
PROJECT(GitWrap)
set( PROJECT_MAINTAINER "Nils Fenner" )
set( PROJECT_CONTACT "Nils Fenner (My personal key) <nilsfenner@web.de>" )
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)

FIND_PACKAGE(RAD-Tools REQUIRED)

RAD_INIT(
    FEATURES
        QtMacros
        Installation
        DoxygenTarget
        SplitSources
)

# Set the Include-Directory here, so we can use "libGitWrap/XXX" includes inside libGitWrap and be
# sure forever that they will work on an installed source tree...
INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

#ADD_DEFINITIONS(-DGW_NO_DEPRECATION)

INCLUDE(cmake/GoogleTest.cmake)

ADD_SUBDIRECTORY(libGitWrap)
ADD_SUBDIRECTORY(testGitWrap)

RAD_CREATE_PACKAGE(
    GitWrap

    TEMPLATE_DIR
        cmake

    TARGETS
        GitWrap
)

RAD_ADD_DOXYGEN_TARGET(
    libGitWrap
    ${libGitWrap_SOURCE_DIR}/doc

    ${MGV_GITWRAP_SOURCE_DIR}
    EXCLUDE ${MGV_GITWRAP_SOURCE_DIR}/libgit2
)


#== Packaging ==

# To Create a package go into the build directory
# and run one of the following commands:
#
# - On Mac OSX run: cpack -G "DragNDrop"
# - On Windows run: cpack -G "NSIS"
# - On Linux, you have the following options:
#   1. To create a DEB package, run "make package"
#   2. To upload the PPA for Ubuntu, enable the "PreparePPA" flag
#      and (after building) run "make dput" to upload the PPA.
#      Make sure, the "devscripts" package installed.
#   3. (NOT IMPLEMENTED YET) -- To create an RPM package run: cpack -G "RPM"
#
# This will create the package from the "CpackConfig.cmake" file.
#
# You may want to use other package generators like e.g. RPM.
# To do so, just change the attribute after the "-G" parameter


# == Environment ==
if(NOT CPACK_SYSTEM_NAME)
    set(CPACK_SYSTEM_NAME "${CMAKE_SYSTEM_PROCESSOR}")
    if(CPACK_SYSTEM_NAME STREQUAL "x86_64")
        set(CPACK_SYSTEM_NAME "amd64")
    endif()
endif()

# == CPack generstor ==
IF( APPLE )
    SET( CPACK_BINARY_DRAGNDROP         ON )
ELSEIF( LINUX )
    SET( CPACK_GENERATOR                TGZ TBZ2 DEB RPM )
    SET( CPACK_SOURCE_GENERATOR         TGZ TBZ2 )
ELSEIF( WIN32 )
    SET( CPACK_BINARY_WIX               ON )
ENDIF()

# == Basic information ==
SET( CPACK_PACKAGE_NAME                  "libGitWrap" )
SET( CPACK_PACKAGE_VERSION               "0.1.0" )
SET( CPACK_PACAGE_VENDOR                 "http://macgitver.info" )
SET( CPACK_PACKAGE_CONTACT               "${PROJECT_CONTACT}" )
SET( CPACK_PACKAGE_DESCRIPTION_SUMMARY   "Qt API for libgit2." )
SET( CPACK_PACKAGE_DESCRIPTION           "The GitWrap library wraps the libgit2 library for Qt applications." )
#SET( CPACK_PACKAGE_DESCRIPTION_FILE      "${CMAKE_CURRENT_SOURCE_DIR}/DESCRIPTION" )


# == DEB package config ==
SET( CPACK_DEBIAN_PACKAGE_MAINTAINER        "${PROJECT_MAINTAINER}" )
SET( CPACK_DEBIAN_PACKAGE_ARCHITECTURE      "${CPACK_SYSTEM_NAME}" )
SET( CPACK_DEBIAN_PACKAGE_SECTION           "devel" )
SET( CPACK_DEBIAN_PACKAGE_PRIORITY          "optional" )
#SET( CPACK_DEBIAN_PACKAGE_DEPENDS          "libgit2 (>=0.21.1)" )
set( CPACK_DEBIAN_BUILD_DEPENDS             debhelper cmake )

# == RPM package config ==
#SET( CPACK_RPM_PACKAGE_REQUIRES "" )
## == Source package config ==


include( CPack )

if( ${PreparePPA} )
    # TODO: This should be asked from the distro
    set( UBUNTU_NAME    "trusty" )
    set( DPUT_HOST      "ppa:nilsfenner/macgitver-dev" )

    include( cmake/UploadPPA.cmake )
endif()
