
PROJECT(GitWrap)
CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

CMAKE_POLICY( SET CMP0043 NEW )

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

IF(NOT MSVC)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
ENDIF()

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
