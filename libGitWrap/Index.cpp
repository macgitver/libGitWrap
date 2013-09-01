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

    IndexEntry Index::getEntry(Result &result, const QString &path, Stages stage) const
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

        const git_index_entry *entry = git_index_get_bypath(d->mIndex, path.toUtf8().constData(),
                                                            int(stage));
        if(entry == NULL)
        {
            result.setError(GIT_ENOTFOUND);
        }

        return IndexEntry( new Internal::IndexEntryPrivate( entry ) );
    }

    void Index::updateEntry(Result &result, const IndexEntry& entry)
    {
        if (!result) {
            return;
        }

        if (!d || !entry.d) {
            result.setInvalidObject();
            return;
        }

        result = git_index_add(d->mIndex, &entry.d->mEntry);
    }

    /**
     * @brief           Adds a file entry to the index.
     *
     *                  The path must be a relative subpath to the repository.
     *
     * @param[in]       path the file path
     * @param[in,out]   result the error result
     */
    void Index::addFile(Result &result, const QString &path)
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
    void Index::removeFile(Result &result, const QString &path)
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
     * @brief           Resets an index entry to HEAD without changing the working tree
     *
     *                  This behaviour is also known as "unstaging".
     *                  It resets the state of a file without changing any contents.
     *
     * @param[in]       paths   the file paths relative to the repository's working directory
     * @param[in,out]   result  a Result object; see @ref GitWrapErrorHandling
     */
    void Index::resetFiles(Result &result, const QStringList &paths)
    {
        if ( !result || paths.isEmpty() )
            return;

        if ( !d )
        {
            result.setInvalidObject();
            return;
        }

        git_reference *ref = NULL;
        result = git_repository_head( &ref, d->repo()->mRepo );
        if ( !result )
            return;

        git_object *o = NULL;
        result = git_reference_peel( &o, ref, GIT_OBJ_COMMIT );
        git_reference_free( ref );
        if ( !result )
            return;

        result = git_reset_default( d->repo()->mRepo, o, Internal::StrArrayWrapper( paths ) );
    }

    /**
     * @brief           Overwrites the file content with the content from the index.
     *
     * @param[in]       paths
     * @param[in,out]   result  A Result object; see @ref GitWrapErrorHandling
     *
     *                  Checkout the files to the index. File changes are discarded only in the
     *                  working directory. Changes in the index stay untouched.
     *                  This allows staging part of a file and discard the rest.
     */
    void Index::checkoutFiles(Result &result, const QStringList &paths)
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

        git_checkout_opts options = GIT_CHECKOUT_OPTS_INIT;
        options.checkout_strategy = GIT_CHECKOUT_FORCE;

        // TODO don't copy, just map paths here
        result = git_strarray_copy( &options.paths, Internal::StrArrayWrapper( paths ) );
        if ( !result )
            return;

        result = git_checkout_index( d->repo()->mRepo, d->mIndex, &options );

        git_strarray_free(&options.paths);
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

    void Index::clear()
    {
        if (d) {
            git_index_clear(d->mIndex);
        }
    }

}
