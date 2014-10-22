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

#include "libGitWrap/Index.hpp"
#include "libGitWrap/Repository.hpp"
#include "libGitWrap/FileInfo.hpp"

#include "libGitWrap/Private/GitWrapPrivate.hpp"

namespace Git
{

    namespace Internal
    {

        GitWrapPrivate* GitWrapPrivate::self = NULL;

        /**
         * @internal
         * @brief       Create a QStringList from a git_starray
         * @param[in]   arry    Pointer to the git_starrray
         * @return      A QStringList with all entries of @a arry converted to UTF-8.
         */
        QStringList slFromStrArray( git_strarray* arry )
        {
            QStringList sl;

            for( unsigned int i = 0; i < arry->count; i++ )
            {
                sl << GW_StringToQt( arry->strings[ i ] );
            }

            git_strarray_free( arry );
            return sl;
        }


        //-- Buffer ----------------------------------------------------------------------------- >8

        Buffer::Buffer()
        {
            memset(&buf, 0, sizeof(buf));
        }

        Buffer::~Buffer()
        {
            git_buf_free( &buf );
        }

        Buffer::operator QString() const
        {
            return toString();
        }

        Buffer::operator git_buf*()
        {
            return &buf;
        }

        Buffer::operator const char*() const
        {
            return buf.ptr;
        }

        /**
         * @brief        Converts the contents of the buffer into a QString.
         *
         * @return       the buffer in readable text format
         */
        QString Buffer::toString() const
        {
            return GW_StringToQt( buf.ptr, buf.size );
        }


        //-- CheckoutOptions -------------------------------------------------------------------- >8

        CheckoutOptions::CheckoutOptions()
        {
            git_checkout_init_options( &mOptions, GIT_CHECKOUT_OPTIONS_VERSION );
            mOptions.paths = *mPaths;
        }

        CheckoutOptions::operator git_checkout_options*()
        {
            return &mOptions;
        }

        CheckoutOptions::operator const git_checkout_options*() const
        {
            return &mOptions;
        }

        CheckoutOptions::operator git_checkout_options&()
        {
            return mOptions;
        }

        git_checkout_options& CheckoutOptions::operator *()
        {
            return mOptions;
        }

        const QStringList& CheckoutOptions::paths() const
        {
            return mPaths;
        }

        void CheckoutOptions::setPaths( const QStringList& paths )
        {
            mPaths = StrArray( paths );
        }


        //-- StrArray --------------------------------------------------------------------------- >8

        StrArray::StrArray(const QStringList& sl)
            : mInternalCopy( sl )
        {
            mEncoded.count = mInternalCopy.count();
            mEncoded.strings = new char *[mEncoded.count];

            for( int i = 0; i < mInternalCopy.count(); i++ )
            {
                strcpy( mEncoded.strings[i], GW_EncodeQString( mInternalCopy[i] ).data() );
            }
        }

        StrArray::~StrArray()
        {
            delete[] mEncoded.strings;
        }

        StrArray::StrArray(const StrArray& other)
        {
            *this = other;
        }

        StrArray& StrArray::operator =( const StrArray& other )
        {
            if ( (&other == this) || (other.mEncoded.strings == mEncoded.strings) )
            {
                return *this;
            }

            // TODO: mEncoded should probably become a QSharedPointer
            delete[] mEncoded.strings;

            mInternalCopy = other.mInternalCopy;
            mEncoded = other.mEncoded;

            return *this;
        }

        StrArray::operator git_strarray*()
        {
            return &mEncoded;
        }

        StrArray::operator const git_strarray *() const
        {
            return &mEncoded;
        }

        StrArray::operator const QStringList &() const
        {
            return mInternalCopy;
        }

        //-- StrArrayRef ------------------------------------------------------------------------ >8

        StrArrayRef& StrArrayRef::operator=(const StrArrayRef&)
        {
            Q_ASSERT(false);
            return *this;
        }

        StrArrayRef::StrArrayRef(git_strarray& _a, const QStringList& sl)
            : mEncoded(_a)
            , mInternalCopy( sl )
        {
            mEncoded.count = mInternalCopy.count();
            mEncoded.strings = new char *[mEncoded.count];

            for( int i = 0; i < mInternalCopy.count(); i++ )
            {
                strcpy( mEncoded.strings[i], GW_EncodeQString( mInternalCopy[i] ).data() );
            }
        }

        StrArrayRef::~StrArrayRef()
        {
            delete[] mEncoded.strings;
        }

        FileInfo mkFileInfo(const git_diff_file* df)
        {
            if (!df) {
                return FileInfo();
            }

            QString path;
            if (df->path) {
                path = GW_StringToQt(df->path);
            }
            return FileInfo(path, ObjectId::fromRaw(df->id.id), df->size, FileModes(df->mode),
                            false, (df->flags & GIT_DIFF_FLAG_VALID_ID) != 0);
        }

    }

    GitWrap::GitWrap()
    {
        git_threads_init();

        Q_ASSERT( Internal::GitWrapPrivate::self == NULL );
        Internal::GitWrapPrivate::self = new Internal::GitWrapPrivate;
    }

    GitWrap::~GitWrap()
    {
        Q_ASSERT( Internal::GitWrapPrivate::self != NULL );
        delete Internal::GitWrapPrivate::self;
        Internal::GitWrapPrivate::self = NULL;

        git_threads_shutdown();
    }

    Result& GitWrap::lastResult()
    {
        Q_ASSERT( Internal::GitWrapPrivate::self != NULL );

        if( !Internal::GitWrapPrivate::self->mTLStore.hasLocalData() )
        {
            Internal::GitWrapTLS* tls = new Internal::GitWrapTLS;
            Internal::GitWrapPrivate::self->mTLStore.setLocalData( tls );
        }

        Internal::GitWrapTLS* tls = Internal::GitWrapPrivate::self->mTLStore.localData();
        return tls->mLastResult;
    }

}
