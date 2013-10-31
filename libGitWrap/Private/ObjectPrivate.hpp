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

#ifndef GIT_OBJECT_PRIVATE_H
#define GIT_OBJECT_PRIVATE_H

#include "libGitWrap/Private/RepoObjectPrivate.hpp"

namespace Git
{

    namespace Internal
    {

        /**
         * @internal    GitWrap
         * @brief       The ObjectPrivate class
         *
         */
        class ObjectPrivate : public RepoObjectPrivate
        {
        public:
            Q_DECL_DEPRECATED ObjectPrivate(RepositoryPrivate* repo, git_object* o);
            ~ObjectPrivate();

            git_object* o() { return mObj; }
            const git_object* o() const { return mObj; }

        public:
            git_object* mObj;
        };

        template< class T >
        class ObjectTPrivate : public ObjectPrivate
        {
        public:
            ObjectTPrivate(RepositoryPrivate* _repo, T* _o)
                : ObjectPrivate(_repo, reinterpret_cast<git_object*>(_o))
            {
            }

        public:
            T* o()              { return reinterpret_cast< T* >(mObj); }
            const T* o() const  { return reinterpret_cast< const T* >(mObj); }
        };

        // Without usage of C++11, we cannot provide these typedefs as X::Private typedefs, too.
        // That means, Private::dataOf won't work with these, unless we specialize it. But even that
        // requires some support of C++11...
        typedef ObjectTPrivate<git_commit>  CommitPrivate;
        typedef ObjectTPrivate<git_tree>    TreePrivate;
        typedef ObjectTPrivate<git_blob>    ObjectBlobPrivate;
        typedef ObjectTPrivate<git_tag>     ObjectTagPrivate;

    }

}

#endif
