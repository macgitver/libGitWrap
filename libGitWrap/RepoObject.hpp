/*
 * MacGitver
 * Copyright (C) 2012-2013 The MacGitver-Developers <dev@macgitver.org>
 *
 * (C) Sascha Cunz <sascha@macgitver.org>
 * (C) Cunz RaD Ltd.
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

#ifndef GITWRAP_REPO_OBJECT_HPP
#define GITWRAP_REPO_OBJECT_HPP

#include "libGitWrap/Base.hpp"

namespace Git
{

    namespace Internal
    {

        class RepoObjectPrivate;

    }

    class GITWRAP_API RepoObject : public Base
    {
    public:
        RepoObject();

    public:
        bool operator==(const RepoObject& other) const;

    public:
        Repository repository(Result &result) const;
        Repository repository() const;

    protected:
        RepoObject(Internal::RepoObjectPrivate& _d);
    };

}

#endif
