/*
 * libGitWrap - A Qt wrapper library for libgit2
 * Copyright (C) 2012-2013 The MacGitver-Developers <dev@macgitver.org>
 *
 * (C) Sascha Cunz <sascha@macgitver.org>
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

#include "IndexConflict.hpp"
#include "IndexConflicts.hpp"
#include "Index.hpp"

#include "Private/IndexPrivate.hpp"
#include "Private/IndexEntryPrivate.hpp"
#include "Private/IndexConflictPrivate.hpp"

namespace Git
{

    IndexConflicts::IndexConflicts(Internal::IndexPrivate& _d)
        : Base(_d)
    {
    }

    IndexConflicts::IndexConflicts(const IndexConflicts& other)
        : Base(other)
    {
    }

    IndexConflicts::~IndexConflicts()
    {
    }

    IndexConflicts& IndexConflicts::operator=(const IndexConflicts& other)
    {
        Base::operator =(other);
        return * this;
    }

    Index IndexConflicts::index() const
    {
        GW_D(Index);
        return Index(*d);
    }

    void IndexConflicts::refresh()
    {
        GW_D(Index);
        // this is a lazy refresh :)
        d->clearKnownConflicts();
    }

    int IndexConflicts::count()
    {
        GW_D(Index);
        if (!d) {
            return 0;
        }

        d->ensureConflictsLoaded();
        return d->conflicts.count();
    }

    IndexConflict IndexConflicts::at(int index)
    {
        GW_D(Index);
        if (!d) {
            return IndexConflict();
        }

        d->ensureConflictsLoaded();
        return d->conflicts.at(index);
    }

}
