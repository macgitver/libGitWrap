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

#include "libGitWrap/BranchRef.hpp"
#include "libGitWrap/Private/BranchRefPrivate.hpp"

namespace Git
{

    namespace Internal
    {

        BranchRefPrivate::BranchRefPrivate(const RepositoryPrivate::Ptr& repo,
                                           git_reference* reference)
            : ReferencePrivate(repo, reference)
        {
            #ifdef QT_DEBUG
            // We want this constructor to analyze only for the assert...
            ensureAnalyzed();
            Q_ASSERT(isBranch);
            #endif
        }

        BranchRefPrivate::BranchRefPrivate(git_reference* reference, const RefNamePrivate* refName)
            : ReferencePrivate(reference, refName)
        {
        }

        ReferenceKinds BranchRefPrivate::kind() const
        {
            return BranchReference;
        }

    }

    GW_PRIVATE_IMPL(BranchRef, Reference)

}
