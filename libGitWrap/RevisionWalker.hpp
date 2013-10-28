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

#ifndef GIT_REVISION_WALKER_H
#define GIT_REVISION_WALKER_H

#include "libGitWrap/RepoObject.hpp"

namespace Git
{

    namespace Internal
    {
        class RevisionWalkerPrivate;
    }

    /**
     * @ingroup     GitWrap
     * @brief       Provides access to a git repository's history
     *
     */
    class GITWRAP_API RevisionWalker : public RepoObject
    {
    public:
        RevisionWalker();
        RevisionWalker(Internal::RevisionWalkerPrivate& _d);
        ~RevisionWalker();
        RevisionWalker& operator=( const RevisionWalker& other );

    public:
        void reset( Result& result );

        void push( Result& result, const ObjectId& id  );
        void push( Result& result, const Reference& ref );
        void pushRef( Result& result, const QString& name );
        void pushHead( Result& result );

        void hide( Result& result, const ObjectId& id );
        void hide( Result& result, const Reference& ref );
        void hideRef( Result& result, const QString& name );
        void hideHead( Result& result );

        bool next( Result& result, ObjectId& oidNext );
        ObjectIdList all( Result& result );

        void setSorting( Result& result, bool topological, bool timed );
    };

}

Q_DECLARE_METATYPE( Git::RevisionWalker )

#endif
