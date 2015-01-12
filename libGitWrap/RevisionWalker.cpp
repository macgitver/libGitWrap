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

#include "libGitWrap/RevisionWalker.hpp"
#include "libGitWrap/ObjectId.hpp"
#include "libGitWrap/Reference.hpp"

#include "libGitWrap/Private/RevisionWalkerPrivate.hpp"

namespace Git
{

    namespace Internal
    {

        RevisionWalkerPrivate::RevisionWalkerPrivate(RepositoryPrivate* repo, git_revwalk* walker )
            : RepoObjectPrivate(repo)
            , mWalker(walker)
        {
            Q_ASSERT(walker);
        }

        RevisionWalkerPrivate::~RevisionWalkerPrivate()
        {
            git_revwalk_free(mWalker);
        }

    }

    GW_PRIVATE_IMPL(RevisionWalker, RepoObject)

    RevisionWalker RevisionWalker::create(Result& result, const Repository& repository)
    {
        if (!result) {
            return RevisionWalker();
        }

        if (!repository) {
            return RevisionWalker();
        }

        Repository::Private* rp = Private::dataOf<Repository>(repository);
        git_revwalk* walker = NULL;

        result = git_revwalk_new(&walker, rp->mRepo);

        if (!result) {
            return RevisionWalker();
        }

        return new RevisionWalker::Private(rp, walker);
    }

    void RevisionWalker::reset( Result& result )
    {
        GW_D_CHECKED(RevisionWalker, void(), result);
        git_revwalk_reset( d->mWalker );
    }

    void RevisionWalker::push(Result& result, const ObjectId& id)
    {
        GW_D_CHECKED(RevisionWalker, void(), result);
        result = git_revwalk_push( d->mWalker, (const git_oid*) id.raw() );
    }

    void RevisionWalker::push(Result& result, const Reference& ref)
    {
        pushRef(result, ref.name());
    }

    void RevisionWalker::pushRef(Result& result, const QString& name)
    {
        GW_D_CHECKED(RevisionWalker, void(), result);
        result = git_revwalk_push_ref( d->mWalker, GW_StringFromQt(name) );
    }

    void RevisionWalker::pushHead( Result& result )
    {
        GW_D_CHECKED(RevisionWalker, void(), result);
        result = git_revwalk_push_head( d->mWalker );
    }

    void RevisionWalker::hide( Result& result, const ObjectId& id )
    {
        GW_D_CHECKED(RevisionWalker, void(), result);
        result = git_revwalk_hide( d->mWalker, (const git_oid*) id.raw() );
    }

    void RevisionWalker::hide(Result& result, const Reference& ref)
    {
        hideRef(result, ref.name());
    }

    void RevisionWalker::hideRef(Result& result, const QString& name)
    {
        GW_D_CHECKED(RevisionWalker, void(), result);
        result = git_revwalk_hide_ref( d->mWalker, GW_StringFromQt(name) );
    }

    void RevisionWalker::hideHead( Result& result )
    {
        GW_D_CHECKED(RevisionWalker, void(), result);
        result = git_revwalk_hide_head( d->mWalker );
    }

    bool RevisionWalker::next(Result& result, ObjectId& oidNext)
    {
        GW_D_CHECKED(RevisionWalker, false, result);

        git_oid oid;
        Result tmp( git_revwalk_next( &oid, d->mWalker ) );
        if( tmp.errorCode() == GIT_ITEROVER )
        {
            return false;
        }
        if( !tmp )
        {
            result = tmp;
            return false;
        }

        oidNext = ObjectId::fromRaw( oid.id );
        return true;
    }

    ObjectIdList RevisionWalker::all( Result& result )
    {
        GW_D_CHECKED(RevisionWalker, ObjectIdList(), result);
        ObjectIdList ids;
        ObjectId id;

        while (next(result, id)) {
            if (!result) {
                return ObjectIdList();
            }
            ids.append( id );
        }

        return ids;
    }

    void RevisionWalker::setSorting(Result& result, bool topological, bool timed)
    {
        GW_D_CHECKED(RevisionWalker, void(), result);
        git_revwalk_sorting( d->mWalker,
                             ( topological ? GIT_SORT_TOPOLOGICAL : 0 ) |
                             ( timed ? GIT_SORT_TIME : 0 ) );
    }

}

