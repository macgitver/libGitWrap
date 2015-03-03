/*
 * libGitWrap - A Qt wrapper library for libgit2
 * Copyright (C) 2012-2013 The MacGitver-Developers <dev@macgitver.org>
 *
 * (C) Sascha Cunz <sascha@macgitver.org>
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

#pragma once

#include "libGitWrap/Base.hpp"
#include "libGitWrap/Signature.hpp"
#include "libGitWrap/ObjectId.hpp"
#include "libGitWrap/RepoObject.hpp"

namespace Git
{

    namespace Internal
    {
        class RefLogPrivate;
        class RefLogEntryPrivate;
    }

    class GITWRAP_API RefLogEntry : public Base
    {
        GW_PRIVATE_DECL(RefLogEntry, Base, public)

    public:
        Signature committer() const;
        QString message() const;
        ObjectId oidOld() const;
        ObjectId oidNew() const;
    };

    class GITWRAP_API RefLog : public RepoObject
    {
        GW_PRIVATE_DECL( RefLog, RepoObject, public )

    public:
        int count();
        RefLogEntry at(int index) const;

        void append( Result& result, const ObjectId& oid, const Signature& committer, const QString& message );
        void removeAt( Result& result, int index, bool rewritePreviousEntry = true );

        void write( Result &result ) const;

    public:
        static RefLog read( Result& result,
                            const Repository& repo,
                            const QString& refName );

        static void remove( Result& result,
                            const Repository& repo,
                            const QString& refName );

        static void rename( Result& result,
                            const Repository& repo,
                            const QString& oldRefName,
                            const QString& newRefName );
    };

}
