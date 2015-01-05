/*
 * MacGitver
 * Copyright (C) 2012-2013 The MacGitver-Developers <dev@macgitver.org>
 *
 * (C) Sascha Cunz <sascha@macgitver.org>
 * (C) Nils Fenner <nilsfenner@web.de>
 * (C) Cunz RaD Ltd.
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

#include "libGitWrap/BranchRef.hpp"
#include "libGitWrap/Private/BranchRefPrivate.hpp"

#include "Private/CommitPrivate.hpp"

#include "libGitWrap/Commit.hpp"


namespace Git
{

    namespace Internal
    {

        BranchRefPrivate::BranchRefPrivate(const RepositoryPrivate::Ptr& repo,
                                           git_reference* reference)
            : ReferencePrivate(repo, reference)
        {
            #ifdef QT_DEBUG
            // We want this constructor to analyze only for the assert...
            ensureAnalyzed();
            Q_ASSERT(isBranch);
            #endif
        }

        BranchRefPrivate::BranchRefPrivate(git_reference* reference, const RefNamePrivate* refName)
            : ReferencePrivate(reference, refName)
        {
        }

        ReferenceKinds BranchRefPrivate::kind() const
        {
            return BranchReference;
        }

    }


    //-- BranchRef -->8

    /**
     * @class      BranchRef
     * @ingroup    GitWrap
     *
     * @brief      Represents a local or remote Git branch reference.
     */

    GW_PRIVATE_IMPL(BranchRef, Reference)

    /**
     * @brief           Create a branch on a commit.
     *
     * @param[in,out]   result  A result object; see @ref GitWrapErrorHandling
     *
     * @param[in]       name    the branches reference name
     *
     * @param[in]       force   if true, creation is forced (an existing branch will be moved)
     *
     * @return          the created reference
     */
    BranchRef BranchRef::create(Result& result, const QString& name, const Commit& commit, bool force)
    {
        GW_CHECK_RESULT( result, BranchRef() );

        if ( !commit.isValid() ) {
            result.setInvalidObject();
            return BranchRef();
        }

        Internal::CommitPrivate* cp = Internal::BasePrivate::dataOf<Commit>( commit );

        git_reference* ref = NULL;
        result = git_branch_create( &ref, cp->repo()->mRepo, GW_StringFromQt(name), cp->o(), force, NULL, NULL );
        GW_CHECK_RESULT( result, BranchRef() );

        return new Internal::BranchRefPrivate(cp->repo(), ref);
    }

}
