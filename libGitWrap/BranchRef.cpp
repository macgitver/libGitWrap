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

#include <QStringBuilder>

#include "libGitWrap/BranchRef.hpp"
#include "libGitWrap/Repository.hpp"
#include "libGitWrap/Private/RepositoryPrivate.hpp"
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

    QString BranchRef::upstreamName(Result& result) const
    {
        Internal::Buffer buf;
        GW_CD_CHECKED(BranchRef, QString(), result);

        if (d->wasDeleted) {
            return QString();
        }

        result = git_branch_upstream_name(buf, d->repo()->mRepo, GW_StringFromQt(d->fqrn));
        GW_CHECK_RESULT(result, QString());

        return buf;
    }

    QString BranchRef::upstreamRemoteName(Result& result) const
    {
        Internal::Buffer buf;
        GW_CD_CHECKED(BranchRef, QString(), result);

        result = git_branch_upstream_remote(buf, d->repo()->mRepo, GW_StringFromQt(d->fqrn));
        GW_CHECK_RESULT(result, QString());

        return buf;
    }

    Remote BranchRef::upstreamRemote(Result& result) const
    {
        return repository().remote(result, upstreamRemoteName(result));
    }

    Reference BranchRef::upstreamReference(Result& result) const
    {
        git_reference* ref = NULL;

        GW_CD_CHECKED(BranchRef, Reference(), result);

        result = git_branch_upstream(&ref, d->reference);
        GW_CHECK_RESULT(result, BranchRef());

        return new Internal::ReferencePrivate(d->repo(), ref);
    }

    void BranchRef::setUpstream(Result& result, const QString& ref)
    {
        GW_D(BranchRef);
        result = git_branch_set_upstream(d->reference, GW_StringFromQt(ref));
    }

    void BranchRef::setUpstream(Result& result, const Remote& remote)
    {
        GW_D(BranchRef);

        if (!remote.isValid()) {
            result.setInvalidObject();
            return;
        }

        QString ref = remote.name() % QChar(L'/') % shorthand();
        result = git_branch_set_upstream(d->reference, GW_StringFromQt(ref));
    }

    void BranchRef::getAheadBehind(Result& result, size_t& ahead, size_t& behind) const
    {
        GW_CD_CHECKED_VOID(BranchRef, result);

        Repository(d->repo()).calculateDivergence(
                    result,
                    objectId(),
                    upstreamReference(result).objectId(),
                    ahead, behind);
    }

}
