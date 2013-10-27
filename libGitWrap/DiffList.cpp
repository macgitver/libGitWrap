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

#include "ChangeListConsumer.hpp"
#include "PatchConsumer.hpp"
#include "DiffList.hpp"
#include "Repository.hpp"

#include "Private/GitWrapPrivate.hpp"
#include "Private/DiffListPrivate.hpp"
#include "Private/RepositoryPrivate.hpp"

namespace Git
{

    namespace Internal
    {

        class DiffListConsumer : public ChangeListConsumer
        {
        public:
            bool startFileChange(const Git::ChangeListEntry &entry);

            const Git::ChangeList &changeList() const;

        private:
            Git::ChangeList      mChanges;
        };

        bool DiffListConsumer::startFileChange(const Git::ChangeListEntry &entry)
        {
            mChanges.append( entry );

            return true;
        }

        const ChangeList &DiffListConsumer::changeList() const
        {
            return mChanges;
        }

        DiffListPrivate::DiffListPrivate( const GitPtr< RepositoryPrivate >& repo,
                                          git_diff_list* difflist )
            : RepoObject( repo )
            , mDiffList( difflist )
        {
            Q_ASSERT( difflist );
        }

        DiffListPrivate::~DiffListPrivate()
        {
            Q_ASSERT( mRepo );

            if( mDiffList )
            {
                git_diff_list_free( mDiffList );
            }
        }

        static int patchFileCallBack( const git_diff_delta* delta, float, void* cb_data )
        {
            PatchConsumer* pc = (PatchConsumer*) cb_data;

            ChangeListEntry entry =
            {
                QString::fromUtf8( delta->old_file.path )
                , QString::fromUtf8( delta->new_file.path )
                , PatchConsumer::Type( delta->status )
                , delta->similarity
                , ( delta->flags & GIT_DIFF_FLAG_BINARY ) != 0
            };

            if( pc->startFileChange( entry ) )
            {
                return GIT_OK;
            }

            return GIT_ERROR;
        }

        static int patchHunkCallBack( const git_diff_delta* delta,
                                      const git_diff_range* range,
                                      const char* header, size_t header_len,
                                      void* cb_data )
        {
            PatchConsumer* pc = (PatchConsumer*) cb_data;

            if( pc->startHunkChange( range->new_start, range->new_lines,
                                     range->old_start, range->old_lines,
                                     header ? QString::fromUtf8( header, int( header_len ) ) : QString() ) )
            {
                return GIT_OK;
            }

            return GIT_ERROR;
        }

        static int patchDataCallBack( const git_diff_delta* delta,
                                      const git_diff_range* range,
                                      char line_origin,
                                      const char *content,
                                      size_t content_len,
                                      void* cb_data )
        {
            PatchConsumer* pc = (PatchConsumer*) cb_data;

            QString ct;
            if( content && content_len )
            {
                if( content[ content_len - 1 ] == '\n' )
                    --content_len;

                ct = QString::fromUtf8( content, int( content_len ) );
            }

            switch( line_origin )
            {
            case GIT_DIFF_LINE_CONTEXT:
                if( !pc->appendContext( ct ) )
                {
                    return GIT_ERROR;
                }
                break;

            case GIT_DIFF_LINE_ADDITION:
                if( !pc->appendAddition( ct ) )
                {
                    return GIT_ERROR;
                }
                break;

            case GIT_DIFF_LINE_DELETION:
                if( !pc->appendDeletion( ct ) )
                {
                    return GIT_ERROR;
                }
                break;

            default:
                qDebug( "Foo: t=%i", int(line_origin) );

            }

            return GIT_OK;
        }

        static int changeListCallBack( const git_diff_delta* delta, float, void* cb_data )
        {
            ChangeListConsumer* consumer = (ChangeListConsumer*) cb_data;

            ChangeListEntry change =
            {
                QString::fromUtf8( delta->old_file.path )
                , QString::fromUtf8( delta->new_file.path )
                , ChangeListConsumer::Type( delta->status )
                , delta->similarity
                , ( delta->flags & GIT_DIFF_FLAG_BINARY ) != 0
            };

            if( consumer->startFileChange( change ) )
            {
                return GIT_OK;
            }

            return GIT_ERROR;
        }

    }

    DiffList::DiffList()
    {
    }

    DiffList::DiffList( Internal::DiffListPrivate* _d )
        : d( _d )
    {
    }

    DiffList::DiffList( const DiffList& o )
        : d( o.d )
    {
    }

    DiffList::~DiffList()
    {
    }

    DiffList& DiffList::operator=( const DiffList& other )
    {
        d = other.d;
        return *this;
    }

    bool DiffList::isValid() const
    {
        return d;
    }

    Repository DiffList::repository( Result& result ) const
    {
        if( !result )
        {
            return Repository();
        }

        if( !d )
        {
            result.setInvalidObject();
            return Repository();
        }

        return Repository( d->repo() );
    }

    bool DiffList::mergeOnto(Result& result, DiffList onto) const
    {
        if( !result )
        {
            return false;
        }

        if( !d || !onto.isValid() )
        {
            result.setInvalidObject();
            return false;
        }

        result = git_diff_merge( onto.d->mDiffList, d->mDiffList );
        return result;
    }

    bool DiffList::consumePatch(Result& result, PatchConsumer* consumer) const
    {
        if( !result )
        {
            return false;
        }

        if( !d )
        {
            result.setInvalidObject();
            return false;
        }

        if( !consumer )
        {
            result.setInvalidObject();
            return false;
        }

        result = git_diff_foreach( d->mDiffList,
                                   &Internal::patchFileCallBack,
                                   &Internal::patchHunkCallBack,
                                   &Internal::patchDataCallBack,
                                   consumer );

        return result;
    }

    bool DiffList::consumeChangeList(Result& result, ChangeListConsumer* consumer) const
    {
        if( !result )
        {
            return false;
        }

        if( !d )
        {
            result.setInvalidObject();
            return false;
        }

        if( !consumer )
        {
            result.setInvalidObject();
            return false;
        }

        result = git_diff_foreach( d->mDiffList,
                                   &Internal::changeListCallBack,
                                   NULL,
                                   NULL,
                                   consumer );

        return result;
    }

    ChangeList DiffList::changeList(Result &result) const
    {
        Internal::DiffListConsumer consumer;
        consumeChangeList(result, &consumer);

        return consumer.changeList();
    }

    /**
     * @brief   Try to find renames
     *
     * @param[in,out] result
     *
     * @return        `true` on success, otherwise `false`.
     */
    bool DiffList::findRenames( Result& result )
    {
        if( !result )
        {
            return false;
        }

        if( !d )
        {
            result.setInvalidObject();
            return false;
        }

        git_diff_find_options opts = GIT_DIFF_FIND_OPTIONS_INIT;
        result = git_diff_find_similar( d->mDiffList, &opts );

        return result;
    }

}
