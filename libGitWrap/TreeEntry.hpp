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

#include "libGitWrap/Base.hpp"

namespace Git
{

    namespace Internal
    {
        class TreeEntryPrivate;
    }

    /**
     * @ingroup     GitWrap
     * @brief       Represents an entry in a git tree
     */
    class GITWRAP_API TreeEntry : public Base
    {
        GW_PRIVATE_DECL(TreeEntry, Base, public)

    public:
        TreeEntry clone() const;

        ObjectId sha1() const;
        QString name() const;
        ObjectType type() const;
    };

}

Q_DECLARE_METATYPE( Git::TreeEntry )
