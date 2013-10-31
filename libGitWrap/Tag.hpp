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

#ifndef GIT_OBJECT_TAG_H
#define GIT_OBJECT_TAG_H

#include "libGitWrap/GitWrap.hpp"
#include "libGitWrap/ObjectId.hpp"
#include "libGitWrap/Object.hpp"

namespace Git
{

    /**
     * @ingroup     GitWrap
     * @brief       Represents a git tag object.
     *
     */
    class GITWRAP_API Tag : public Object
    {
    public:
        enum { ObjectTypeId = otTag };

    public:
        Tag();
        Tag(Internal::ObjectPrivate& _d);
        Tag(const Tag& o);
    };

    template<>
    inline Tag Object::as(Result& result) const
    {
        return asTag(result);
    }

}

Q_DECLARE_METATYPE(Git::Tag)

#endif
