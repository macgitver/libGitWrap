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

#include "libGitWrap/NoteRef.hpp"
#include "libGitWrap/Private/NoteRefPrivate.hpp"

namespace Git
{

    namespace Internal
    {

        NoteRefPrivate::NoteRefPrivate(RepositoryPrivate* repo,
                                       git_reference* reference)
            : ReferencePrivate(repo, reference)
        {
            #ifdef QT_DEBUG
            // We want this constructor to analyze only for the assert...
            ensureAnalyzed();
            Q_ASSERT(isNote);
            #endif
        }

        NoteRefPrivate::NoteRefPrivate(git_reference* reference, const RefNamePrivate* refName)
            : ReferencePrivate(reference, refName)
        {
        }

        ReferenceKinds NoteRefPrivate::kind() const
        {
            return NoteReference;
        }

    }

    GW_PRIVATE_IMPL(NoteRef, Reference)

}
