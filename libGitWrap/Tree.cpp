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

#include "libGitWrap/DiffList.hpp"
#include "libGitWrap/Tree.hpp"
#include "libGitWrap/TreeEntry.hpp"
#include "libGitWrap/Repository.hpp"

#include "libGitWrap/Private/GitWrapPrivate.hpp"
#include "libGitWrap/Private/DiffListPrivate.hpp"
#include "libGitWrap/Private/ObjectPrivate.hpp"
#include "libGitWrap/Private/RepositoryPrivate.hpp"
#include "libGitWrap/Private/TreeEntryPrivate.hpp"
#include "libGitWrap/Private/TreePrivate.hpp"

namespace Git
{

    namespace Internal {

        TreePrivate::TreePrivate(const RepositoryPrivate::Ptr& repo, git_tree* o)
            : ObjectPrivate(repo, reinterpret_cast<git_object*>(o))
        {
            Q_ASSERT(o);
        }

        TreePrivate::TreePrivate(const RepositoryPrivate::Ptr& repo, git_object* o)
            : ObjectPrivate(repo, o)
        {
            Q_ASSERT(o);
            Q_ASSERT(git_object_type(o) == GIT_OBJ_TREE);
        }

        git_otype TreePrivate::otype() const
        {
            return GIT_OBJ_TREE;
        }

        ObjectType TreePrivate::objectType() const
        {
            return otTree;
        }

    }

    GW_PRIVATE_IMPL(Tree, Object)

    Tree Tree::subPath(Result& result , const QString& pathName) const
    {
        GW_CD_CHECKED(Tree, Tree(), result)

        const git_tree_entry* entry = git_tree_entry_byname(d->o(), pathName.toUtf8().constData());
        if (!entry) {
            return Tree();
        }

        git_object* subObject = 0;
        result = git_tree_entry_to_object(&subObject, d->repo()->mRepo, entry);
        if (!result) {
            return Tree();
        }

        if (git_object_type(subObject) != GIT_OBJ_TREE) {
            git_object_free(subObject);
            return Tree();
        }

        return PrivatePtr(new Tree::Private(d->repo(), subObject));
    }

    DiffList Tree::diffToTree(Result& result , Tree newTree)
    {
        GW_D_CHECKED(Tree, DiffList(), result)

        Tree::Private* tp = Base::Private::dataOf<Tree>(newTree);

        git_diff* diff = NULL;
        result = git_diff_tree_to_tree(&diff, d->repo()->mRepo, d->o(), tp->o(), NULL);
        if (!result) {
            return DiffList();
        }

        return DiffList::PrivatePtr(new DiffList::Private(d->repo(), diff));
    }

    DiffList Tree::diffToIndex(Result& result)
    {
        GW_D_CHECKED(Tree, DiffList(), result)
        git_diff* diff = NULL;

        result = git_diff_tree_to_index(&diff, d->repo()->mRepo, d->o(), NULL, NULL);
        if (!result) {
            return DiffList();
        }

        return DiffList::PrivatePtr(new DiffList::Private(d->repo(), diff));
    }

    DiffList Tree::diffToWorkingDir( Result& result )
    {
        GW_D_CHECKED(Tree, DiffList(), result)

        git_diff* diff= NULL;
        result = git_diff_tree_to_workdir( &diff, d->repo()->mRepo, d->o(), NULL );
        if (!result) {
            return DiffList();
        }

        return DiffList::PrivatePtr(new DiffList::Private(d->repo(), diff));
    }

    size_t Tree::entryCount() const
    {
        GW_CD(Tree);
        return d ? git_tree_entrycount(d->o()) : 0;
    }

    TreeEntry Tree::entryAt( size_t index ) const
    {
        GW_CD(Tree);

        if(!d) {
            return TreeEntry();
        }
        const git_tree_entry* entry = git_tree_entry_byindex(d->o(), index);
        return TreeEntry::PrivatePtr(new TreeEntry::Private(entry));
    }

    TreeEntry Tree::entry(const QString& fileName) const
    {
        GW_CD(Tree);

        if (!d) {
            return TreeEntry();
        }

        const git_tree_entry* entry = git_tree_entry_byname(d->o(), fileName.toUtf8().constData());
        return TreeEntry::PrivatePtr(new TreeEntry::Private(entry));
    }

    void Tree::checkout(Result& result, bool force, const QStringList &paths) const
    {
        GW_CD_CHECKED_VOID(Tree, result);

        git_checkout_options opts = GIT_CHECKOUT_OPTIONS_INIT;
        opts.checkout_strategy = force ? GIT_CHECKOUT_FORCE : GIT_CHECKOUT_SAFE;
        Internal::StrArray(opts.paths, paths);

        result = git_checkout_tree(d->repo()->mRepo, d->mObj, &opts);
    }

    TreeEntry Tree::operator[](const QString& fileName) const
    {
        return entry( fileName );
    }

    TreeEntry Tree::operator[](size_t index) const
    {
        return entryAt( index );
    }

}
