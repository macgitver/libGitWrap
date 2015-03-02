/*
 * libGitWrap - A Qt wrapper library for libgit2
 * Copyright (C) 2015 The MacGitver-Developers <dev@macgitver.org>
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

#include "libGitWrap/Tree.hpp"
#include "libGitWrap/TreeEntry.hpp"
#include "libGitWrap/Repository.hpp"

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
        GW_CD_CHECKED(Tree, Tree(), result);

        const git_tree_entry* entry = git_tree_entry_byname(d->o(), GW_StringFromQt(pathName));
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

        const git_tree_entry* entry = git_tree_entry_byname(d->o(), GW_StringFromQt(fileName));
        return TreeEntry::PrivatePtr(new TreeEntry::Private(entry));
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
