/*
 * libGitWrap - A Qt wrapper library for libgit2
 * Copyright (C) 2012-2013 The MacGitver-Developers <dev@macgitver.org>
 *
 * (C) Sascha Cunz <sascha@macgitver.org>
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

#include "Index.hpp"

#include "IndexPrivate.hpp"
#include "IndexEntry.hpp"
#include "IndexEntryPrivate.hpp"
#include "Repository.hpp"
#include "RepositoryPrivate.hpp"

namespace Git
{

    namespace Internal
    {

        IndexPrivate::IndexPrivate( const GitPtr< RepositoryPrivate >& repo, git_index* index )
            : RepoObject( repo )
            , mIndex( index )
        {
            Q_ASSERT( index );
        }

        IndexPrivate::~IndexPrivate()
        {
            Q_ASSERT( mRepo );
            if( mRepo->mIndex == this )
            {
                mRepo->mIndex = NULL;
            }

            if( mIndex )
            {
                git_index_free( mIndex );
            }
        }

    }

    /**
     * @brief       Constructor
     *
     * @param[in]   create      If `true`, an _in-memory_ Index will be created. An in-memory index
     *                          can be used to perform index operations, but cannot be written to
     *                          disc.
     *                          If `false` (the default), an invalid Index object will be
     *                          constructed, which is equal to a default constructed object.
     *
     */
    Index::Index( bool create )
    {
        if( create )
        {
            git_index* index = NULL;

            // We can't do anything with a potential error; anyway: it can only error out in
            // Out-Of-Memory case.
            git_index_new( &index );

            d = new Internal::IndexPrivate( NULL, index );
        }
    }

    /**
     * @brief           Constructor
     *
     * @param[in]       path    Path to read an index from.
     *
     * @param[in,out]   result  A Result object; see @ref GitWrapErrorHandling
     *
     * Creates a so called _bare_ index. A bare index is loaded from disc (from the @a path file)
     * and can be stored back there. It is _not_ associated with any repository.
     *
     */
    Index::Index(Result& result, const QString& path)
    {
        if( !result )
        {
            // Simply keep ourselves invalid, as we cannot report
            return;
        }

        git_index* index = NULL;

        result = git_index_open( &index, path.toUtf8().constData() );
        if( !result )
        {
            // Simply keep ourselves invalid, as we cannot report
            return;
        }

        d = new Internal::IndexPrivate( NULL, index );
    }

    Index::Index( Internal::IndexPrivate* _d )
        : d( _d )
    {
    }

    Index::Index( const Index& o )
        : d( o.d )
    {
    }

    Index::~Index()
    {
    }

    Index& Index::operator=( const Index& other )
    {
        d = other.d;
        return *this;
    }

    bool Index::isValid() const
    {
        return d;
    }

    int Index::count( Result& result ) const
    {
        if( !result )
        {
            return 0;
        }

        if( !d )
        {
            result.setInvalidObject();
            return 0;
        }
        return (int)git_index_entrycount( d->mIndex );
    }

    Repository Index::repository( Result& result ) const
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

    IndexEntry Index::getEntry(Result &result, int n) const
    {
        if( !result )
        {
            return IndexEntry();
        }

        if( !d )
        {
            result.setInvalidObject();
            return IndexEntry();
        }

        const git_index_entry *entry = git_index_get_byindex(d->mIndex, n);
        if(entry == NULL)
        {
            result.setError(GIT_ENOTFOUND);
        }

        return IndexEntry( new Internal::IndexEntryPrivate( entry ) );
    }

    IndexEntry Index::getEntry(Result &result, const QString &path) const
    {
        if( !result )
        {
            return IndexEntry();
        }

        if( !d )
        {
            result.setInvalidObject();
            return IndexEntry();
        }

        const git_index_entry *entry = git_index_get_bypath(d->mIndex, path.toUtf8().constData(), 0);
        if(entry == NULL)
        {
            result.setError(GIT_ENOTFOUND);
        }

        return IndexEntry( new Internal::IndexEntryPrivate( entry ) );
    }

    /**
     * @brief           Adds a file entry to the index.
     *
     *                  The path must be a relative subpath to the repository.
     *
     * @param[in]       path the file path
     * @param[in,out]   result the error result
     */
    void Index::addEntry(Result &result, const QString &path)
    {
        if ( !result )
            return;

        if ( !d )
        {
            result.setInvalidObject();
            return;
        }

        result = git_index_add_bypath( d->mIndex, path.toUtf8().constData() );
    }

    /**
     * @brief           Removes a file entry from the index.
     *
     *                  The path must be a relative subpath to the repository.
     *
     * @param[in]       path the file path
     * @param[in,out]   result the error result
     */
    void Index::removeEntry(Result &result, const QString &path)
    {
        if ( !result )
            return;

        if ( !d )
        {
            result.setInvalidObject();
            return;
        }

        result = git_index_remove_bypath( d->mIndex, path.toUtf8().constData() );
    }

    /**
     * @brief           Read the index from storage
     *
     * Refills this index object with data obtained from hard disc. Any local modifications to this
     * index object will be lost.
     *
     * @param[in,out]   result  A Result object; see @ref GitWrapErrorHandling
     *
     */
    void Index::read( Result& result )
    {
        if( !result )
        {
            return;
        }

        if( !d )
        {
            result.setInvalidObject();
            return;
        }

        result = git_index_read( d->mIndex );
    }

    /**
     * @brief           Write the index to storage
     *
     * Writes this index object to the hard disc.
     *
     * @param[in,out]   result  A Result object; see @ref GitWrapErrorHandling
     *
     */
    void Index::write( Result& result )
    {
        if( !result )
        {
            return;
        }

        if( !d )
        {
            result.setInvalidObject();
            return;
        }

        result = git_index_write( d->mIndex );
    }

}
