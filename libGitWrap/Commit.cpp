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

#include "libGitWrap/Commit.hpp"
#include "libGitWrap/Private/CommitPrivate.hpp"

#include "libGitWrap/Reference.hpp"
#include "libGitWrap/Repository.hpp"
#include "libGitWrap/Tree.hpp"

#include "libGitWrap/Private/GitWrapPrivate.hpp"
#include "libGitWrap/Private/ObjectPrivate.hpp"
#include "libGitWrap/Private/RepositoryPrivate.hpp"
#include "libGitWrap/Private/ReferencePrivate.hpp"
#include "libGitWrap/Private/TreePrivate.hpp"
#include "libGitWrap/Private/DiffListPrivate.hpp"

namespace Git
{

    namespace Internal {

        CommitPrivate::CommitPrivate(const RepositoryPrivate::Ptr& repo, git_commit *o)
            : ObjectPrivate(repo, reinterpret_cast<git_object*>(o))
        {
            Q_ASSERT(o);
        }

        CommitPrivate::CommitPrivate(const RepositoryPrivate::Ptr& repo, git_object *o)
            : ObjectPrivate(repo, o)
        {
            Q_ASSERT(o);
            Q_ASSERT(git_object_type(o) == GIT_OBJ_TREE);
        }

        git_otype CommitPrivate::otype() const
        {
            return GIT_OBJ_TREE;
        }

        ObjectType CommitPrivate::objectType() const
        {
            return otCommit;
        }

        const git_commit** CommitPrivate::commitList2git(Result& result, const CommitList &list)
        {
            if (!result) return NULL;

            const git_commit** ret = new const git_commit *[list.count()];
            for ( int i=0; i < list.count(); ++i )
            {
                ret[i] = dataOf<Commit>(list.at(i))->o();
            }

            return ret;
        }
    }

    GW_PRIVATE_IMPL(Commit, Object)

    /**
     * @brief           Creates a new commit and adds it to the repository.
     *
     *                  The HEAD reference and, if it exist, the current branch
     *                  are updated and point to the new commit.
     *
     * @param[in,out]   result  a result object; see @ref GitWrapErrorHandling
     *
     * @param repo      the target Git repository
     *
     * @param tree      the root tree object within the commit
     *
     * @param message   the commit message
     *
     * @param author    the original commit author's signature
     *
     * @param committer the committer's signature
     *
     * @param parents   the commit's parent commit(s)
     *
     * @return          the created commit object on success or an invalid object
     */
    Commit Commit::create(Result &result, Repository& repo, const Tree &tree, const QString &message,
                          const Signature &author, const Signature &committer, const CommitList &parents)
    {
        GW_CHECK_RESULT( result, Commit() );
        if (!repo.isValid() || !tree.isValid())
        {
            result.setInvalidObject();
            return Commit();
        }

        Repository::Private* rp = Private::dataOf<Repository>(repo);
        git_signature* gitAuthor = Internal::signature2git(result, author);
        git_signature* gitCommitter = Internal::signature2git(result, committer);
        QString branchName = repo.HEAD(result).name();
        QScopedArrayPointer<const git_commit*> constParents( Internal::CommitPrivate::commitList2git(result, parents) );

        if (!result) return Commit();

        git_oid commitId;
        result = git_commit_create( &commitId, rp->mRepo, GW_StringFromQt(branchName),
                                    gitAuthor, gitCommitter,
                                    NULL, GW_StringFromQt(message), Private::dataOf<Tree>(tree)->o(),
                                    parents.count(), constParents.data() );

        return repo.lookupCommit( result, ObjectId::fromRaw(commitId.id) );
    }

    /**
     * @brief           Creates a new commit and adds it to the repository.
     *
     *                  The HEAD reference and, if it exist, the current branch
     *                  are updated and point to the new commit.
     *
     * @param[in,out]   result  a result object; see @ref GitWrapErrorHandling
     *
     * @param repo      the target Git repository
     *
     * @param tree      the root tree object within the commit
     *
     * @param message   the commit message
     *
     * @param author    the original commit author's signature
     *
     * @param committer the committer's signature
     *
     * @param parents   the commit's parent commit(s)
     *
     * @return          the created commit object on success or an invalid object
     */
    Commit Commit::create(Result &result, Repository& repo, const Tree &tree, const QString &message,
                          const Signature &author, const Signature &committer, const ObjectIdList &parents)
    {
        GW_CHECK_RESULT( result, Commit() );
        if (!repo.isValid() || !tree.isValid())
        {
            result.setInvalidObject();
            return Commit();
        }

        Repository::Private* rp = Private::dataOf<Repository>(repo);
        git_signature* gitAuthor = Internal::signature2git(result, author);
        git_signature* gitCommitter = Internal::signature2git(result, committer);
        QString branchName = repo.HEAD(result).name();
        QScopedArrayPointer<const git_oid*> constParents( Internal::ObjectIdList2git(result, parents) );

        if (!result) return Commit();

        git_oid commitId;
        result = git_commit_create_from_ids( &commitId, rp->mRepo, GW_StringFromQt(branchName),
                                             gitAuthor, gitCommitter,
                                             NULL, GW_StringFromQt(message),
                                             Internal::ObjectId2git(tree.id()),
                                             parents.count(), constParents.data() );

        return repo.lookupCommit( result, ObjectId::fromRaw(commitId.id) );
    }

    Commit::operator ParentProviderPtr() const
    {
        return ParentProviderPtr( new CommitParentProvider( *this ) );
    }

    Commit::operator TreeProviderPtr() const
    {
        return TreeProviderPtr( new CommitTreeProvider( *this ) );
    }

    Tree Commit::tree( Result& result ) const
    {
        GW_CHECK_RESULT( result, Tree() );
        GW_CD_CHECKED(Commit, Tree(), result);
        git_tree* tree = NULL;

        result = git_commit_tree(&tree, d->o());
        GW_CHECK_RESULT( result, Tree() );

        return Tree::PrivatePtr(new Tree::Private(d->repo(), tree));
    }

    ObjectId Commit::treeId( Result& result ) const
    {
        GW_CHECK_RESULT( result, ObjectId() );
        GW_CD_CHECKED(Commit, ObjectId(), result);
        return Private::oid2sha(git_commit_tree_id(d->o()));
    }

    ObjectIdList Commit::parentCommitIds( Result& result ) const
    {
        GW_CHECK_RESULT( result, ObjectIdList() );
        GW_CD_CHECKED(Commit, ObjectIdList(), result);

        const git_commit* commit = d->o();
        ObjectIdList ids;
        for (unsigned int i = 0; i < numParentCommits(); i++) {
            ids << Private::oid2sha(git_commit_parent_id(commit, i));
        }

        return ids;
    }

    Commit Commit::parentCommit(Result& result, unsigned int index) const
    {
        GW_CHECK_RESULT( result, Commit() );
        GW_CD_CHECKED(Commit, Commit(), result);
        git_commit* gitparent = NULL;

        result = git_commit_parent(&gitparent, d->o(), index);
        if (!result) {
            return Commit();
        }

        return PrivatePtr(new Private(d->repo(), gitparent));
    }

    ObjectId Commit::parentCommitId(Result& result, unsigned int index) const
    {
        GW_CHECK_RESULT( result, ObjectId() );
        GW_CD_CHECKED(Commit, ObjectId(), result)

        if(numParentCommits() > index) {
            const git_oid* oid = git_commit_parent_id(d->o(), index);
            if (oid) {
                return Private::oid2sha(oid);
            }
        }

        return ObjectId();
    }

    CommitList Commit::parentCommits( Result& result ) const
    {
        GW_CHECK_RESULT( result, CommitList() );
        GW_CD_CHECKED(Commit, CommitList(), result)
        CommitList objs;

        for( unsigned int i = 0; i < numParentCommits(); i++ )
        {
            git_commit* parent = NULL;

            result = git_commit_parent(&parent, d->o(), i);
            if (!result) {
                return CommitList();
            }

            objs.append(PrivatePtr(new Private(d->repo(), parent)));
        }

        return objs;
    }

    unsigned int Commit::numParentCommits() const
    {
        GW_CD(Commit);
        if (!d) {
            return 0;
        }

        return git_commit_parentcount(d->o());
    }

    bool Commit::isParentOf(Result& result, const Git::Commit& child) const
    {
        CommitList parents = child.parentCommits( result );

        for (int i = 0; result && i < parents.count(); i++) {
            if (isEqual(result, parents[i])) {
                return true;
            }
        }

        return false;
    }

    bool Commit::isChildOf(Result& result, const Git::Commit& parent) const
    {
        CommitList children = parentCommits( result );

        for (int i = 0; result && i < children.count(); i++) {
            if (parent.isEqual(result, children[i])) {
                return true;
            }
        }

        return false;
    }

    bool Commit::isEqual(Result& result, const Git::Commit& commit) const
    {
        return result && id() == commit.id();
    }

    Signature Commit::author( Result& result ) const
    {
        return author();
    }

    Signature Commit::committer( Result& result ) const
    {
        return committer();
    }

    QString Commit::message( Result& result ) const
    {
        return message();
    }

    QString Commit::shortMessage( Result& result ) const
    {
        return shortMessage();
    }

    Signature Commit::author() const
    {
        GW_CD(Commit);
        if (!d) {
            return Signature();
        }
        return Internal::git2Signature(git_commit_author(d->o()));
    }

    Signature Commit::committer() const
    {
        GW_CD(Commit);
        if (!d) {
            return Signature();
        }
        return Internal::git2Signature(git_commit_committer(d->o()));
    }

    QString Commit::message() const
    {
        GW_CD(Commit);
        if (!d) {
            return QString();
        }

        const char* msg = git_commit_message(d->o());
        int len = int( strlen( msg ) );

        if (len && msg[len - 1] == '\n') {
            len--;
        }

        return GW_StringToQt(msg, len);
    }

    QString Commit::shortMessage() const
    {
        GW_CD(Commit);
        if (!d) {
            return QString();
        }

        const char* msg = git_commit_message(d->o());

        int len = 0;
        while (msg[len] && msg[len] != '\n') {
            len++;
        }

        return GW_StringToQt(msg, len);
    }

    DiffList Commit::diffFromParent(Result& result, unsigned int index)
    {
        GW_CHECK_RESULT( result, DiffList() );
        GW_CD_CHECKED(Commit, DiffList(), result)

        Commit parentObjCommit = parentCommit( result, index );
        Tree parentObjTree = parentObjCommit.tree( result );

        return parentObjTree.diffToTree( result, tree( result ) );
    }

    DiffList Commit::diffFromAllParents( Result& result )
    {
        GW_CHECK_RESULT( result, DiffList() );
        GW_CD_CHECKED(Commit, DiffList(), result)

        if (numParentCommits() == 0) {
            return DiffList();
        }

        DiffList dl = diffFromParent( result, 0 );
        for (unsigned int i = 1; i < numParentCommits(); i++) {
            DiffList dl2 = diffFromParent( result, i );
            dl2.mergeOnto( result, dl );
        }

        return dl;
    }


    // -- CommitParentProvider -->8

    CommitParentProvider::CommitParentProvider(const Commit &commit)
        : mCommit( commit )
    {
    }

    Repository CommitParentProvider::repository() const
    {
        return mCommit.repository();
    }

    ObjectIdList CommitParentProvider::parents(Result& result) const
    {
        return mCommit.parentCommitIds( result );
    }


    // -- CommitTreeProvider -->8

    CommitTreeProvider::CommitTreeProvider(const Commit &commit)
        : mCommit( commit )
    {
    }

    Repository CommitTreeProvider::repository() const
    {
        return mCommit.repository();
    }

    Tree CommitTreeProvider::tree(Result &result)
    {
        return mCommit.tree( result );
    }

}

/**
 * @ingroup     GitWrap
 * @brief       Debug-Stream support for Git::Commit
 * @param[in]   debug   The Debug-Stream to output into
 * @param[in]   commit  The commit object to output
 * @return      The Debug-Stream
 */
QDebug operator<<( QDebug debug, const Git::Commit& commit )
{
    return debug << "Commit(id=" << commit.id() << ";author=" << commit.author() << ")";
}
