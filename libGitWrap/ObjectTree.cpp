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

#include "DiffList.hpp"
#include "ObjectTree.hpp"
#include "Repository.hpp"

#include "Private/GitWrapPrivate.hpp"
#include "Private/DiffListPrivate.hpp"
#include "Private/ObjectPrivate.hpp"
#include "Private/RepositoryPrivate.hpp"
#include "Private/TreeEntryPrivate.hpp"

namespace Git
{

    ObjectTree::ObjectTree()
    {
    }

    ObjectTree::ObjectTree(Internal::ObjectPrivate& _d)
        : Object( _d )
    {
        Result r;
        if( ( type( r ) != otTree ) || !r )
        {
            mData = NULL;
        }
    }

    ObjectTree::ObjectTree( const ObjectTree& o )
        : Object( o )
    {
    }

    ObjectTree ObjectTree::subPath(Result& result , const QString& pathName) const
    {
        GW_CD_CHECKED(Object, ObjectTree(), result)

        git_tree* d2 = (git_tree*) d->mObj;

        const git_tree_entry* entry = git_tree_entry_byname( d2, pathName.toUtf8().constData() );
        if( !entry )
        {
            return ObjectTree();
        }

        git_object* subObject = 0;
        result = git_tree_entry_to_object( &subObject, d->repo()->mRepo, entry );
        if( !result )
        {
            return ObjectTree();
        }

        if( git_object_type( subObject ) != GIT_OBJ_TREE )
        {
            git_object_free( subObject );
            return ObjectTree();
        }

        return *new Object::Private(d->repo(), subObject);
    }

    DiffList ObjectTree::diffToTree(Result& result , ObjectTree newTree)
    {
        GW_D_CHECKED(ObjectTree, DiffList(), result)

        Object::Private* op = Base::Private::dataOf<Object>(newTree);
        Internal::ObjectTreePrivate* otp = static_cast<Internal::ObjectTreePrivate*>(op);

        git_diff_list* diffList = NULL;
        result = git_diff_tree_to_tree( &diffList, d->repo()->mRepo, d->o(), otp->o(), NULL );
        if( !result )
        {
            return DiffList();
        }

        return DiffList(*new Internal::DiffListPrivate(d->repo(), diffList));
    }

    DiffList ObjectTree::diffToIndex( Result& result )
    {
        GW_D_CHECKED(ObjectTree, DiffList(), result)
        git_diff_list* diffList = NULL;

        result = git_diff_tree_to_index( &diffList, d->repo()->mRepo, d->o(), NULL, NULL );
        if (!result) {
            return DiffList();
        }

        return DiffList(*new Internal::DiffListPrivate(d->repo(), diffList));
    }

    DiffList ObjectTree::diffToWorkingDir( Result& result )
    {
        GW_D_CHECKED(ObjectTree, DiffList(), result)

        git_diff_list* diffList = NULL;
        result = git_diff_tree_to_workdir( &diffList, d->repo()->mRepo, d->o(), NULL );
        if (!result) {
            return DiffList();
        }

        return DiffList(*new Internal::DiffListPrivate(d->repo(), diffList));
    }

    size_t ObjectTree::entryCount() const
    {
        GW_CD(ObjectTree);
        return d ? git_tree_entrycount(d->o()) : 0;
    }

    TreeEntry ObjectTree::entryAt( size_t index ) const
    {
        GW_CD(ObjectTree);

        if(!d) {
            return TreeEntry();
        }
        const git_tree_entry* entry = git_tree_entry_byindex(d->o(), index);
        return *new Internal::TreeEntryPrivate(entry);
    }

    TreeEntry ObjectTree::entry( const QString& fileName ) const
    {
        GW_CD(ObjectTree);

        if(!d) {
            return TreeEntry();
        }

        const git_tree_entry* entry = git_tree_entry_byname(d->o(), fileName.toUtf8().constData());
        return *new Internal::TreeEntryPrivate( entry );
    }

    void ObjectTree::checkout(Result& result, bool force, const QStringList &paths) const
    {
        GW_CD_CHECKED_VOID(Object, result);

        git_checkout_opts opts = GIT_CHECKOUT_OPTS_INIT;
        opts.checkout_strategy = force ? GIT_CHECKOUT_FORCE : GIT_CHECKOUT_SAFE;
        Internal::StrArray(opts.paths, paths);

        result = git_checkout_tree(d->repo()->mRepo, d->mObj, &opts);
    }

}
