/*
 * libGitWrap - A Qt wrapper library for libgit2
 * Copyright (C) 2012-2013 The MacGitver-Developers <dev@macgitver.org>
 *
 * (C) Nils Fenner <nilsfenner@web.de>
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
        class IndexEntryPrivate;
    }

    class GITWRAP_API IndexEntry : public Base
    {
        GW_PRIVATE_DECL(IndexEntry, Base, public)

    public:
        QString path() const;
        ObjectId blobSha() const;
        unsigned int mode() const;
        unsigned int ino() const;
        unsigned int uid() const;
        unsigned int gid() const;
        QDateTime cTime() const;
        QDateTime mTime() const;
        int stage() const;
    };

}
