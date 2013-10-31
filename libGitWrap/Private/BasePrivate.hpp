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

#ifndef GIT_BASIC_OBJECT_H
#define GIT_BASIC_OBJECT_H

#include <QSharedData>

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
        class BasePrivate : public QSharedData
        {
        public:
            BasePrivate();
            virtual ~BasePrivate();

        public:
            template< class T>
            static typename T::Private* dataOf(T* o)
            {
                return static_cast<typename T::Private*>(o->mData.data());
            }

            template< class T>
            static typename T::Private* dataOf(const T& o)
            {
                return static_cast<typename T::Private*>(o.mData.data());
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

        };

    }

}

#define GW_D(CLASS) \
    Internal::CLASS##Private* d = \
        static_cast<Internal::CLASS##Private*>(mData.data())

#define GW_CD(CLASS) \
    const Internal::CLASS##Private* d = \
        static_cast<const Internal::CLASS##Private*>(mData.constData())

#define GW_D_CHECKED(CLASS, returns, result) \
    GW_D(CLASS); \
    if (!result) { return returns; } \
    if (!d) { result.setInvalidObject(); return returns; }

#define GW_CD_CHECKED(CLASS, returns, result) \
    GW_CD(CLASS); \
    if (!result) { return returns; } \
    if (!d) { result.setInvalidObject(); return returns; }

// Wherever we have to use one of those two, we've made bad API design!
#define GW_D_CHECKED_VOID(CLASS, result) \
    GW_D(CLASS); \
    if (!result) { return; } \
    if (!d) { result.setInvalidObject(); return; }

#define GW_CD_CHECKED_VOID(CLASS, result) \
    GW_CD(CLASS); \
    if (!result) { return; } \
    if (!d) { result.setInvalidObject(); return; }

#endif

