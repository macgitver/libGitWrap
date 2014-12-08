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


        //-- CheckoutOptionsRef ----------------------------------------------------------------- >8

        /**
         * @brief           CheckoutRef
         *
         *                  By default, the referenced instance is not initialized.
         *                  If you need to initialize it, set the init parameter to true.
         *
         * @param[in,out]   ref the object reference
         * @param[in]       init initialize the referenced object
         */
        CheckoutOptionsRef::CheckoutOptionsRef( git_checkout_options& ref, bool init )
            : mOptionsRef(ref)
        {
            if ( init )
            {
                this->init( );
            }

            mPaths = QSharedPointer<StrArrayRef>( new StrArrayRef( mOptionsRef.paths ) );

        }
        CheckoutOptionsRef::CheckoutOptionsRef(git_checkout_options& ref, const QStringList& paths, bool init)
            : mOptionsRef(ref)
        {
            if ( init )
            {
                this->init();
            }

            mPaths = QSharedPointer<StrArrayRef>( new StrArrayRef( mOptionsRef.paths ) );
            mPaths->setStrings( paths );
            Q_ASSERT( mPaths == mOptionsRef.paths );
        }

        void CheckoutOptionsRef::init()
        {
            const git_checkout_options *assert_coo_ptr = &mOptionsRef;
            Q_UNUSED( assert_coo_ptr )

            git_checkout_init_options( &mOptionsRef, GIT_CHECKOUT_OPTIONS_VERSION );
            Q_ASSERT( assert_coo_ptr == &mOptionsRef );
        }

        CheckoutOptionsRef::operator git_checkout_options*()
        {
            return &mOptionsRef;
        }

        CheckoutOptionsRef::operator const git_checkout_options*() const
        {
            return &mOptionsRef;
        }

        CheckoutOptionsRef::operator git_checkout_options&()
        {
            return mOptionsRef;
        }

        git_checkout_options& CheckoutOptionsRef::operator *()
        {
            return mOptionsRef;
        }

        QStringList CheckoutOptionsRef::paths() const
        {
            return *mPaths;
        }

        void CheckoutOptionsRef::setPaths(const QStringList& paths)
        {
            mPaths->setStrings( paths );
        }


        //-- CheckoutOptions -------------------------------------------------------------------- >8

        CheckoutOptions::CheckoutOptions()
            : CheckoutOptionsRef( mOptionsRef, true )
        {
        }

        CheckoutOptions::CheckoutOptions(const QStringList& paths)
            : CheckoutOptionsRef( mOptionsRef, paths, true )
        {
        }


        //-- CloneOptions ----------------------------------------------------------------------- >8

        CloneOptions::CloneOptions()
        {
            git_clone_init_options( &mOptions, GIT_CLONE_OPTIONS_VERSION );
        }

        CloneOptions::operator const git_clone_options*() const
        {
            return &mOptions;
        }

        CloneOptions::operator git_clone_options&()
        {
            return mOptions;
        }

        git_clone_options& CloneOptions::operator *()
        {
            return mOptions;
        }

        CheckoutOptionsRef CloneOptions::checkoutOptions()
        {
            return mOptions.checkout_opts;
        }


        //-- StrArray --------------------------------------------------------------------------- >8

        StrArray::StrArray()
        {
            mEncoded.count = 0;
            mEncoded.strings = 0;
        }

        StrArray::StrArray(const QStringList &strings)
        {
            setStrings(strings);
        }

        StrArray::~StrArray()
        {
            delete[] mEncoded.strings;
        }

        StrArray::operator git_strarray*()
        {
            return &mEncoded;
        }

        StrArray::operator const git_strarray *() const
        {
            return &mEncoded;
        }

        StrArray::operator QStringList() const
        {
            return strings();
        }

        QStringList StrArray::strings() const
        {
            QStringList result;
            for (int i=0; i < mEncoded.count; i++)
            {
                result << GW_StringToQt( mEncoded.strings[i] );
            }

            return result;
        }

        void StrArray::setStrings(const QStringList &strings)
        {
            delete[] mEncoded.strings;

            mEncoded.count = strings.count();
            mEncoded.strings = new char *[mEncoded.count];

            for( int i = 0; i < strings.count(); i++ )
            {
                strcpy( mEncoded.strings[i], GW_EncodeQString( strings[i] ).data() );
            }
        }


        //-- StrArrayRef ------------------------------------------------------------------------ >8

        StrArrayRef::StrArrayRef(git_strarray& _a, bool init)
            : mEncoded( _a )
        {
            if ( init ) {
                mEncoded.strings = 0;
                mEncoded.count = 0;
            }
        }

        StrArrayRef::StrArrayRef(git_strarray& _a, const QStringList& strings)
            : mEncoded(_a)
        {
            Q_ASSERT( !mEncoded.strings && !mEncoded.count );
            setStrings( strings );
        }

        StrArrayRef::~StrArrayRef()
        {
            delete[] mEncoded.strings;
        }

        bool StrArrayRef::operator ==(const git_strarray& other) const
        {
            return &mEncoded == &other;
        }

        bool StrArrayRef::operator !=(const git_strarray& other) const
        {
            return !(*this == other);
        }

        bool StrArrayRef::operator ==(const git_strarray* other) const
        {
            return &mEncoded == other;
        }

        bool StrArrayRef::operator !=(const git_strarray* other) const
        {
            return !(*this == other);
        }

        StrArrayRef::operator QStringList() const
        {
            return strings();
        }

        QStringList StrArrayRef::strings() const
        {
            QStringList result;
            for (int i=0; i < mEncoded.count; i++)
            {
                result << GW_StringToQt( mEncoded.strings[i] );
            }

            return result;
        }

        void StrArrayRef::setStrings(const QStringList& strings)
        {
            delete[] mEncoded.strings;

            mEncoded.count = strings.count();
            if ( strings.isEmpty() )
            {
                // The strings pointer must be NULL in this case.
                mEncoded.strings = NULL;
                return;
            }

            mEncoded.strings = new char *[mEncoded.count];

            for( int i = 0; i < strings.count(); i++ )
            {
                strcpy( mEncoded.strings[i], GW_EncodeQString( strings[i] ).data() );
            }
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
