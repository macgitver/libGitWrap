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

#include "libGitWrap/ObjectId.hpp"
#include "libGitWrap/Object.hpp"
#include "libGitWrap/Tree.hpp"
#include "libGitWrap/Tag.hpp"
#include "libGitWrap/Commit.hpp"
#include "libGitWrap/Blob.hpp"
#include "libGitWrap/Repository.hpp"

#include "libGitWrap/Private/GitWrapPrivate.hpp"
#include "libGitWrap/Private/CommitPrivate.hpp"
#include "libGitWrap/Private/TreePrivate.hpp"
#include "libGitWrap/Private/TagPrivate.hpp"
#include "libGitWrap/Private/BlobPrivate.hpp"

namespace Git
{

    namespace Internal
    {

        ObjectPrivate::ObjectPrivate(const RepositoryPrivate::Ptr& repo, git_object* o)
            : RepoObjectPrivate(repo)
            , mObj(o)
        {
            Q_ASSERT(o);
        }

        ObjectPrivate::~ObjectPrivate()
        {
            git_object_free(mObj);
        }

        Object::PrivatePtr ObjectPrivate::create(const RepositoryPrivate::Ptr& repo, git_object* o)
        {
            Q_ASSERT(o);
            ObjectPrivate* op = NULL;

            switch (git_object_type(o)) {
            case GIT_OBJ_TAG:    op = new TagPrivate(   repo, reinterpret_cast<git_tag*   >(o)); break;
            case GIT_OBJ_COMMIT: op = new CommitPrivate(repo, reinterpret_cast<git_commit*>(o)); break;
            case GIT_OBJ_TREE:   op = new TreePrivate(  repo, reinterpret_cast<git_tree*  >(o)); break;
            case GIT_OBJ_BLOB:   op = new BlobPrivate(  repo, reinterpret_cast<git_blob*  >(o)); break;
            default:             break;
            }

            return Object::PrivatePtr(op);
        }

    }

    GW_PRIVATE_IMPL(Object, RepoObject)

    ObjectType Object::type(Result& result) const
    {
        GW_CD_CHECKED(Object, otAny, result);

        switch(git_object_type(d->mObj)) {
        case GIT_OBJ_BLOB:      return otBlob;
        case GIT_OBJ_TREE:      return otTree;
        case GIT_OBJ_COMMIT:    return otCommit;
        case GIT_OBJ_TAG:       return otTag;

        default:                return otAny;
        }
    }

    ObjectId Object::id() const
    {
        GW_CD(Object);
        if (!d) {
            return ObjectId();
        }

        return Private::oid2sha(git_object_id(d->mObj));
    }


    Tree Object::asTree() const
    {
        Tree o;
        if (isTree()) {
            GW_CD_EX(Tree);
            o = Tree(d);
        }
        return o;
    }

    Commit Object::asCommit() const
    {
        Commit o;
        if (isCommit()) {
            GW_CD_EX(Commit);
            o = Commit(d);
        }
        return o;
    }

    Blob Object::asBlob() const
    {
        Blob o;
        if (isBlob()) {
            GW_CD_EX(Blob);
            o = Blob(d);
        }
        return o;
    }

    Tag Object::asTag() const
    {
        Tag o;
        if (isTag()) {
            GW_CD_EX(Tag);
            o = Tag(d);
        }
        return o;
    }

    bool Object::isTree() const
    {
        GW_CD(Object);
        return d && d->objectType() == otTree;
    }

    bool Object::isTag() const
    {
        GW_CD(Object);
        return d && d->objectType() == otTag;
    }

    bool Object::isCommit() const
    {
        GW_CD(Object);
        return d && d->objectType() == otCommit;
    }

    bool Object::isBlob() const
    {
        GW_CD(Object);
        return d && d->objectType() == otBlob;
    }

}
