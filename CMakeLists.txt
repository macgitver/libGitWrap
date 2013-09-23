
PROJECT( libGitWrap )

SET( libGitWrap_includes
    ${CMAKE_CURRENT_LIST_DIR}
    CACHE INTERNAL "libGitWrap include directories"
)

ADD_SUBDIRECTORY(libGitWrap)
ADD_SUBDIRECTORY(Tests)

ADD_DOXYGEN_TARGET(
    libGitWrap
    ${libGitWrap_SOURCE_DIR}/doc

    ${MGV_GITWRAP_SOURCE_DIR}
    EXCLUDE ${MGV_GITWRAP_SOURCE_DIR}/libgit2
)
