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

#pragma once

#include "libGitWrap/GitWrap.hpp"
#include "libGitWrap/ObjectId.hpp"
#include "libGitWrap/Object.hpp"

namespace Git
{

    namespace Internal
    {

        class BlobPrivate;

    }

    /**
     * @ingroup     GitWrap
     * @brief       Provides access to git BLOB (Binary Large Object) objects
     *
     */
    class GITWRAP_API Blob : public Object
    {
    public:
        GW_PRIVATE_OBJECT_DECL(Blob, Object, public)
    };

    template<>
    inline Blob Object::as() const
    {
        return asBlob();
    }

}

Q_DECLARE_METATYPE(Git::Blob)
