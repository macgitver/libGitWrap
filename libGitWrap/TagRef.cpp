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

#include "libGitWrap/TagRef.hpp"
#include "libGitWrap/Private/TagRefPrivate.hpp"

namespace Git
{

    namespace Internal
    {

        TagRefPrivate::TagRefPrivate(const RepositoryPrivate::Ptr& repo,
                                     git_reference* reference)
            : ReferencePrivate(repo, reference)
        {
            #ifdef QT_DEBUG
            // We want this constructor to analyze only for the assert...
            ensureAnalyzed();
            Q_ASSERT(isTag);
            #endif
        }

        TagRefPrivate::TagRefPrivate(git_reference* reference, const RefNamePrivate* refName)
            : ReferencePrivate(reference, refName)
        {
        }

        ReferenceKinds TagRefPrivate::kind() const
        {
            return TagReference;
        }

    }

    GW_PRIVATE_IMPL(TagRef, Reference)

    CommitOperation* TagRef::commitOperation(const TreeProviderPtr treeProvider, const QString& msg)
    {
        Q_UNUSED( treeProvider )
        Q_UNUSED( msg )

        return NULL;
    }

}
