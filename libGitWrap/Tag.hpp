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

    namespace Internal
    {
        class TagPrivate;
    }

    /**
     * @ingroup     GitWrap
     * @brief       Represents a git tag object.
     *
     */
    class GITWRAP_API Tag : public Object
    {
        GW_PRIVATE_DECL(Tag, Object, public)
    public:
        enum { ObjectTypeId = otTag };

    public:
        static ObjectId create(Result& result,
                               const QString& name,
                               const Object& target,
                               const Signature& tagger,
                               const QString& message,
                               bool force);

        static ObjectId createLight(Result& result,
                                    const QString& name,
                                    const Object& target,
                                    bool force);

        static Tag lookup(Result& result, const Repository& repo, const ObjectId& tagId);
    };

    template<>
    inline Tag Object::as() const
    {
        return asTag();
    }

}

Q_DECLARE_METATYPE(Git::Tag)

#endif
