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

#pragma once

#include <QAtomicInt>

#include "libGitWrap/Private/GitWrapPrivate.hpp"

namespace Git
{

    class Object;

    namespace Internal
    {

        /**
         * @internal
         * @ingroup GitWrap
         * @brief The BaseObject class
         *
         */
        class BasePrivate
        {
        public:
            BasePrivate();
            virtual ~BasePrivate();

        public:
            static bool isValid(Result& r, const BasePrivate* d);
            virtual bool isValidObject(Result& r) const;

        public:
            template< class T>
            static typename T::Private* dataOf(T* o)
            {
                return static_cast<typename T::Private*>(o->mData.data());
            }

            template< class T>
            static typename T::Private* dataOf(const T& o)
            {
                return static_cast<typename T::Private*>(o.mData);
            }

            static inline const git_oid* sha(const ObjectId& id)
            {
                return reinterpret_cast<const git_oid*>(id.raw());
            }

            static inline ObjectId oid2sha(const git_oid* oid)
            {
                return ObjectId::fromRaw(oid->id);
            }

            static git_object* objectOf(const Object& o);

            template<class T>
            T outer() const {
                const typename T::Private* ctp = static_cast< const typename T::Private* >(this);
                typename T::Private* tp = const_cast< typename T::Private* >(ctp);
                typename T::PrivatePtr tpp(tp);
                return T(tpp);
            }

        public:
            void addRef() const;
            void delRef() const;

        private:
            mutable QAtomicInt mRef;
        };

    }

}
