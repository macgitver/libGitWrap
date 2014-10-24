/*
 * MacGitver
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

#include "libGitWrap/Base.hpp"
#include "libGitWrap/Object.hpp"

#include "libGitWrap/Private/BasePrivate.hpp"
#include "libGitWrap/Private/ObjectPrivate.hpp"

namespace Git
{

    namespace Internal
    {

        BasePrivate::BasePrivate()
        {
        }

        BasePrivate::~BasePrivate()
        {
        }

        git_object* BasePrivate::objectOf(const Object& o)
        {
            Object::Private* op = dataOf<Object>(o);

            if (!op) {
                return NULL;
            }

            return op->o();
        }

        /**
         * @internal
         * @brief           Static private validity checker
         *
         * This is a private validity checker. It is used from the GW_D macro. Implementing it as a
         * private static method, allows a lot of code reduction (resulting in isValid() being in
         * the CPU cache and thus much less cache misses on code that uses GitWrap a lot).
         *
         * Also, this allows us to transparently delegate to the virtual method isValidObject(),
         * which gives the object itself an opportunitiy to tell whether it is valid or not and also
         * set a custom result.
         *
         * This is esp. useful since Reference objects can become invalid (when destroy() is used)
         * and we cannot warrant that this will be checked in all methods of Reference derivates.
         *
         * @param[in,out]   r   A Result that will be set to invalid if @a d is `NULL` and can be
         *                      set to anything, if @a d is not `NULL`.
         *
         * @param[in]       d   The BasePrivate to check.
         *
         * @return          `true`, if this object can be treated as vaild.
         *
         */
        bool BasePrivate::isValid(Result& r, const BasePrivate* d)
        {
            if (Q_LIKELY(d)) {
                return d->isValidObject(r);
            }
            r.setInvalidObject();
            return false;
        }

        /**
         * @internal
         * @brief           Check for validity
         *
         * @param[in,out]   r   A Result that can be used to tell the user about any problems with
         *                      the object at hand.
         *
         * @return          `true`, if this object is valid - `false` otherwise. Note, that by
         *                  convention, the object is invalid if the Result @a r contains an error
         *                  on input, thus this method should `return r;` whenever it wants to
         *                  return `true` actually.
         *
         */
        bool BasePrivate::isValidObject(Result& r) const
        {
            return r;
        }

    }

    Base::Base()
    {
    }

    Base::Base(const PrivatePtr& _d)
        : mData(_d)
    {
    }

    Base::Base(const Base& other)
        : mData(other.mData)
    {
    }

    Base::~Base()
    {
    }

    Base& Base::operator=(const Base& other)
    {
        mData = other.mData;
        return * this;
    }

    bool Base::operator==(const Base& other) const
    {
        return mData == other.mData;
    }

    bool Base::operator!=(const Base& other) const
    {
        return mData != other.mData;
    }

    bool Base::isValid() const
    {
        return mData;
    }

}
