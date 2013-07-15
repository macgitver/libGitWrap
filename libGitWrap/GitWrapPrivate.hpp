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

#include <QThreadStorage>
#include <QStringList>

#include "git2.h"

#include "GitWrap.hpp"
#include "Result.hpp"
#include "ObjectId.hpp"

namespace Git
{

    class Signature;
    class RefSpec;

    namespace Internal
    {

        template< class T >
        inline GitPtr< T >::GitPtr()
            : d(0)
        {
        }

        template< class T >
        inline GitPtr< T >::GitPtr( const GitPtr< T >& o )
            : d( o.d )
        {
            if( d )
            {
                d->ref();
            }
        }

        template< class T >
        inline GitPtr< T >::GitPtr( T* o )
            : d( o )
        {
            if( d )
            {
                d->ref();
            }
        }

        template< class T >
        inline GitPtr< T >::~GitPtr()
        {
            if( d )
            {
                d->deref();
            }
        }

        template< class T >
        inline GitPtr< T >& GitPtr< T >::operator=( const GitPtr< T >& o )
        {
            T* x = d; d = o.d;
            if( x != d )
            {
                if( d ) d->ref();
                if( x ) x->deref();
            }
            return *this;
        }

        template< class T >
        inline bool GitPtr< T >::operator==( const GitPtr< T >& o ) const
        {
            return d == o.d;
        }

        template< class T >
        inline bool GitPtr< T >::operator==( T* o ) const
        {
            return d == d;
        }

        template< class T >
        inline T* GitPtr< T >::operator->()
        {
            return d;
        }

        template< class T >
        inline const T* GitPtr< T >::operator->() const
        {
            return d;
        }

        template< class T >
        inline T* GitPtr< T >::operator*()
        {
            return d;
        }

        template< class T >
        inline const T* GitPtr< T >::operator*() const
        {
            return d;
        }


        template< class T >
        inline GitPtr< T >::operator bool() const
        {
            return d;
        }

        template< class T >
        inline GitPtr< T >::operator T*()
        {
            return d;
        }

        template< class T >
        inline GitPtr< T >::operator const T*() const
        {
            return d;
        }

        template< class T >
        inline T* GitPtr< T >::data() const
        {
            return d;
        }

        class RepositoryPrivate;
        class IndexPrivate;
        class ObjectPrivate;

        // Some internal helpers
        Signature git2Signature( const git_signature* gitsig );
        git_signature* signature2git( Result& result, const Signature& sig );
        RefSpec mkRefSpec( const git_refspec* refspec );
        QStringList slFromStrArray( git_strarray* arry );

        /**
         * @internal
         * @ingroup GitWrap
         * @brief The StrArrayWrapper class wraps a QStringList as a pointer to git_strarray.
         */
        class StrArrayWrapper
        {
            git_strarray a;
            QStringList internalCopy;

        public:
            StrArrayWrapper(const QStringList& sl);
            ~StrArrayWrapper();

            operator git_strarray*();
            operator const git_strarray*() const;

        private:
            StrArrayWrapper();
            StrArrayWrapper(const StrArrayWrapper&);
        };

        /**
         * @internal
         * @ingroup     GitWrap
         * @brief       convert GitWrap's TreeEntryAttributes convert to LibGit2 file mode
         * @param[in]   attr    GitWrap's TreeEntryAttributes
         * @return      LibGit2's git_filemode_t
         */
        static inline git_filemode_t teattr2filemode( TreeEntryAttributes attr )
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
        static inline ObjectType gitotype2ObjectType( git_otype otype )
        {
            switch( otype )
            {
            case GIT_OBJ_BLOB:      return otBlob;
            case GIT_OBJ_COMMIT:    return otCommit;
            case GIT_OBJ_TREE:      return otTree;
            case GIT_OBJ_TAG:       return otTag;
            default:                Q_ASSERT( false );
                                    return otAny;
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

            if ( v & GIT_STATUS_CURRENT )           s |= FileUnchanged;
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

#endif
