/*
 * MacGitver
 * Copyright (C) 2012-2013 The MacGitver-Developers <dev@macgitver.org>
 *
 * (C) Sascha Cunz <sascha@macgitver.org>
 * (C) Nils Fenner <nilsfenner@web.de>
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

#ifndef GITWRAP_BRANCH_REF_HPP
#define GITWRAP_BRANCH_REF_HPP
#pragma once

#include "libGitWrap/Reference.hpp"

namespace Git
{

    namespace Internal
    {

        class BranchRefPrivate;

    }

    class GITWRAP_API BranchRef : public Reference
    {
        GW_PRIVATE_DECL(BranchRef, Reference, public)

    public:
    };

    template<>
    inline BranchRef Reference::as() const {
        return asBranch();
    }

}

Q_DECLARE_METATYPE(Git::BranchRef)

#endif
