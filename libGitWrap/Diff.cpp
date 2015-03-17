/*
 * libGitWrap - A Qt wrapper library for libgit2
 * Copyright (C) 2015 The MacGitver-Developers <dev@macgitver.org>
 *
 * (C) Nils Fenner <nils@macgitver.org>
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

#include "Diff.hpp"

#include "libGitWrap/DiffList.hpp"
#include "libGitWrap/Index.hpp"
#include "libGitWrap/Repository.hpp"
#include "libGitWrap/Result.hpp"
#include "libGitWrap/Tree.hpp"

#include "libGitWrap/Private/DiffPrivate.hpp"
#include "libGitWrap/Private/IndexPrivate.hpp"
#include "libGitWrap/Private/TreePrivate.hpp"


namespace Git
{
    /**
     * @ingroup     GitWrap
     *
     * @class       Diff
     *
     * @brief       Diff various GitWrap objects.
     */

    /**
     * @brief           Diff the repository's index to the working directory.
     *
     * @param[in,out]   result  a @ref Result object; see @ref GitWrapErrorHandling
     *
     * @param[in]       repo    the owner repository of index and working directory
     *
     * @param[in]       opts    optional diff options
     *
     * @return
     */
    Diff::Diff(Result& result)
        : mOpts( new Internal::DiffOptions( result ) )
    {
    }

    Diff::~Diff()
    {
        delete mOpts;
    }

    /**
     * @brief           Diff a repository's index and working directory.
     *
     * @param[in,out]   result  a @ref Result object; see @ref GitWrapErrorHandling
     *
     * @param[in]       repo    the repository, that contains the index and
     *                          working directory
     *
     * @return          the differences as a @ref DiffList object
     */
    DiffList Diff::indexToWorkDir(Result& result, const Repository& repo) const
    {
        GW_CHECK_RESULT( result, NULL );
        if ( !repo.isValid() ) {
            result.setInvalidObject();
            return NULL;
        }

        Repository::Private* rp = Internal::BasePrivate::dataOf<Repository>( repo );
        Q_ASSERT( rp );

        git_diff* diff = NULL;
        result = git_diff_index_to_workdir( &diff, rp->mRepo, NULL, *mOpts );

        return new Internal::DiffListPrivate(rp, diff);
    }

    /**
     * @brief           Diff a Git index to a repository's working directory.
     *
     * @param[in,out]   result  a @ref Result object; see @ref GitWrapErrorHandling
     *
     * @param[in]       repo    the repository, that contains the
     *                          working directory for comparison
     *
     * @param[in]       index   a Git index to compare with
     *                          the working directory from @p repo
     *
     * @return          the differences of the repository's index to it's
     *                  working directory as a @ref DiffList
     *
     * By default, the repository's index is loaded from disk before comparison.
     */
    DiffList Diff::indexToWorkDir(Result& result, const Repository& repo, const Index& index) const
    {
        GW_CHECK_RESULT( result, NULL );

        if ( !(repo.isValid() && index.isValid()) ) {
            result.setInvalidObject();
            return NULL;
        }

        Repository::Private* rp = Internal::BasePrivate::dataOf<Repository>( repo );
        Q_ASSERT( rp );
        Index::Private* ip = Internal::BasePrivate::dataOf<Index>( index );
        Q_ASSERT( ip );

        git_diff* diff = NULL;
        result = git_diff_index_to_workdir( &diff, rp->mRepo, ip->index, *mOpts );

        return new Internal::DiffListPrivate(rp, diff);
    }

    /**
     * @brief           Diff two trees within a repository.
     *
     * @param[in,out]   result  a @ref Result object; see @ref GitWrapErrorHandling
     *
     * @param[in]       oldTree the old tree
     *
     * @param[in]       newTree the new tree
     *
     * @param[in]       opts    optional diff options
     *
     * @return          the differences as a @ref DiffList object
     */
    DiffList Diff::treeToTree(Result& result, const Tree& oldTree, const Tree& newTree) const
    {
        GW_CHECK_RESULT( result, NULL );

        const bool otValid = oldTree.isValid();
        Repository::Private* otRepo = otValid
                                      ? Internal::BasePrivate::dataOf<Repository>( oldTree.repository() )
                                      : NULL;

        const bool ntValid = newTree.isValid();
        Repository::Private* ntRepo = ntValid
                                      ? Internal::BasePrivate::dataOf<Repository>( newTree.repository() )
                                      : NULL;

        if ( otRepo && ntRepo && (otRepo != ntRepo) )
        {
            // repo must be the same or NULL
            result.setInvalidObject();
            return NULL;
        }

        Repository::Private* rp = otRepo ? otRepo : ntRepo;
        Q_ASSERT(rp);

        git_tree* gitOldTree = otValid ? Internal::BasePrivate::dataOf<Tree>(oldTree)->o() : NULL;
        git_tree* gitNewTree = ntValid ? Internal::BasePrivate::dataOf<Tree>(newTree)->o() : NULL;

        git_diff* diff = NULL;
        result = git_diff_tree_to_tree( &diff, rp->mRepo, gitOldTree, gitNewTree, *mOpts );
        GW_CHECK_RESULT( result, NULL );

        return new Internal::DiffListPrivate(rp, diff);
    }

    /**
     * @brief           Diff a tree to the repository's index.
     *
     * @param[in,out]   result  a @ref Result object; see @ref GitWrapErrorHandling
     *
     * @param[in]       tree    the tree to diff against the index
     *
     * @param[in]       opts    optional diff options
     *
     * @return          the differences as a @ref DiffList object
     */
    DiffList Diff::treeToIndex(Result& result, const Tree& tree) const
    {
        GW_CHECK_RESULT( result, NULL );
        if ( !tree.isValid() ) {
            result.setInvalidObject();
            return NULL;
        }

        Internal::TreePrivate* tp = Internal::BasePrivate::dataOf<Tree>( tree );
        Q_ASSERT( tp );
        Internal::RepositoryPrivate* rp = tp->repo().data();
        Q_ASSERT( rp );

        git_diff* diff = NULL;
        result = git_diff_tree_to_index( &diff, rp->mRepo, tp->o(), NULL, *mOpts );
        GW_CHECK_RESULT( result, NULL );

        return new Internal::DiffListPrivate(rp, diff);
    }

    /**
     * @brief           Diff an empty tree to the repository's index.
     *
     * @param[in,out]   result  a @ref Result object; see @ref GitWrapErrorHandling
     *
     * @param[in]       repo    the repo contains the index for comparison
     *
     * @return          the repository's index as a @ref DiffList
     */
    DiffList Diff::treeToIndex(Result& result, const Repository& repo) const
    {
        GW_CHECK_RESULT( result, NULL );
        if ( !repo.isValid() ) {
            result.setInvalidObject();
            return NULL;
        }

        Internal::RepositoryPrivate* rp = Internal::BasePrivate::dataOf<Repository>( repo );
        Q_ASSERT( rp );

        git_diff* diff= NULL;
        result = git_diff_tree_to_index( &diff, rp->mRepo, NULL, NULL, *mOpts );
        GW_CHECK_RESULT( result, NULL );

        return new Internal::DiffListPrivate(rp, diff);
    }

    /**
     * @brief           Diff a tree to the repository's working directory.
     *
     * @param[in,out]   result  a @ref Result object; see @ref GitWrapErrorHandling
     *
     * @param[in]       tree    the tree to diff against the working directory
     *
     * @param[in]       opts    optional diff options
     *
     * @return          the differences as a @ref DiffList object
     */
    DiffList Diff::treeToWorkDir(Result& result, const Tree& tree) const
    {
        GW_CHECK_RESULT( result, NULL );
        if ( !tree.isValid() ) {
            result.setInvalidObject();
            return NULL;
        }

        Internal::TreePrivate* tp = Internal::BasePrivate::dataOf<Tree>( tree );
        Q_ASSERT( tp );
        Internal::RepositoryPrivate* rp = tp->repo().data();
        Q_ASSERT( rp );

        git_diff* diff= NULL;
        result = git_diff_tree_to_workdir( &diff,
                                           rp->mRepo,
                                           tp->o(), *mOpts );
        GW_CHECK_RESULT( result, NULL );

        return new Internal::DiffListPrivate(rp, diff);
    }

    /**
     * @brief           Diff an empty tree to the repository's working directory.
     *
     * @param[in,out]   result  a @ref Result object; see @ref GitWrapErrorHandling
     *
     * @return          the repository's working directory as a @ref DiffList
     */
    DiffList Diff::treeToWorkDir(Result& result, const Repository& repo) const
    {
        GW_CHECK_RESULT( result, NULL );
        if ( !repo.isValid() ) {
            result.setInvalidObject();
            return NULL;
        }

        Internal::RepositoryPrivate* rp = Internal::BasePrivate::dataOf<Repository>( repo );
        Q_ASSERT( rp );

        git_diff* diff= NULL;
        result = git_diff_tree_to_workdir( &diff, rp->mRepo, NULL, *mOpts );
        GW_CHECK_RESULT( result, NULL );

        return new Internal::DiffListPrivate(rp, diff);
    }

    /**
     * @brief           Diff a tree to the repository's working directory and index
     *
     * @param[in,out]   result  a @ref Result object; see @ref GitWrapErrorHandling
     *
     * @param[in]       tree    the tree to diff agains index and working directory
     *
     * @param[in]       opts    optional diff options
     *
     * @return          the differences as a @ref DiffList object
     */
    DiffList Diff::treeToWorkDirWithIndex(Result& result, const Tree& tree) const
    {
        GW_CHECK_RESULT( result, NULL );
        if ( !tree.isValid() ) {
            result.setInvalidObject();
            return NULL;
        }

        Internal::TreePrivate* tp = Internal::BasePrivate::dataOf<Tree>( tree );
        Q_ASSERT( tp );
        Internal::RepositoryPrivate* rp = tp->repo().data();
        Q_ASSERT( rp );

        git_diff* diff= NULL;
        result = git_diff_tree_to_workdir_with_index( &diff,
                                                      rp->mRepo,
                                                      tp->o(), *mOpts );
        GW_CHECK_RESULT( result, NULL );

        return new Internal::DiffListPrivate(rp, diff);
    }

    /**
     * @brief           Diff an empty tree to the repository's working directory and index.
     *
     * @param[in,out]   result  a @ref Result object; see @ref GitWrapErrorHandling
     *
     * @param[in]       repo        the repository
     *
     * @return          the differences of the repository's working directory
     *                  and index as a @ref DiffList
     */
    DiffList Diff::treeToWorkDirWithIndex(Result& result, const Repository& repo) const
    {
        GW_CHECK_RESULT( result, NULL );
        if ( !repo.isValid() )
        {
            result.setInvalidObject();
            return NULL;
        }

        Internal::RepositoryPrivate* rp = Internal::BasePrivate::dataOf<Repository>( repo );
        Q_ASSERT( rp );

        git_diff* diff= NULL;
        result = git_diff_tree_to_workdir_with_index( &diff,
                                                      rp->mRepo,
                                                      NULL, *mOpts );
        GW_CHECK_RESULT( result, NULL );

        return new Internal::DiffListPrivate(rp, diff);
    }

    /**
     * @brief           Set option flags, controlling a diff.
     *
     * @param           flags   a combination of @ref git_diff_option_t
     */
    void Diff::setFlags(quint32 flags)
    {
        (**mOpts).flags = flags;
    }


    namespace Internal
    {
        DiffOptions::DiffOptions(Result& result)
            : mOpts( new git_diff_options )
        {
            result = git_diff_init_options( mOpts, GIT_DIFF_OPTIONS_VERSION );
        }

        DiffOptions::DiffOptions(Result& result, git_diff_options* opts)
            : mOpts( opts )
        {
        }

        DiffOptions::~DiffOptions()
        {
            delete mOpts;
        }

        DiffOptions::operator const git_diff_options*() const
        {
            return mOpts;
        }

        git_diff_options& DiffOptions::operator *()
        {
            return *mOpts;
        }

    }

}
