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

#ifndef GITWRAP_NOTE_REF_PRIVATE_HPP
#define GITWRAP_NOTE_REF_PRIVATE_HPP
#pragma once

#include "libGitWrap/Private/ReferencePrivate.hpp"

namespace Git
{

    namespace Internal
    {

        class NoteRefPrivate : public ReferencePrivate
        {
        public:
            NoteRefPrivate(const RepositoryPrivate::Ptr& repo, git_reference* reference);
            NoteRefPrivate(git_reference* reference, const RefNamePrivate* refName);

        public:
            ReferenceKinds kind() const;
        };

    }

}

#endif
