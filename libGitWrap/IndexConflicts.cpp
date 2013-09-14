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
#include "IndexConflictPrivate.hpp"
#include "Index.hpp"
#include "IndexPrivate.hpp"
#include "IndexEntryPrivate.hpp"

namespace Git
{

    IndexConflicts::IndexConflicts(Internal::IndexPrivate* _d)
        : d(_d)
    {
    }

    IndexConflicts::IndexConflicts(const IndexConflicts& other)
        : d(other.d)
    {
    }

    IndexConflicts::~IndexConflicts()
    {
    }

    IndexConflicts& IndexConflicts::operator=(const IndexConflicts& other)
    {
        d = other.d;
        return * this;
    }

    bool IndexConflicts::isValid() const
    {
        return d;
    }

    Index IndexConflicts::index() const
    {
        return Index(d.data());
    }

    void IndexConflicts::refresh()
    {
        // this is a lazy refresh :)
        d->clearKnownConflicts();
    }

    int IndexConflicts::count()
    {
        if (!d) {
            return 0;
        }

        d->ensureConflictsLoaded();
        return d->conflicts.count();
    }

    IndexConflict IndexConflicts::at(int index)
    {
        if (!d) {
            return IndexConflict();
        }

        d->ensureConflictsLoaded();
        return d->conflicts.at(index);
    }

}
