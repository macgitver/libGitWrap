
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

IF( NOT WIN32 )
    FIND_PACKAGE( Threads )
    GCC_ADD_C_FLAG(-Wno-implicit-function-declaration)
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

FIND_PACKAGE( ZLIB )
IF( ZLIB_FOUND )
    INCLUDE_DIRECTORIES( ${ZLIB_INCLUDE_DIRS} )
    LIST( APPEND GIT_LIBS ${ZLIB_LIBRARIES} )
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
    LIST(APPEND GIT_LIBS ${CMAKE_THREAD_LIBS_INIT})

    ADD_GLOBS(
        LIBGIT2_FILES
        ${LIBGIT2_BASE}/src/hash/hash_generic.[ch]
        ${LIBGIT2_BASE}/src/unix/*.[ch]
        ${LIBGIT2_BASE}/deps/http-parser/*.[ch]
    )

    INCLUDE_DIRECTORIES(
        ${LIBGIT2_BASE}/deps/http-parser
    )

ENDIF()

IF( OPENSSL_FOUND )
    ADD_DEFINITIONS( -DGIT_SSL )
    INCLUDE_DIRECTORIES( ${OPENSSL_INCLUDE_DIR} )
    SET( SSL_LIBRARIES ${OPENSSL_LIBRARIES} )
ENDIF()

ADD_DEFINITIONS( -D_FILE_OFFSET_BITS=64 -DGIT_THREADS )

SET( SRC_FILES

    Base.cpp
    RepoObject.cpp
    Config.cpp
    ChangeListConsumer.cpp
    CloneOperation.cpp
    DiffList.cpp
    GitWrap.cpp
    IFetchEvents.cpp
    Index.cpp
    IndexEntry.cpp
    IndexConflict.cpp
    IndexConflicts.cpp
    Object.cpp
    ObjectId.cpp
    PatchConsumer.cpp
    RefName.cpp
    RefSpec.cpp
    Reference.cpp
    Remote.cpp
    Repository.cpp
    Result.cpp
    RevisionWalker.cpp
    Signature.cpp
    Submodule.cpp
    TreeBuilder.cpp
    TreeEntry.cpp
    Tag.cpp
    Tree.cpp
    Commit.cpp
    Blob.cpp

    Private/FetchCallbacks.cpp
    Private/WorkerThread.cpp
)

SET( PUB_HDR_FILES

    Base.hpp
    RepoObject.hpp
    Result.hpp
    Config.hpp
    ChangeListConsumer.hpp
    CloneOperation.hpp
    DiffList.hpp
    GitWrap.hpp
    IFetchEvents.hpp
    Index.hpp
    IndexEntry.hpp
    IndexConflict.hpp
    IndexConflicts.hpp
    Object.hpp
    ObjectBlob.hpp
    ObjectCommit.hpp
    ObjectId.hpp
    ObjectTag.hpp
    ObjectTree.hpp
    RefSpec.hpp
    RefName.hpp
    Reference.hpp
    Remote.hpp
    Repository.hpp
    RevisionWalker.hpp
    Signature.hpp
    Submodule.hpp
    TreeBuilder.hpp
    TreeEntry.hpp
    Tag.hpp
    Tree.hpp
    Commit.hpp
    Blob.hpp
)

SET( PRI_HDR_FILES

    Private/BasePrivate.hpp
    Private/BlobPrivate.hpp
    Private/CloneOperationPrivate.hpp
    Private/CommitPrivate.hpp
    Private/ConfigPrivate.hpp
    Private/DiffListPrivate.hpp
    Private/FetchCallbacks.hpp
    Private/GitWrapPrivate.hpp
    Private/IndexConflictPrivate.hpp
    Private/IndexEntryPrivate.hpp
    Private/IndexPrivate.hpp
    Private/ObjectPrivate.hpp
    Private/ReferencePrivate.hpp
    Private/RemotePrivate.hpp
    Private/RepoObjectPrivate.hpp
    Private/RepositoryPrivate.hpp
    Private/RevisionWalkerPrivate.hpp
    Private/TagPrivate.hpp
    Private/TreeBuilderPrivate.hpp
    Private/TreeEntryPrivate.hpp
    Private/TreePrivate.hpp
    Private/WorkerThread.hpp

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

TARGET_LINK_LIBRARIES(
    GitWrap

    LINK_PRIVATE
        ${GIT_LIBS}
)

IF( MSVC )
    SET_TARGET_PROPERTIES( GitWrap PROPERTIES COMPILE_FLAGS "/FIforce_no_export.h" )
    MSVC_SPLIT_SOURCES( GitWrap )
ENDIF()

INSTALL_FRAMEWORK( GitWrap Runtime )
