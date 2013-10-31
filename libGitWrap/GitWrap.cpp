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
                sl << QString::fromUtf8( arry->strings[ i ] );
            }

            git_strarray_free( arry );
            return sl;
        }

        //-- StrArrayWrapper -------------------------------------------------------------------- >8

        StrArrayWrapper::StrArrayWrapper()
        {
            Q_ASSERT(false);
        }

        StrArrayWrapper::StrArrayWrapper( const StrArrayWrapper & )
        {
            Q_ASSERT(false);
        }

        StrArrayWrapper& StrArrayWrapper::operator=(const StrArrayWrapper&)
        {
            Q_ASSERT(false);
            return *this;
        }

        StrArrayWrapper::StrArrayWrapper(const QStringList& sl)
        {
            internalCopy = sl;

            a.count = internalCopy.count();
            a.strings = new char *[a.count];

            for(int i=0; i < internalCopy.count(); ++i )
            {
                a.strings[i] = internalCopy[i].toUtf8().data();
            }
        }

        StrArrayWrapper::~StrArrayWrapper()
        {
            delete[] a.strings;
        }

        StrArrayWrapper::operator git_strarray*()
        {
            return &a;
        }

        StrArrayWrapper::operator const git_strarray *() const
        {
            return &a;
        }

        //-- StrArray --------------------------------------------------------------------------- >8

        StrArray& StrArray::operator=(const StrArray&)
        {
            Q_ASSERT(false);
            return *this;
        }

        StrArray::StrArray(git_strarray& _a, const QStringList& sl)
            : a(_a)
            , internalCopy(sl)
        {
            a.count = internalCopy.count();
            a.strings = new char *[a.count];

            for(int i=0; i < internalCopy.count(); ++i )
            {
                a.strings[i] = internalCopy[i].toUtf8().data();
            }
        }

        StrArray::~StrArray()
        {
            delete[] a.strings;
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
