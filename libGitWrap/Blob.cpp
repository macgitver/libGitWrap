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

#include "libGitWrap/Blob.hpp"

#include "libGitWrap/Private/GitWrapPrivate.hpp"
#include "libGitWrap/Private/BlobPrivate.hpp"

namespace Git
{

    namespace Internal {

        BlobPrivate::BlobPrivate(RepositoryPrivate* repo, git_blob* o)
            : ObjectPrivate(repo, reinterpret_cast<git_object*>(o))
        {
            Q_ASSERT(o);
        }

        BlobPrivate::BlobPrivate(RepositoryPrivate* repo, git_object* o)
            : ObjectPrivate(repo, o)
        {
            Q_ASSERT(o);
            Q_ASSERT(git_object_type(o) == GIT_OBJ_BLOB);
        }

        git_otype BlobPrivate::otype() const
        {
            return GIT_OBJ_BLOB;
        }

        ObjectType BlobPrivate::objectType() const
        {
            return otBlob;
        }

    }

    Blob::Blob()
    {
    }

    Blob::Blob(Private& _d)
        : Object(_d)
    {
        GW_D(Blob);
        // This is just for safety
        // can only occur in case of a bad static_cast, which we usually avoid
        if (d->objectType() != otBlob) {
            mData = NULL;
        }
    }

    Blob::Blob(const Blob& o)
        : Object(o)
    {
    }

}
