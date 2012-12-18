
PROJECT( MGV_GITWRAP )

QT_PREPARE( Core -Gui )

INCLUDE_DIRECTORIES(
    ${MGV_GITWRAP_SOURCE_DIR}
    ${MGV_GITWRAP_BINARY_DIR}
)

SET( LIBGIT2_BASE "libgit2" )

MACRO( GCC_ADD_C_FLAG _flag )
    IF( NOT DEFINED CMAKE_C_FLAGS )
        SET( CMAKE_C_FLAGS "${_flag}" CACHE STRING "C++ Build Flags" FORCE )
    ELSEIF( NOT "${CMAKE_C_FLAGS}" MATCHES ".*${_flag}.*" )
        SET( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${_flag}" CACHE STRING "C Build Flags" FORCE )
    ENDIF()
ENDMACRO()

MACRO( ADD_GLOBS var )
    FOREACH( glob ${ARGN} )
        FILE( GLOB data ${glob} )
        LIST( APPEND ${var} ${data} )
    ENDFOREACH()
ENDMACRO()

FIND_PACKAGE( ZLIB )
IF( NOT WIN32 )
    FIND_PACKAGE( Threads )
ENDIF()

INCLUDE_DIRECTORIES(
    ${LIBGIT2_BASE}/include
    ${LIBGIT2_BASE}/src
)

# libgit2 files themselves
SET( LIBGIT2_FILES "" )
ADD_GLOBS(
    LIBGIT2_FILES
    ${LIBGIT2_BASE}/src/*.[ch]
    ${LIBGIT2_BASE}/src/transports/*.[ch]
    ${LIBGIT2_BASE}/src/xdiff/*.[ch]
    ${LIBGIT2_BASE}/include/*.h
    ${LIBGIT2_BASE}/include/git2/*.h
)

IF( ZLIB_FOUND )
    INCLUDE_DIRECTORIES( ${ZLIB_INCLUDE_DIRS} )
ELSE()
    INCLUDE_DIRECTORIES( ${LIBGIT2_BASE}/deps/zlib )
    ADD_DEFINITIONS( -DNO_VIZ -DSTDC -DNO_GZIP )
    ADD_GLOBS(
        LIBGIT2_FILES
        ${LIBGIT2_BASE}/deps/zlib/*.[ch]
    )
ENDIF()

IF( WIN32 )
    ADD_GLOBS(
        LIBGIT2_FILES
        ${LIBGIT2_BASE}/src/win32/*.[ch]
        ${LIBGIT2_BASE}/deps/regex/regex.c
    )
    INCLUDE_DIRECTORIES(
        ${LIBGIT2_BASE}/deps/regex
    )
    
    ADD_DEFINITIONS( -DWIN32 -D_DEBUG -D_WIN32_WINNT=0x0501 )
    
    IF( NOT MINGW )
        ADD_DEFINITIONS( -DGIT_WINHTTP )

        ADD_GLOBS(
            LIBGIT2_FILES
            ${LIBGIT2_BASE}/src/hash/hash_win32.[ch]
        )
        ADD_DEFINITIONS( -DWIN32_SHA1 )
    ELSE()
        FIND_PACKAGE(OpenSSL)

        ADD_GLOBS(
            LIBGIT2_FILES
            ${LIBGIT2_BASE}/deps/http-parser/*.[ch]
            ${LIBGIT2_BASE}/src/hash/hash_generic.[ch]
        )
    ENDIF()

ELSEIF( UNIX )
    FIND_PACKAGE( Threads REQUIRED )

    ADD_GLOBS(
        LIBGIT2_FILES
        ${LIBGIT2_BASE}/src/hash/hash_generic.[ch]
    )

ELSEIF( APPLE )
    FIND_PACKAGE( Threads REQUIRED )

    ADD_GLOBS(
        LIBGIT2_FILES
        ${LIBGIT2_BASE}/src/hash/hash_generic.[ch]
    )

ENDIF()

ADD_DEFINITIONS( -D_FILE_OFFSET_BITS=64 -DGIT_THREADS )

SET( SRC_FILES

    BasicObject.cpp
    Config.cpp
    ChangeListConsumer.cpp
    DiffList.cpp
    GitWrap.cpp
    Index.cpp
    Object.cpp
    ObjectBlob.cpp
    ObjectCommit.cpp
    ObjectId.cpp
    ObjectTag.cpp
    ObjectTree.cpp
    PatchConsumer.cpp
    RefSpec.cpp
    Reference.cpp
    Remote.cpp
    RepoObject.cpp
    Repository.cpp
    Result.cpp
    RevisionWalker.cpp
    Signature.cpp
    Submodule.cpp
    TreeBuilder.cpp
    TreeEntry.cpp
    BackgroundTasks.cpp
    BackgroundFetch.cpp
    BackgroundCheckout.cpp
    BackgroundClone.cpp
)

SET( PUB_HDR_FILES

    Result.hpp
    Config.hpp
    ChangeListConsumer.hpp
    GitWrap.hpp
    DiffList.hpp
    Index.hpp
    Object.hpp
    ObjectBlob.hpp
    ObjectCommit.hpp
    ObjectId.hpp
    ObjectTag.hpp
    ObjectTree.hpp
    RefSpec.hpp
    Reference.hpp
    Remote.hpp
    Repository.hpp
    RevisionWalker.hpp
    Signature.hpp
    Submodule.hpp
    TreeBuilder.hpp
    TreeEntry.hpp
    BackgroundTasks.hpp
    BackgroundFetch.hpp
    BackgroundCheckout.hpp
    BackgroundClone.hpp
)

SET( PRI_HDR_FILES

    GitWrapPrivate.hpp
    BasicObject.hpp
    ConfigPrivate.hpp
    DiffListPrivate.hpp
    IndexPrivate.hpp
    ObjectPrivate.hpp
    ReferencePrivate.hpp
    RemotePrivate.hpp
    RepoObject.hpp
    RepositoryPrivate.hpp
    RevisionWalkerPrivate.hpp
    TreeBuilderPrivate.hpp
    TreeEntryPrivate.hpp
)

SET( HDR_FILES ${PRI_HDR_FILES} ${PUB_HDR_FILES} )

QT_MOC( MOC_FILES ${HDR_FILES} )

ADD_QT_LIBRARY(
    GitWrap SHARED

    ${SRC_FILES}
    ${HDR_FILES}
    ${MOC_FILES}
    ${LIBGIT2_FILES}
)

FUNCTION( MSVC_SPLIT_SOURCES target )
    GET_TARGET_PROPERTY( sources ${target} SOURCES )

    FOREACH( source ${sources} )
    
        IF( NOT source MATCHES ".*/" )

            SET( rel "Sources" )
            
        ELSE()
            STRING( REPLACE ${CMAKE_CURRENT_BINARY_DIR}/ "Sources/Generated/" rel ${source} )
            STRING( REPLACE ${CMAKE_CURRENT_SOURCE_DIR}/ "Sources/" rel ${rel} )
            STRING( REGEX REPLACE "/([^/]*)$" "" rel ${rel} )
        ENDIF()

        STRING( REPLACE "/" "\\\\" rel ${rel} )
        SOURCE_GROUP( ${rel} FILES ${source} )
    ENDFOREACH()
ENDFUNCTION()

IF( MSVC )
    SET_TARGET_PROPERTIES( GitWrap PROPERTIES COMPILE_FLAGS "/FIforce_no_export.h" )
    MSVC_SPLIT_SOURCES( GitWrap )
ENDIF()
