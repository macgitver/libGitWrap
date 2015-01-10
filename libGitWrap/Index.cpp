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

#include "libGitWrap/Index.hpp"

#include "libGitWrap/IndexEntry.hpp"
#include "libGitWrap/IndexConflict.hpp"
#include "libGitWrap/IndexConflicts.hpp"
#include "libGitWrap/Reference.hpp"
#include "libGitWrap/Repository.hpp"
#include "libGitWrap/Tree.hpp"

#include "libGitWrap/Private/IndexPrivate.hpp"
#include "libGitWrap/Private/IndexEntryPrivate.hpp"
#include "libGitWrap/Private/IndexConflictPrivate.hpp"
#include "libGitWrap/Private/RepositoryPrivate.hpp"
#include "libGitWrap/Private/ObjectPrivate.hpp"
#include "libGitWrap/Private/TreePrivate.hpp"

#include "libGitWrap/Operations/CommitOperation.hpp"


namespace Git
{

    namespace Internal
    {

        IndexPrivate::IndexPrivate(const RepositoryPrivate::Ptr& repo, git_index* index)
            : RepoObjectPrivate(repo)
            , index(index)
            , conflictsLoaded(false)
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

            if( index )
            {
                git_index_free( index );
            }
        }

        void IndexPrivate::ensureConflictsLoaded()
        {
            if (conflictsLoaded) {
                return;
            }
        }

        void IndexPrivate::clearKnownConflicts()
        {
            conflictsLoaded = false;
            conflicts.clear();
        }

    }

    /**
     * @class       Index
     * @ingroup     GitWrap
     * @brief       Provides access to the git index.
     *
     * An index is a flat list of filenames and their stati. It represents a state of a repository.
     * The usual workflow is to change some files in your working directory, then _stage_ them into
     * the index. Finally, a commit is created out of the index and glued to the existing commits.
     *
     * The index is also the place where the result of a merge is kept. Each filename in the index
     * can be present several times (in different _stages_). If everything's alright, each filename
     * occurs only once in the index. If a merge resulted in conflicts, then the index can be used
     * to determine where the conflict came from. See Git::Index::Stages.
     *
     * Usually, every repository has one index. But you can create arbitrary indices, which helps
     * in creating complex commits. One can use so called _in-memory_ indices for this or create
     * them on disc.
     *
     * Following the naming schema of repostiories, an index that is not attached to a repository
     * (and thus not to a object database), is called a _bare_ index. Bare indices can live purely
     * in memory or on disc. They aid in creating new tree and commit objects.
     * Conceptually, some operations cannot be executed with a bare index.
     *
     * To obtain a repository's index call Repository::index(). To create an in-memory bare index,
     * call Index(true). To create a bare index from a file on disc, call Index(r, path) with
     * @a path being either a path to an existing index on disc or a path where the index will be
     * created.
     *
     * A easy way to create a new commit is to create a new index, populate it with the root tree of
     * the current head commit, modify it as you please and write it back to the repository. Then
     * finally use @a newRoot as tree to create the commit object.
     *
     * @code
     * Index i;
     * i.readTree(result, rootTree);
     * // modify i...
     * Tree newRoot = i.writeTreeTo(result, repository);
     * @endcode
     *
     */

    GW_PRIVATE_IMPL(Index, RepoObject)

    /**
     * @brief       Create an in-memory index
     *
     * @return      The created _in-memory_ Index.
     *
     * An in-memory index can be used to perform index operations, but cannot be written to disc.
     *
     */
    Index Index::createInMemory()
    {
        git_index* index = NULL;

        // We can't do anything with a potential error; anyway: it can only error out in
        // Out-Of-Memory case.
        git_index_new(&index);

        return PrivatePtr(new Private(Repository::PrivatePtr(), index));
    }

    /**
     * @brief           Open an existing on disc index
     *
     * @param[in,out]   result  A Result object; see @ref GitWrapErrorHandling
     *
     * @param[in]       path    Path to read an index from.
     *
     * @return          The loaded index or an invalid Index object if anything went wrong.
     *
     * Creates a so called _bare_ index. A bare index is loaded from disc (from the @a path file)
     * and can be stored back there. It is _not_ associated with any repository.
     *
     */
    Index Index::openPath(Result& result, const QString& path)
    {
        if (!result) {
            return Index();
        }

        git_index* index = NULL;
        result = git_index_open( &index, GW_StringFromQt(path) );

        if (!result) {
            return Index();
        }

        return PrivatePtr(new Private(Repository::PrivatePtr(), index));
    }

    Index::operator TreeProviderPtr() const
    {
        return TreeProviderPtr( new IndexTreeProvider( *this ) );
    }

    /**
     * @brief       Query whether this is a bare index
     *
     * @return      @c true if this index is bare (meaning it has no repository associated to it) or
     *              @c false if this index is non-bare and has a repository associated to it.
     */
    bool Index::isBare() const
    {
        GW_CD(Index);
        return d->repo().data() == NULL;
    }

    /**
     * @brief       Get the number of entries in this index
     *
     * @param[in,out]   result  A Result object; see @ref GitWrapErrorHandling
     *
     * @return      The number of entries contained in this index or 0 (zero) if this is not a valid
     *              Index object.
     *
     * Each stage of a path is counted seperately.
     *
     */
    int Index::count( Result& result ) const
    {
        GW_CD_CHECKED(Index, 0, result)
        return (int)git_index_entrycount( d->index );
    }

    /**
     * @brief       Get an entry of this index
     *
     * @param[in,out]   result  A Result object; see @ref GitWrapErrorHandling
     *
     * @param[in]       n       A zero based index into the list of entries of this Index object.
     *
     * @return          The index at position @a n or an invalid IndexEntry object if no such entry
     *                  could be found.
     *
     * In any case: The returned IndexEntry is a short lived value object. It is a copy of the real
     * data. You can modify it and use it to a call to updateEntry(). But you should not keep it
     * around.
     *
     */
    IndexEntry Index::getEntry(Result &result, int n) const
    {
        GW_CD_CHECKED(Index, IndexEntry(), result)
        const git_index_entry *entry = git_index_get_byindex(d->index, n);
        if(entry == NULL)
        {
            result.setError(GIT_ENOTFOUND);
        }

        return IndexEntry::PrivatePtr(new IndexEntry::Private(entry));
    }

    /**
     * @brief       Get an entry of this index given its path and stage
     *
     * @param[in,out]   result  A Result object; see @ref GitWrapErrorHandling
     *
     * @param[in]       path    The path to look for.
     *
     * @param[in]       stage   The stage to look for. Defaults to StageDefault, which means to get
     *                          the entry without any conflicts.
     *
     * @return      The found entry or an invalid IndexEntry object if no file with that name and
     *              stage does exist in this index.
     *
     * In any case: The returned IndexEntry is a short lived value object. It is a copy of the real
     * data. You can modify it and use it to a call to updateEntry(). But you should not keep it
     * around.
     *
     */
    IndexEntry Index::getEntry(Result &result, const QString &path, Stages stage) const
    {
        GW_CD_CHECKED(Index, IndexEntry(), result)
        const git_index_entry *entry = git_index_get_bypath(d->index, GW_StringFromQt(path),
                                                            int(stage));
        if(entry == NULL)
        {
            result.setError(GIT_ENOTFOUND);
        }

        return IndexEntry::PrivatePtr(new IndexEntry::Private(entry));
    }

    /**
     * @brief       Index-Operator (count based)
     *
     * @param       index   Zero-based index of the IndexEntry to get.
     *
     * @return      The desired IndexEntry or an invalid one if any error occured.
     *
     * Caution: This method just calls getEntry() and ignores any errors. If you care for
     * possible errors, call getEntry() instead.
     *
     * In any case: The returned IndexEntry is a short lived value object. It is a copy of the real
     * data. You can modify it and use it to a call to updateEntry(). But you should not keep it
     * around.
     *
     */
    IndexEntry Index::operator [](int index) const
    {
        Result r;
        return getEntry(r, index);
    }

    /**
     * @brief       Index-Operator (path based)
     *
     * @param       path    Path name of the entry to get.
     *
     * @return      The desired IndexEntry or an invalid one if any error occured.
     *
     * If an IndexEntry is returned, it comes from StageDefault. All other stages are ignored. If
     * you need to get any other stage's entries, use getEntry() instead and supply the stage you
     * care for.
     *
     * Caution: This method just calls getEntry() and ignores any errors. If you care for
     * possible errors, call getEntry() instead.
     *
     * In any case: The returned IndexEntry is a short lived value object. It is a copy of the real
     * data. You can modify it and use it to a call to updateEntry(). But you should not keep it
     * around.
     *
     */
    IndexEntry Index::operator [](const QString& path) const
    {
        Result r;
        return getEntry(r, path);
    }

    /**
     * @brief       Update an index entry
     *
     *
     * @param[in,out]   result  A Result object; see @ref GitWrapErrorHandling
     *
     * @param[in]       entry   The entry to change
     *
     * If an entry with the @a entry's filename is not existing, it will be added.
     *
     */
    void Index::updateEntry(Result &result, const IndexEntry& entry)
    {
        GW_D_CHECKED_VOID(Index, result)

        if (!entry.isValid()) {
            result.setInvalidObject();
            return;
        }

        IndexEntry::Private* ip = Private::dataOf<IndexEntry>(entry);
        result = git_index_add(d->index, &ip->mEntry);
    }

    /**
     * @brief           Adds a file entry to the index.
     *
     * The path must be a relative subpath to the repository.
     *
     * @param[in,out]   result the error result
     *
     * @param[in]       path the file path to add.
     *
     * An object in the object database with the file's current content is created. This method thus
     * requires a non-bare index.
     *
     */
    void Index::addFile(Result &result, const QString &path)
    {
        GW_CD_CHECKED_VOID(Index, result)
        result = git_index_add_bypath( d->index, GW_StringFromQt(path) );
    }

    /**
     * @brief           Removes a file entry from the index.
     *
     * The path must be a relative subpath to the repository.
     *
     * @param[in,out]   result the error result
     *
     * @param[in]       path the file path
     */
    void Index::removeFile(Result &result, const QString &path)
    {
        GW_D_CHECKED_VOID(Index, result)
        result = git_index_remove_bypath( d->index, GW_StringFromQt(path) );
    }

    /**
     * @brief           Resets an index entry to HEAD without changing the working tree
     *
     * This behaviour is also known as "unstaging". It resets the state of a file without changing
     * any contents.
     *
     * The corresponding git command is:
     *
     *      git reset --mixed <paths>
     *
     * @param[in,out]   result  a Result object; see @ref GitWrapErrorHandling
     *
     * @param[in]       paths   the file paths relative to the repository's working directory
     */
    void Index::resetFiles(Result &result, const QStringList &paths)
    {
        GW_D_CHECKED_VOID(Index, result)

        if (paths.isEmpty()) {
            return;
        }

        if (isBare()) {
            result.setInvalidObject();
            return;
        }

        git_reference *ref = NULL;
        result = git_repository_head( &ref, d->repo()->mRepo );
        GW_CHECK_RESULT( result, void() );

        git_object *o = NULL;
        result = git_reference_peel( &o, ref, GIT_OBJ_COMMIT );
        git_reference_free( ref );
        GW_CHECK_RESULT( result, void() );

        result = git_reset_default( d->repo()->mRepo, o, Internal::StrArray( paths ) );
        GW_CHECK_RESULT( result, void() );

        d->clearKnownConflicts();   // conflicts need to be reloaded as conflicts related to
                                    // the paths (which we did reset) are removed now.
    }

    /**
     * @brief           Overwrites the file content with the content from the index.
     *
     * @param[in,out]   result  A Result object; see @ref GitWrapErrorHandling
     *
     * @param[in]       paths   A list of globs or pathnames to checkout.
     *
     * Checkout the files to the index. File changes are discarded only in the working directory.
     * Changes in the index stay untouched. This allows staging part of a file and discard the rest.
     *
     */
    void Index::checkoutFiles(Result &result, const QStringList &paths)
    {
        GW_D_CHECKED_VOID(Index, result)
        Internal::CheckoutOptions options( paths );
        (*options).checkout_strategy = GIT_CHECKOUT_FORCE;

        result = git_checkout_index(d->repo()->mRepo, d->index, options);
    }

    /**
     * @brief           Read the index from storage
     *
     * Refills this index object with data obtained from hard disc. Any local modifications to this
     * index object will be lost.
     *
     * @param[in,out]   result  A Result object; see @ref GitWrapErrorHandling
     *
     * @param[in]       force   If `true` (the default) always reread the index file. Otherwise just
     *                          read it if it has changed since the last time being read.
     *
     */
    void Index::read(Result& result, bool force)
    {
        GW_D_CHECKED_VOID(Index, result)

        result = git_index_read(d->index, force ? 1 : 0);

        if (result) {
            d->clearKnownConflicts();
        }
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
        GW_D_CHECKED_VOID(Index, result)
        result = git_index_write( d->index );
    }

    /**
     * @brief       Remove all entries from this index
     */
    void Index::clear()
    {
        GW_D(Index);
        if (d) {
            git_index_clear(d->index);
            d->clearKnownConflicts();
        }
    }

    /**
     * @brief       Read a tree into this index
     *
     *
     * @param[in,out]   result  A Result object; see @ref GitWrapErrorHandling
     *
     * @param[in]       tree    The tree to read.
     *
     * The index is cleared and recursively populated with all tree entries contained in @a tree.
     */
    void Index::readTree(Result& result, Tree& tree)
    {
        GW_D_CHECKED_VOID(Index, result);

        if (!tree.isValid()) {
            result.setInvalidObject();
            return;
        }

        Tree::Private* tp = Private::dataOf<Tree>(tree);
        result = git_index_read_tree(d->index, tp->o());

        if (result) {
            d->clearKnownConflicts();
        }
    }

    /**
     * @brief       Create tree objects from this index
     *
     * @param[in,out]   result  A Result object; see @ref GitWrapErrorHandling
     *
     * @return      The root tree as Tree on success; an invalid Tree in case of any failure.
     *
     * The Index object must not be bare (isBare() must return @c false) and be associated to a
     * repository. It must also not contain any conflict entries (hasConflicts() must return
     * @c false).
     *
     * If there is no associated repository (i.e. if this is a bare index), use writeTreeTo()
     * instead to specify a Repository object.
     *
     * The index is split up into individual trees which are put into the associated repository as
     * tree objects.
     *
     */
    Tree Index::writeTree(Result& result)
    {
        GW_D_CHECKED(Index, Tree(), result)

        git_oid treeGitOid;
        result = git_index_write_tree(&treeGitOid, d->index);
        if (!result) {
            return Tree();
        }

        return repository().lookupTree(result, ObjectId::fromRaw(treeGitOid.id));
    }

    /**
     * @brief       Create tree objects from this index
     *
     * @param[in,out]   result  A Result object; see @ref GitWrapErrorHandling
     *
     * @param[in]       repo    The repository to store objects into
     *
     * @return      The root tree as Tree on success; an invalid Tree in case of any failure.
     *
     * The Index must not contain any conflict entries (hasConflicts() must return @c false).
     *
     * The index is split up into individual trees which are put into the given repository @a repo
     * as tree objects.
     *
     */
    Tree Index::writeTreeTo(Result& result, Repository& repo)
    {
        GW_D_CHECKED(Index, Tree(), result)

        if (!repo.isValid()) {
            result.setInvalidObject();
            return Tree();
        }

        git_oid treeGitOid;
        Repository::Private* rp = Private::dataOf<Repository>(repo);
        result = git_index_write_tree_to(&treeGitOid, d->index, rp->mRepo);
        if (!result) {
            return Tree();
        }

        return repo.lookupTree(result, ObjectId::fromRaw(treeGitOid.id));
    }

    /**
     * @brief       Get this Index object's conflicts
     *
     * @return      A IndexConflicts object that is associated with this index.
     *
     */
    IndexConflicts Index::conflicts() const
    {
        GW_CD_EX(Index);
        return d;
    }

    /**
     * @brief       Does this Index have any conflict entries?
     *
     * @return      @c true if this is a valid Index object and has conflict entries. @c false if
     *              not.
     *
     */
    bool Index::hasConflicts() const
    {
        GW_CD(Index);
        return d && git_index_has_conflicts(d->index);
    }


    // *** IndexTreeProvider ***


    IndexTreeProvider::IndexTreeProvider(const Index& index)
        : mIndex( index )
    {
    }

    Tree IndexTreeProvider::tree(Result& result)
    {
        return mIndex.writeTree(result);
    }

    Repository IndexTreeProvider::repository() const
    {
        return mIndex.repository();
    }

}
