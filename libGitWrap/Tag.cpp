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

#include "libGitWrap/Tag.hpp"

#include "libGitWrap/Private/GitWrapPrivate.hpp"
#include "libGitWrap/Private/TagPrivate.hpp"

namespace Git
{

    namespace Internal {

        TagPrivate::TagPrivate(const RepositoryPrivate::Ptr& repo, git_tag* o)
            : ObjectPrivate(repo, reinterpret_cast<git_object*>(o))
        {
            Q_ASSERT(o);
        }

        TagPrivate::TagPrivate(const RepositoryPrivate::Ptr& repo, git_object* o)
            : ObjectPrivate(repo, o)
        {
            Q_ASSERT(o);
            Q_ASSERT(git_object_type(o) == GIT_OBJ_TAG);
        }

        git_otype TagPrivate::otype() const
        {
            return GIT_OBJ_TAG;
        }

        ObjectType TagPrivate::objectType() const
        {
            return otTag;
        }

    }

    Tag::Tag()
    {
    }

    Tag::Tag(const PrivatePtr& _d)
        : Object(_d)
    {
        GW_D(Tag);
        // This is just for safety
        // can only occur in case of a bad static_cast, which we usually avoid
        if (d->objectType() != otTag) {
            mData = NULL;
        }
    }

    Tag::Tag( const Tag& o )
        : Object( o )
    {
    }

}
