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
#include "libGitWrap/Private/ObjectPrivate.hpp"

namespace Git
{

    namespace Internal
    {

        ObjectPrivate::ObjectPrivate(RepositoryPrivate* repo, git_object* o)
            : RepoObjectPrivate(repo)
            , mObj(o)
        {
            Q_ASSERT(o);
        }

        ObjectPrivate::~ObjectPrivate()
        {
            git_object_free(mObj);
        }

    }

    Object::Object(Internal::ObjectPrivate& _d)
        : RepoObject(_d)
    {
    }

    Object::Object(const Object& other)
        : RepoObject(other)
    {
    }

    Object::Object()
    {
    }

    Object::~Object()
    {
    }

    Object& Object::operator=( const Object& other )
    {
        RepoObject::operator =(other);
        return * this;
    }

    bool Object::operator==( const Object& other ) const
    {
        return RepoObject::operator==(other);
    }

    ObjectType Object::type(Result& result) const
    {
        GW_CD_CHECKED(Object, otAny, result)

        switch(git_object_type(d->mObj)) {
        case GIT_OBJ_BLOB:      return otBlob;
        case GIT_OBJ_TREE:      return otTree;
        case GIT_OBJ_COMMIT:    return otCommit;
        case GIT_OBJ_TAG:       return otTag;

        default:                return otAny;
        }
    }

    ObjectId Object::id(Result& result) const
    {
        return id();
    }

    ObjectId Object::id() const
    {
        GW_CD(Object);
        if (!d) {
            return ObjectId();
        }

        return Private::oid2sha(git_object_id(d->mObj));
    }


    Tree Object::asTree( Result& result ) const
    {
        Tree o;
        if (isTree(result)) {
            GW_D(Object);
            o = Tree(*d);
        }
        return o;
    }

    Commit Object::asCommit(Result& result) const
    {
        Commit o;
        if (isCommit(result)) {
            GW_D(Object);
            o = Commit(*d);
        }
        return o;
    }

    Blob Object::asBlob(Result& result) const
    {
        Blob o;
        if (isBlob(result)) {
            GW_D(Object);
            o = Blob(*d);
        }
        return o;
    }

    Tag Object::asTag(Result& result) const
    {
        Tag o;
        if (isTag(result)) {
            GW_D(Object);
            o = Tag(*d);
        }
        return o;
    }

    bool Object::isTree(Result& result) const
    {
        return type(result) == otTree;
    }

    bool Object::isTag(Result& result) const
    {
        return type(result) == otTag;
    }

    bool Object::isCommit(Result& result) const
    {
        return type(result) == otCommit;
    }

    bool Object::isBlob(Result& result) const
    {
        return type(result) == otBlob;
    }

}
