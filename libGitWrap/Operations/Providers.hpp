/*
 * libGitWrap - A Qt wrapper library for libgit2
 * Copyright (C) 2014 The MacGitver-Developers <dev@macgitver.org>
 *
 * (C) Nils Fenner <nils@macgitver.org>
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

namespace Git
{

    class GITWRAP_API BaseOperationProvider : public QSharedData
    {
    public:
        virtual ~BaseOperationProvider() {}

    public:
        virtual Repository repository() const = 0;

        virtual bool prepare() { return true; }
        virtual bool finalize(const ObjectId& commitId) { return true; }
    };

    class GITWRAP_API TreeProvider : public BaseOperationProvider
    {
    public:
        virtual ~TreeProvider() {}

    public:
        virtual Tree tree(Result &result) = 0;
    };

    class GITWRAP_API ParentProvider : public BaseOperationProvider
    {
    public:
        virtual ~ParentProvider() {}

    public:
        virtual ObjectIdList parents(Result &result) const = 0;
    };

    typedef QExplicitlySharedDataPointer<TreeProvider>      TreeProviderPtr;
    typedef QExplicitlySharedDataPointer<ParentProvider>    ParentProviderPtr;

}
