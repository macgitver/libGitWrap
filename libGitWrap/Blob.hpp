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

#ifndef GIT_OBJECT_BLOB_H
#define GIT_OBJECT_BLOB_H

#include "libGitWrap/GitWrap.hpp"
#include "libGitWrap/ObjectId.hpp"
#include "libGitWrap/Object.hpp"

namespace Git
{

    /**
     * @ingroup     GitWrap
     * @brief       Provides access to git BLOB (Binary Large Object) objects
     *
     */
    class GITWRAP_API Blob : public Object
    {
    public:
        enum { ObjectTypeId = otBlob };

    public:
        Blob();
        Blob(Internal::ObjectPrivate& _d);
        Blob(const Blob& o);
    };

    template<>
    inline Blob Object::as(Result& result) const
    {
        return asBlob(result);
    }

}

Q_DECLARE_METATYPE(Git::Blob)

#endif
