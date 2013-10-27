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

#include "ObjectCommit.hpp"
#include "ObjectTree.hpp"
#include "Reference.hpp"

#include "Private/GitWrapPrivate.hpp"
#include "Private/ObjectPrivate.hpp"
#include "Private/RepositoryPrivate.hpp"
#include "Private/ReferencePrivate.hpp"

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

    ObjectTree ObjectCommit::tree( Result& result )
    {
        GW_D_CHECKED(Object, ObjectTree(), result)

        git_commit* commit = (git_commit*) d->mObj;
        git_tree* tree = 0;

        result = git_commit_tree( &tree, commit );
        if( !result )
        {
            return ObjectTree();
        }

        return *new Internal::ObjectPrivate( d->repo(), (git_object*) tree );
    }

    ObjectId ObjectCommit::treeId( Result& result )
    {
        GW_D_CHECKED(Object, ObjectId(), result)

        git_commit* commit = (git_commit*) d->mObj;
        return ObjectId::fromRaw(git_commit_tree_id( commit )->id);
    }

    ObjectIdList ObjectCommit::parentCommitIds( Result& result ) const
    {
        GW_CD_CHECKED(Object, ObjectIdList(), result)
        ObjectIdList ids;

        git_commit* commit = (git_commit*) d->mObj;

        for( unsigned int i = 0; i < numParentCommits(); i++ )
        {
            ObjectId id = ObjectId::fromRaw( git_commit_parent_id( commit, i )->id );
            ids.append( id );
        }

        return ids;
    }

    ObjectCommit ObjectCommit::parentCommit(Result& result, unsigned int index) const
    {
        GW_D_CHECKED(Object, ObjectCommit(), result)

        git_commit* commit = (git_commit*) d->mObj;
        git_commit* gitparent = NULL;

        result = git_commit_parent( &gitparent, commit, index );
        if( !result )
        {
            return ObjectCommit();
        }

        return *new Internal::ObjectPrivate( d->repo(), (git_object*) gitparent );
    }

    ObjectId ObjectCommit::parentCommitId(Result& result, unsigned int index) const
    {
        GW_CD_CHECKED(Object, ObjectId(), result)

        if( numParentCommits() > index )
        {
            git_commit* commit = (git_commit*) d->mObj;

            const git_oid* oid = git_commit_parent_id( commit, index );
            if( oid )
            {
                return ObjectId::fromRaw( oid->id );
            }
        }

        return ObjectId();
    }

    ObjectCommitList ObjectCommit::parentCommits( Result& result ) const
    {
        GW_CD_CHECKED(Object, ObjectCommitList(), result)
        ObjectCommitList objs;

        git_commit* commit = (git_commit*) d->mObj;

        for( unsigned int i = 0; i < numParentCommits(); i++ )
        {
            git_commit* parent = NULL;

            result = git_commit_parent( &parent, commit, i );
            if( !result )
            {
                return ObjectCommitList();
            }

            objs.append(*new Internal::ObjectPrivate(d->repo(), (git_object*) parent));
        }

        return objs;
    }

    unsigned int ObjectCommit::numParentCommits() const
    {
        GW_D(Object);
        if (!d) {
            return 0;
        }

        git_commit* commit = (git_commit*) d->mObj;
        return git_commit_parentcount( commit );
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
        return id(result) == commit.id(result) && result;
    }

    Signature ObjectCommit::author( Result& result ) const
    {
        GW_CD_CHECKED(Object, Signature(), result)

        git_commit* commit = (git_commit*) d->mObj;
        const git_signature* sig = git_commit_author( commit );

        return Internal::git2Signature( sig );
    }

    Signature ObjectCommit::committer( Result& result ) const
    {
        GW_CD_CHECKED(Object, Signature(), result)

        git_commit* commit = (git_commit*) d->mObj;
        const git_signature* sig = git_commit_committer( commit );

        return Internal::git2Signature( sig );
    }

    QString ObjectCommit::message( Result& result ) const
    {
        GW_CD_CHECKED(Object, QString(), result)

        git_commit* commit = (git_commit*) d->mObj;
        const char* msg = git_commit_message( commit );
        int len = int( strlen( msg ) );

        if( len && msg[ len - 1 ] == '\n' )
        {
            len--;
        }

        return QString::fromUtf8( msg, len );
    }

    QString ObjectCommit::shortMessage( Result& result ) const
    {
        GW_CD_CHECKED(Object, QString(), result)

        git_commit* commit = (git_commit*) d->mObj;
        const char* msg = git_commit_message( commit );

        int len = 0;
        while( msg[ len ] && msg[ len ] != '\n' )
        {
            len++;
        }

        return QString::fromUtf8( msg, len );
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
        GW_CD_CHECKED_VOID(Object, result)

        git_checkout_opts opts = GIT_CHECKOUT_OPTS_INIT;
        opts.checkout_strategy = force ? GIT_CHECKOUT_FORCE : GIT_CHECKOUT_SAFE;
        if ( !paths.isEmpty() )
        {
            // TODO: don't copy, just map paths here
            result = git_strarray_copy( &opts.paths, Internal::StrArrayWrapper( paths ) );
            if ( !result ) return;
        }

        result = git_checkout_tree( d->repo()->mRepo, d->mObj, &opts );
        if (updateHEAD) {
            this->updateHEAD(result);
        }
    }

    Reference ObjectCommit::createBranch(Result& result, const QString& name, bool force) const
    {
        GW_CD_CHECKED(Object, Reference(), result)

        git_reference* ref = NULL;
        result = git_branch_create( &ref, d->repo()->mRepo, name.toUtf8().constData(),
                                    (const git_commit*) d->mObj, force );
        if( !result )
        {
            return Reference();
        }

        return Reference( *new Internal::ReferencePrivate( d->repo(), ref ) );
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
    Git::Result r;
    return debug << "Commit(id=" << commit.id( r ) << ";author=" << commit.author( r ) << ")";
}

