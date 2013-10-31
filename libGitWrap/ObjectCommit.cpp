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

#include "libGitWrap/ObjectCommit.hpp"
#include "libGitWrap/ObjectTree.hpp"
#include "libGitWrap/Reference.hpp"
#include "libGitWrap/Repository.hpp"

#include "libGitWrap/Private/GitWrapPrivate.hpp"
#include "libGitWrap/Private/ObjectPrivate.hpp"
#include "libGitWrap/Private/RepositoryPrivate.hpp"
#include "libGitWrap/Private/ReferencePrivate.hpp"

namespace Git
{

    ObjectCommit::ObjectCommit()
    {
    }

    ObjectCommit::ObjectCommit(Internal::ObjectPrivate& _d)
        : Object(_d)
    {
        Result r;
        if( ( type( r ) != otCommit ) || !r )
        {
            mData = NULL;
        }
    }

    ObjectCommit::ObjectCommit( const ObjectCommit& o )
        : Object( o )
    {
    }

    ObjectTree ObjectCommit::tree( Result& result ) const
    {
        GW_D_CHECKED(Object, ObjectTree(), result)

        const git_commit* commit = reinterpret_cast<git_commit*>(d->mObj);
        git_tree* tree = 0;

        result = git_commit_tree( &tree, commit );
        if( !result )
        {
            return ObjectTree();
        }

        return *new Internal::ObjectTreePrivate(d->repo(), tree);
    }

    ObjectId ObjectCommit::treeId( Result& result ) const
    {
        GW_D_CHECKED(ObjectCommit, ObjectId(), result);
        return Private::oid2sha(git_commit_tree_id(d->o()));
    }

    ObjectIdList ObjectCommit::parentCommitIds( Result& result ) const
    {
        GW_CD_CHECKED(ObjectCommit, ObjectIdList(), result);
        const git_commit* commit = d->o();
        ObjectIdList ids;

        for (unsigned int i = 0; i < numParentCommits(); i++) {
            ids << Private::oid2sha(git_commit_parent_id(commit, i));
        }

        return ids;
    }

    ObjectCommit ObjectCommit::parentCommit(Result& result, unsigned int index) const
    {
        GW_D_CHECKED(ObjectCommit, ObjectCommit(), result);

        const git_commit* commit = d->o();
        git_commit* gitparent = NULL;

        result = git_commit_parent(&gitparent, commit, index);
        if (!result) {
            return ObjectCommit();
        }

        return *new Internal::ObjectCommitPrivate(d->repo(), gitparent);
    }

    ObjectId ObjectCommit::parentCommitId(Result& result, unsigned int index) const
    {
        GW_CD_CHECKED(ObjectCommit, ObjectId(), result)

        if(numParentCommits() > index) {
            const git_oid* oid = git_commit_parent_id(d->o(), index);
            if (oid) {
                return Private::oid2sha(oid);
            }
        }

        return ObjectId();
    }

    ObjectCommitList ObjectCommit::parentCommits( Result& result ) const
    {
        GW_CD_CHECKED(ObjectCommit, ObjectCommitList(), result)
        ObjectCommitList objs;

        for( unsigned int i = 0; i < numParentCommits(); i++ )
        {
            git_commit* parent = NULL;

            result = git_commit_parent(&parent, d->o(), i);
            if (!result) {
                return ObjectCommitList();
            }

            objs.append(*new Internal::ObjectCommitPrivate(d->repo(), parent));
        }

        return objs;
    }

    unsigned int ObjectCommit::numParentCommits() const
    {
        GW_D(ObjectCommit);
        if (!d) {
            return 0;
        }

        return git_commit_parentcount(d->o());
    }

    bool ObjectCommit::isParentOf(Result& result, const Git::ObjectCommit& child) const
    {
        QVector< Git::ObjectCommit > parents = child.parentCommits( result );

        for (int i = 0; result && i < parents.count(); i++) {
            if (isEqual(result, parents[i])) {
                return true;
            }
        }

        return false;
    }

    bool ObjectCommit::isChildOf(Result& result, const Git::ObjectCommit& parent) const
    {
        QVector< Git::ObjectCommit > children = parentCommits( result );

        for (int i = 0; result && i < children.count(); i++) {
            if (parent.isEqual(result, children[i])) {
                return true;
            }
        }

        return false;
    }

    bool ObjectCommit::isEqual(Result& result, const Git::ObjectCommit& commit) const
    {
        return result && id() == commit.id();
    }

    Signature ObjectCommit::author( Result& result ) const
    {
        return author();
    }

    Signature ObjectCommit::committer( Result& result ) const
    {
        return committer();
    }

    QString ObjectCommit::message( Result& result ) const
    {
        return message();
    }

    QString ObjectCommit::shortMessage( Result& result ) const
    {
        return shortMessage();
    }

    Signature ObjectCommit::author() const
    {
        GW_CD(ObjectCommit);
        if (!d) {
            return Signature();
        }
        return Internal::git2Signature(git_commit_author(d->o()));
    }

    Signature ObjectCommit::committer() const
    {
        GW_CD(ObjectCommit);
        if (!d) {
            return Signature();
        }
        return Internal::git2Signature(git_commit_committer(d->o()));
    }

    QString ObjectCommit::message() const
    {
        GW_CD(ObjectCommit);
        if (!d) {
            return QString();
        }

        const char* msg = git_commit_message(d->o());
        int len = int( strlen( msg ) );

        if (len && msg[len - 1] == '\n') {
            len--;
        }

        return QString::fromUtf8(msg, len);
    }

    QString ObjectCommit::shortMessage() const
    {
        GW_CD(ObjectCommit);
        if (!d) {
            return QString();
        }

        const char* msg = git_commit_message(d->o());

        int len = 0;
        while (msg[len] && msg[len] != '\n') {
            len++;
        }

        return QString::fromUtf8(msg, len);
    }

    /**
     * @brief           Checkout this commit.
     *
     * @param[in,out]   result  A Result object; see @ref GitWrapErrorHandling
     *
     * @param[in]       force   If @c true, files will be overwritten. If @c false (the default),
     *                          the operation is canceled in case of any problem.
     *
     * @param[in]       updateHEAD  If @c true, ObjectCommit::updateHEAD() is called after a
     *                              successful checkout. If @c false (the default), updateHEAD is
     *                              not called.
     *
     * @param[in]       paths   Inclusive filters to the tree to checkout. If empty (the default),
     *                          the whole tree is checked out.
     *
     */
    void ObjectCommit::checkout(Result &result, bool force, bool updateHEAD,
                                const QStringList &paths) const
    {
        GW_CD_CHECKED_VOID(Object, result);

        tree(result).checkout(result, force, paths);

        if (result && updateHEAD) {
            setAsHEAD(result);
        }
    }

    Reference ObjectCommit::createBranch(Result& result, const QString& name, bool force) const
    {
        GW_CD_CHECKED(ObjectCommit, Reference(), result);

        git_reference* ref = NULL;
        result = git_branch_create(&ref, d->repo()->mRepo, name.toUtf8().constData(), d->o(), force);

        if (!result) {
            return Reference();
        }

        return Reference( *new Reference::Private(d->repo(), ref));
    }

    DiffList ObjectCommit::diffFromParent(Result& result, unsigned int index)
    {
        GW_CD_CHECKED(Object, DiffList(), result)

        ObjectCommit parentObjCommit = parentCommit( result, index );
        ObjectTree parentObjTree = parentObjCommit.tree( result );

        return parentObjTree.diffToTree( result, tree( result ) );
    }

    DiffList ObjectCommit::diffFromAllParents( Result& result )
    {
        GW_CD_CHECKED(Object, DiffList(), result)

        if( numParentCommits() == 0 )
        {
            return DiffList();
        }

        DiffList dl = diffFromParent( result, 0 );
        for( unsigned int i = 1; i < numParentCommits(); i++ )
        {
            DiffList dl2 = diffFromParent( result, i );
            dl2.mergeOnto( result, dl );
        }

        return dl;
    }

    void ObjectCommit::setAsHEAD(Result& result) const
    {
        GW_CD_CHECKED_VOID(Object, result);
        repository().setHEAD(result, *this);
    }

    /**
     * @brief           Set the HEAD ref to point (detached) to this commit.
     *
     * @param[in,out]   result  A Result object; see @ref GitWrapErrorHandling
     *
     */
    void ObjectCommit::updateHEAD(Result &result) const
    {
        GW_CD_CHECKED_VOID(Object, result)

        result = git_repository_set_head_detached( d->repo()->mRepo
                                                   , git_object_id( d->mObj ) );
    }

}

/**
 * @ingroup     GitWrap
 * @brief       Debug-Stream support of Git::ObjectCommit
 * @param[in]   debug   The Debug-Stream to output into
 * @param[in]   commit  The commit object to output
 * @return      The Debug-Stream
 */
QDebug operator<<( QDebug debug, const Git::ObjectCommit& commit )
{
    return debug << "Commit(id=" << commit.id() << ";author=" << commit.author() << ")";
}

