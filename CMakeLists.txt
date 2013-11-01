
PROJECT( libGitWrap )

SET( libGitWrap_includes
    ${CMAKE_CURRENT_LIST_DIR}
    CACHE INTERNAL "libGitWrap include directories"
)

# Set the Include-Directory here, so we can use "libGitWrap/XXX" includes inside libGitWrap and be
# sure forever that they will work on an installed source tree...
INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

#ADD_DEFINITIONS(-DGW_NO_DEPRECATION)

ADD_SUBDIRECTORY(libGitWrap)
ADD_SUBDIRECTORY(testGitWrap)

ADD_DOXYGEN_TARGET(
    libGitWrap
    ${libGitWrap_SOURCE_DIR}/doc

    ${MGV_GITWRAP_SOURCE_DIR}
    EXCLUDE ${MGV_GITWRAP_SOURCE_DIR}/libgit2
)
