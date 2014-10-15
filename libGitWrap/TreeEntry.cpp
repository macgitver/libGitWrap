/*
 * MacGitver
 * Copyright (C) 2012-2013 Sascha Cunz <sascha@babbelbox.org>
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

#include "libGitWrap/ObjectId.hpp"
#include "libGitWrap/TreeEntry.hpp"

#include "libGitWrap/Private/TreeEntryPrivate.hpp"

namespace Git
{

    namespace Internal
    {

        TreeEntryPrivate::TreeEntryPrivate( const git_tree_entry* entry, bool unmanaged )
            : mEntry( entry )
            , mUnmanaged( unmanaged )
        {
            Q_ASSERT( mEntry );
        }

        TreeEntryPrivate::~TreeEntryPrivate()
        {
            if( !mUnmanaged )
            {
                git_tree_entry_free( const_cast< git_tree_entry* >( mEntry ) );
            }
        }

    }

    GW_PRIVATE_IMPL(TreeEntry, Base)

    TreeEntry TreeEntry::clone() const
    {
        GW_CD(TreeEntry);
        if (!d) {
            return TreeEntry();
        }

        Result result;
        git_tree_entry* entry = NULL;
        result = git_tree_entry_dup( &entry, d->mEntry );
        Q_ASSERT(result && entry);

        return PrivatePtr(new Private(entry));
    }

    ObjectId TreeEntry::sha1() const
    {
        GW_CD(TreeEntry);
        if (!d) {
            return ObjectId();
        }

        const git_oid* oid = git_tree_entry_id(d->mEntry);
        if (!oid) {
            return ObjectId();
        }

        return Private::oid2sha(oid);
    }

    QString TreeEntry::name() const
    {
        GW_CD(TreeEntry);
        if (!d) {
            return QString();
        }

        const char* szName = git_tree_entry_name(d->mEntry);

        if (!szName) {
            return QString();
        }

        return Internal::StringHelper(szName);
    }

    ObjectType TreeEntry::type() const
    {
        GW_CD(TreeEntry);
        if (!d) {
            return otAny;
        }

        return Internal::gitotype2ObjectType(git_tree_entry_type(d->mEntry));
    }

}
