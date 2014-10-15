/*
 * MacGitver
 * Copyright (C) 2012-2013 Sascha Cunz <sascha@babbelbox.org>
 *
 * This program is free software; you can redistribute it and/or modify it under the terms of the
 * GNU General Public License (Version 2) as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program; if
 * not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef GIT_P_H
#define GIT_P_H

#include <QDebug>
#include <QTextCodec>
#include <QThreadStorage>

#include "git2.h"
#include "git2/sys/commit.h"

#include "libGitWrap/GitWrap.hpp"
#include "libGitWrap/Result.hpp"
#include "libGitWrap/ObjectId.hpp"

namespace Git
{

    class FileInfo;
    class Signature;
    class RefSpec;
    class IRemoteEvents;

    namespace Internal
    {

        class RepositoryPrivate;
        class IndexPrivate;
        class ObjectPrivate;

        // Some internal helpers
        Signature git2Signature( const git_signature* gitsig );
        git_signature* signature2git( Result& result, const Signature& sig );
        RefSpec mkRefSpec( const git_refspec* refspec );
        QStringList slFromStrArray( git_strarray* arry );
        FileInfo mkFileInfo(const git_diff_file* df);

        /**
         * @internal
         * @ingroup     GitWrap
         * @brief       The Buffer class wraps a git_buf.
         */
        class Buffer {
        private:
            git_buf         buf;
            QTextCodec*     mCodec;

        public:
            Buffer( QTextCodec* codec = 0 );
            ~Buffer();

        public:
            operator git_buf*();
            operator const char*();

        public:
            QString toString() const;

        private:
            Buffer(const Buffer& other);
            Buffer& operator =(const Buffer& other);
        };

        /**
         * @internal
         * @ingroup     GitWrap
         * @brief       Wraps a QString for usage in libgit2.
         *
         * The QString is encoded to the codec used by libgit2. Defaults to UTF-8.
         */
        class String
        {
        public:
            String( QString str, QTextCodec* codec = 0);
            String( const char* str, QTextCodec* codec = 0 );
            String( const char* str, int size, QTextCodec* codec = 0 );

        public:
            operator const char*();
            operator char*();
            operator QString();

        public:
            static QString convert( const char* str, int size, QTextCodec* codec = 0 );
            static QString convert( const char* str, QTextCodec* codec = 0 );

        public:
            QByteArray toArray() const;

        private:
            const char* convert();

        private:
            String(const String& other);
            String& operator =(const String& other);

        private:
            QString         mStr;
            QByteArray      mConvertedStr;  //!< the string is converted, using QString's conversion methods
            QTextCodec *    mCodec;
        };

        /**
         * @internal
         * @ingroup     GitWrap
         * @brief       The StrArrayWrapper class wraps a QStringList as a pointer to git_strarray.
         */
        class StrArrayWrapper
        {
        public:
            StrArrayWrapper(const QStringList& sl);
            ~StrArrayWrapper();

            operator git_strarray*();
            operator const git_strarray*() const;

        private:
            StrArrayWrapper();
            StrArrayWrapper(const StrArrayWrapper&);
            StrArrayWrapper& operator=(const StrArrayWrapper&);

        private:
            git_strarray a;
            QStringList internalCopy;
        };

        class StrArray
        {
        public:
            StrArray(git_strarray& _a, const QStringList& sl);
            ~StrArray();

        private:
            /* Cannot privatize Copy+Default ctor because of the member-by-reference */
            StrArray& operator=(const StrArray&);

        private:
            git_strarray& a;
            QStringList internalCopy;
        };

        /**
         * @internal
         * @ingroup     GitWrap
         * @brief       convert GitWrap's TreeEntryAttributes convert to LibGit2 file mode
         * @param[in]   attr    GitWrap's TreeEntryAttributes
         * @return      LibGit2's git_filemode_t
         */
        static inline git_filemode_t teattr2filemode( FileModes attr )
        {
            switch( attr )
            {
            case UnkownAttr:            return GIT_FILEMODE_NEW;
            case TreeAttr:              return GIT_FILEMODE_TREE;
            case FileAttr:              return GIT_FILEMODE_BLOB;
            case FileExecutableAttr:    return GIT_FILEMODE_BLOB_EXECUTABLE;
            case GitLinkAttr:           return GIT_FILEMODE_LINK;
            case SubmoduleAttr:         return GIT_FILEMODE_COMMIT;
            }
            Q_ASSERT( false );
            // Why is there no "Q_ASSUME( false );"???
            return GIT_FILEMODE_NEW;
        }

        /**
         * @internal
         * @ingroup     GitWrap
         * @brief       Convert LibGit2 object type to GitWrap object type
         * @param[in]   otype   LibGit2's object type
         * @return      LibGitWrap's object type
         */
        static inline ObjectType gitotype2ObjectType(git_otype otype)
        {
            switch (otype) {
            default:                Q_ASSERT(false);
            case GIT_OBJ_ANY:       return otAny;
            case GIT_OBJ_BLOB:      return otBlob;
            case GIT_OBJ_COMMIT:    return otCommit;
            case GIT_OBJ_TREE:      return otTree;
            case GIT_OBJ_TAG:       return otTag;
            }
        }

        /**
         * @internal
         * @ingroup     GitWrap
         * @brief       Convert ObjectType to GIT_OBJ_xxx constant
         *
         * @param[in]   ot      The ObjectType to convert
         *
         * @return      The GIT_OBJ_xxx constant
         *
         */
        static inline git_otype objectType2gitotype(ObjectType ot)
        {
            switch (ot) {
            default:        Q_ASSERT(false);
            case otAny:     return GIT_OBJ_ANY;
            case otBlob:    return GIT_OBJ_BLOB;
            case otCommit:  return GIT_OBJ_COMMIT;
            case otTree:    return GIT_OBJ_TREE;
            case otTag:     return GIT_OBJ_TAG;
            }
        }

        class GitWrapTLS
        {
        public:
            Result			mLastResult;
        };

        class GitWrapPrivate
        {
        public:
            QThreadStorage< GitWrapTLS* >	mTLStore;

            static GitWrapPrivate* self;    // Make this an QAtomicPointer
        };

        inline const git_oid* const ObjectId2git_oid(const ObjectId& id)
        {
            return (const git_oid* const) id.raw();
        }

        inline git_oid* ObjectId2git_oid(ObjectId& id)
        {
            return (git_oid*) id.rawWritable();
        }

        inline const git_oid** ObjectIdList2git(Result& result, const ObjectIdList &list)
        {
            if (!result) return NULL;

            const git_oid** ret = new const git_oid *[list.count()];
            for ( int i=0; i < list.count(); ++i )
            {
                ret[i] = ObjectId2git_oid( list[i] );
            }

            return ret;
        }

        /**
         * @internal
         * @ingroup     GitWrap
         * @brief       convertFileStatus Converts a value to GitWrap's File::Status.
         *
         *              The value is assumed to be the enum type git_status_t in libgit2.
         *
         * @param[in]   v       the libgit2 git_status_t value
         *
         * @return      the File::Status
         */
        static inline StatusFlags convertFileStatus( unsigned int v )
        {
            StatusFlags s = FileInvalidStatus;

            if ( v == GIT_STATUS_CURRENT )          return FileUnchanged;

            if ( v & GIT_STATUS_INDEX_NEW )         s |= FileIndexNew;
            if ( v & GIT_STATUS_INDEX_MODIFIED )    s |= FileIndexModified;
            if ( v & GIT_STATUS_INDEX_DELETED )     s |= FileIndexDeleted;
            if ( v & GIT_STATUS_INDEX_RENAMED )     s |= FileIndexRenamed;
            if ( v & GIT_STATUS_INDEX_TYPECHANGE )  s |= FileIndexTypeChange;
            if ( v & GIT_STATUS_WT_NEW )            s |= FileWorkingTreeNew;
            if ( v & GIT_STATUS_WT_MODIFIED )       s |= FileWorkingTreeModified;
            if ( v & GIT_STATUS_WT_DELETED )        s |= FileWorkingTreeDeleted;
            if ( v & GIT_STATUS_WT_TYPECHANGE )     s |= FileWorkingTreeTypeChange;
            if ( v & GIT_STATUS_IGNORED )           s |= FileIgnored;

            return s;
        }

    }

}

#define GW__CHECK(returns, result) \
    if (!Private::isValid(result, d)) { return returns; }

#define GW__CHECK_VOID(result) \
    if (!Private::isValid(result, d)) { return; }


#define GW__EX_CHECK(returns, result) \
    if (!Private::isValid(result, d.constData())) { return returns; }

#define GW__EX_CHECK_VOID(result) \
    if (!Private::isValid(result, d.constData())) { return; }

#define GW_D(CLASS) \
    Private* d = static_cast<Private*>(mData.data()); \
    ensureThisIsNotConst()

#define GW_D_EX(CLASS) \
    PrivatePtr d(static_cast<Private*>(mData.data())); \
    ensureThisIsNotConst()

#define GW_CD(CLASS) \
    const Private* d = static_cast<const Private*>(mData.constData())

#define GW_CD_EX(CLASS) \
    const PrivatePtr d(static_cast<Private*>(mData.data()))

#define GW_CD_EX_CHECKED(CLASS, returns, result) \
    GW_CD_EX(CLASS); \
    GW__EX_CHECK(returns, result)

#define GW_D_CHECKED(CLASS, returns, result) \
    GW_D(CLASS); \
    GW__CHECK(returns, result)

#define GW_D_EX_CHECKED(CLASS, returns, result) \
    GW_D_EX(CLASS); \
    GW__EX_CHECK(returns, result)

#define GW_CD_CHECKED(CLASS, returns, result) \
    GW_CD(CLASS); \
    GW__CHECK(returns, result)

// Wherever we have to use one of those two, we've made bad API design!
#define GW_D_CHECKED_VOID(CLASS, result) \
    GW_D(CLASS); \
    GW__CHECK_VOID(result)

#define GW_CD_CHECKED_VOID(CLASS, result) \
    GW_CD(CLASS); \
    GW__CHECK_VOID(result)

#endif
