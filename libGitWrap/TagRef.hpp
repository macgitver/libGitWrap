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

#ifndef GITWRAP_TAG_REF_HPP
#define GITWRAP_TAG_REF_HPP

#include "libGitWrap/Reference.hpp"

namespace Git
{

    namespace Internal
    {

        class TagRefPrivate;

    }

    class GITWRAP_API TagRef : public Reference
    {
        GW_PRIVATE_DECL(TagRef, Reference, public)

    public:
    };

    template<>
    inline TagRef Reference::as() const {
        return asTag();
    }

}

Q_DECLARE_METATYPE(Git::TagRef)

#endif
