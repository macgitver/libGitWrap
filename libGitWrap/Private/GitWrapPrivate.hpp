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
#include <QSharedData>
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

    namespace Internal
    {

        class IndexPrivate;
        class ObjectPrivate;
        class RepositoryPrivate;

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
        public:
            Buffer();
            ~Buffer();

        public:
            operator git_buf*();
            operator const char*() const;
            operator QString() const;

        public:
            QString toString() const;

        private:
            Buffer(const Buffer& other);
            Buffer& operator =(const Buffer& other);

        private:
            git_buf         buf;
        };


        /**
         * @internal
         * @ingroup     GitWrap
         * @brief       Wraps an existing git_strarray for conversion from and to a QStringList.
         */
        class StrArrayRef : public QSharedData
        {
        public:
            typedef QExplicitlySharedDataPointer<StrArrayRef> Ptr;

        public:
            explicit StrArrayRef(git_strarray& _a, bool init = false);
            explicit StrArrayRef(git_strarray& _a, const QStringList& sl);
            virtual ~StrArrayRef();

        public:
            bool operator ==(const git_strarray& other) const;
            bool operator !=(const git_strarray& other) const;
            bool operator ==(const git_strarray* other) const;
            bool operator !=(const git_strarray* other) const;
            operator QStringList() const;

        private:
            /* Cannot privatize Copy+Default ctor because of the member-by-reference */
            StrArrayRef& operator=(const StrArrayRef&);

        public:
            void clear();
            int count() const;

            QStringList strings() const;
            void setStrings( const QStringList& strings );

        protected:
            git_strarray&       mEncoded;
            bool                mOwnsRef;
        };


        /**
         * @internal
         * @ingroup     GitWrap
         * @brief       Wraps a git_strarray for conversion from and to a QStringList.
         */
        class StrArray : public StrArrayRef
        {
        public:
            explicit StrArray();
            explicit StrArray(const QStringList& strings);

        public:
            operator git_strarray*();
            operator const git_strarray*() const;

        private:
            StrArray( const StrArray& other );
            StrArray& operator=(const StrArray& other);
        };


        // -- git_..._options wrappers

        /**
         * @internal
         * @ingroup     GitWrap
         * @brief       Thin Qt wrapper to reference an existing instance of git_checkout_options.
         */
        class CheckoutOptionsRef
        {
        public:
            CheckoutOptionsRef(git_checkout_options& ref, bool init = false);
            CheckoutOptionsRef(git_checkout_options& ref, const QStringList& paths, bool init = false);
            virtual ~CheckoutOptionsRef();

        public:
            operator git_checkout_options*();
            operator const git_checkout_options*() const;
            operator git_checkout_options&();

            git_checkout_options& operator *();

        public:
            QStringList paths() const;
            void setPaths( const QStringList& paths );

            QString targetDirectory() const;
            void setTargetDirectory( const QString& path );

            QString ancestorLabel() const;
            void setAncestorLabel(const QString& base );

            QString ourLabel() const;
            void setOurLabel(const QString& ours );

            QString theirLabel() const;
            void setTheirLabel(const QString& theirs );

        private:
            void init();

        private:
            git_checkout_options&   mOptionsRef;
            StrArrayRef::Ptr        mPaths;
        };

        /**
         * @internal
         * @ingroup     GitWrap
         * @brief       Thin Qt wrapper around git_checkout_options.
         */
        class CheckoutOptions : public CheckoutOptionsRef
        {
        public:
            CheckoutOptions();
            CheckoutOptions( const QStringList& paths );

        private:
            CheckoutOptions(const CheckoutOptions& other);
            CheckoutOptions& operator =(const CheckoutOptions& other);

        private:
            git_checkout_options            mOptionsRef;
        };

        /**
         * @internal
         * @ingroup     GitWrap
         * @brief       Wraps git_clone_options.
         */
        class CloneOptions
        {
        public:
            CloneOptions();
            ~CloneOptions();

        public:
            operator const git_clone_options*() const;
            operator git_clone_options&();

            git_clone_options& operator *();

        public:
            CheckoutOptionsRef checkoutOptions();

            QString checkoutBranch() const;
            void setCheckoutBranch(const QString& branch);

        private:
            git_clone_options       mOptions;
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
            case UnkownAttr:            return GIT_FILEMODE_UNREADABLE;
            case TreeAttr:              return GIT_FILEMODE_TREE;
            case FileAttr:              return GIT_FILEMODE_BLOB;
            case FileExecutableAttr:    return GIT_FILEMODE_BLOB_EXECUTABLE;
            case GitLinkAttr:           return GIT_FILEMODE_LINK;
            case SubmoduleAttr:         return GIT_FILEMODE_COMMIT;
            }
            Q_ASSERT( false );
            // Why is there no "Q_ASSUME( false );"???
            return GIT_FILEMODE_UNREADABLE;
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
        static inline git_otype objectType2git(ObjectType ot)
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

        inline const git_oid* const ObjectId2git(const ObjectId& id)
        {
            return (const git_oid* const) id.raw();
        }

        inline git_oid* ObjectId2git(ObjectId& id)
        {
            return (git_oid*) id.rawWritable();
        }

        inline const git_oid** ObjectIdList2git(Result& result, const ObjectIdList &list)
        {
            if (!result) return NULL;

            const git_oid** ret = new const git_oid *[list.count()];
            for ( int i=0; i < list.count(); ++i )
            {
                ret[i] = ObjectId2git( list[i] );
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


// -- internal macro definitions --8>

// string macro definitions --8>
/**
  * @internal
  * @ingroup GitWrap
  * @def Encode a QString @a s into a QByteArray with the UTF-8 codec used by libgit2.
  */
#define GW_EncodeQString(s) (s).toUtf8()

/**
  * @internal
  * @ingroup GitWrap
  * @def Encode a QString @a s with the UTF-8 codec used by libgit2.
  */
#define GW_StringFromQt(s) GW_EncodeQString(s).constData()

/**
  * @internal
  * @ingroup    GitWrap
  * @def        Encode a QString @a s with the UTF-8 codec used by libgit2.
  *             If @a s is empty, The default @a def will be set.
  */
#define GW_StringFromQt_Def(s, def) \
    (s).isEmpty() ? def : GW_EncodeQString(s).constData()

/**
  * @internal
  * @ingroup GitWrap
  * @def Macro to create a QString from an UTF-8 encoded libgit2 string.
  */
#define GW_StringToQt(s, ...) QString::fromUtf8(s, ##__VA_ARGS__)


// -- pimpl helper macro definitions -8>

#define GW__CHECK(returns, result) \
    do { \
        if (!Internal::BasePrivate::isValid(result, d)) { return returns; } \
    } while (0)

#define GW__EX_CHECK(returns, result) \
    do { \
        if (!Internal::BasePrivate::isValid(result, d.constData())) { return returns; } \
    } while (0)

#define GW__CHECK_VOID(result) \
    if (!Internal::BasePrivate::isValid(result, d)) { return; }

#define GW_D(CLASS) \
    Private* d = static_cast<Private*>(mData.data()); \
    ensureThisIsNotConst()

#define GW_D_EX(CLASS) \
    PrivatePtr d(static_cast<Private*>(mData.data())); \
    ensureThisIsNotConst()

#define GW_CD(CLASS) \
    const CLASS::Private* d = static_cast<const CLASS::Private*>(mData.constData())

#define GW_CD_EX(CLASS) \
    const CLASS::PrivatePtr d(static_cast<CLASS::Private*>(mData.data()))


#define GW_CD_CHECKED(CLASS, returns, result) \
    GW_CD(CLASS); \
    GW__CHECK(returns, result)

#define GW_CD_EX_CHECKED(CLASS, returns, result) \
    GW_CD_EX(CLASS); \
    GW__EX_CHECK(returns, result)

#define GW_D_CHECKED(CLASS, returns, result) \
    GW_D(CLASS); \
    GW__CHECK(returns, result)

#define GW_D_EX_CHECKED(CLASS, returns, result) \
    GW_D_EX(CLASS); \
    GW__EX_CHECK(returns, result)

#define GW_D_CHECKED_VOID(CLASS, result) \
    GW_D(CLASS); \
    GW__CHECK_VOID(result)

#define GW_CD_CHECKED_VOID(CLASS, result) \
    GW_CD(CLASS); \
    GW__CHECK_VOID(result)


#endif
