
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
    Blob.cpp
    BranchRef.cpp
    ChangeListConsumer.cpp
    Commit.cpp
    Config.cpp
    DiffList.cpp
    GitWrap.cpp
    Index.cpp
    IndexConflict.cpp
    IndexConflicts.cpp
    IndexEntry.cpp
    NoteRef.cpp
    Object.cpp
    ObjectId.cpp
    PatchConsumer.cpp
    RefName.cpp
    RefSpec.cpp
    Reference.cpp
    Remote.cpp
    RepoObject.cpp
    Repository.cpp
    Result.cpp
    RevisionWalker.cpp
    Signature.cpp
    Submodule.cpp
    Tag.cpp
    TagRef.cpp
    Tree.cpp
    TreeBuilder.cpp
    TreeEntry.cpp

    Events/IRemoteEvents.cpp
    Events/Private/RemoteCallbacks.cpp

    Operations/BaseOperation.cpp
    Operations/CheckoutOperation.cpp
    Operations/CloneOperation.cpp

    Operations/Private/WorkerThread.cpp
)

SET( PUB_HDR_FILES

    Base.hpp
    Blob.hpp
    BranchRef.hpp
    ChangeListConsumer.hpp
    Commit.hpp
    Config.hpp
    DiffList.hpp
    FileInfo.hpp
    GitWrap.hpp
    Index.hpp
    IndexConflict.hpp
    IndexConflicts.hpp
    IndexEntry.hpp
    NoteRef.hpp
    Object.hpp
    ObjectBlob.hpp
    ObjectCommit.hpp
    ObjectId.hpp
    ObjectTag.hpp
    ObjectTree.hpp
    RefName.hpp
    RefSpec.hpp
    Reference.hpp
    Remote.hpp
    RepoObject.hpp
    Repository.hpp
    Result.hpp
    RevisionWalker.hpp
    Signature.hpp
    Submodule.hpp
    Tag.hpp
    TagRef.hpp
    Tree.hpp
    TreeBuilder.hpp
    TreeEntry.hpp

    Operations/BaseOperation.hpp
    Operations/CheckoutOperation.hpp
    Operations/CloneOperation.hpp

    Events/Private/RemoteCallbacks.hpp
)

SET( PRI_HDR_FILES

    Private/BasePrivate.hpp
    Private/BlobPrivate.hpp
    Private/BranchRefPrivate.hpp
    Private/CommitPrivate.hpp
    Private/ConfigPrivate.hpp
    Private/DiffListPrivate.hpp
    Private/GitWrapPrivate.hpp
    Private/IndexConflictPrivate.hpp
    Private/IndexEntryPrivate.hpp
    Private/IndexPrivate.hpp
    Private/ObjectPrivate.hpp
    Private/NoteRefPrivate.hpp
    Private/ReferencePrivate.hpp
    Private/RefNamePrivate.hpp
    Private/RemotePrivate.hpp
    Private/RepoObjectPrivate.hpp
    Private/RepositoryPrivate.hpp
    Private/RevisionWalkerPrivate.hpp
    Private/SubmodulePrivate.hpp
    Private/TagPrivate.hpp
    Private/TagRefPrivate.hpp
    Private/TreeBuilderPrivate.hpp
    Private/TreeEntryPrivate.hpp
    Private/TreePrivate.hpp

    Events/Private/RemoteCallbacks.hpp

    Operations/Private/BaseOperationPrivate.hpp
    Operations/Private/CheckoutOperationPrivate.hpp
    Operations/Private/CloneOperationPrivate.hpp
    Operations/Private/WorkerThread.hpp
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
