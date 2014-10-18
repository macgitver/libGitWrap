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

        Buffer::Buffer(QTextCodec *codec)
            : mCodec(codec)
        {
            memset(&buf, 0, sizeof(buf));
        }

        Buffer::~Buffer()
        {
            git_buf_free( &buf );
        }

        Buffer::operator git_buf*()
        {
            return &buf;
        }

        Buffer::operator const char*()
        {
            return buf.ptr;
        }

        /**
         * @brief        Converts the contents of the buffer into a QString.
         *
         * @return       the buffer in readable text format
         *
         * @note         Defaults to QString::fromUtf8() when no text codec is set.
         */
        QString Buffer::toString() const
        {
            return GW_StringToQt( buf.ptr, buf.size );
        }


        //-- StrArray --------------------------------------------------------------------------- >8

        StrArray::StrArray()
        {
            Q_ASSERT(false);
        }

        StrArray::StrArray(const QStringList& sl)
        {
            a.count = sl.count();
            a.strings = new char *[a.count];

            for( int i = 0; i < sl.count(); i++ )
            {
                mEncoded << GW_EncodeQString( sl[i] );
                a.strings[i] = mEncoded[i].data();
            }
        }

        StrArray::~StrArray()
        {
            delete[] a.strings;
        }

        StrArray::operator git_strarray*()
        {
            return &a;
        }

        StrArray::operator const git_strarray *() const
        {
            return &a;
        }

        //-- StrArrayRef ------------------------------------------------------------------------ >8

        StrArrayRef& StrArrayRef::operator=(const StrArrayRef&)
        {
            Q_ASSERT(false);
            return *this;
        }

        StrArrayRef::StrArrayRef(git_strarray& _a, const QStringList& sl)
            : a(_a)
        {
            a.count = sl.count();
            a.strings = new char *[a.count];

            for(int i=0; i < sl.count(); ++i )
            {
                mEncoded << GW_EncodeQString( sl[i] );
                a.strings[i] = mEncoded[i].data();
            }
        }

        StrArrayRef::~StrArrayRef()
        {
            delete[] a.strings;
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
