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

#pragma once

#include "libGitWrap/Result.hpp"

namespace Git
{

    namespace Internal
    {

        class BasePrivate;

    }

    class GITWRAP_API Base
    {
        friend class Internal::BasePrivate;

    public:
        typedef Internal::BasePrivate Private;

    public:
        Base(Internal::BasePrivate* bp = nullptr)
            : mData(bp)
        {
            addRef();
        }

        Base(const Base& other)
            : mData(other.mData)
        {
            addRef();
        }

        virtual ~Base()
        {
            delRef();
        }

        bool isValid() const
        {
            return !!mData;
        }

        Base& operator=(const Base& other)
        {
            if (mData != other.mData) {
                other.addRef();
                delRef();
                mData = other.mData;
            }
            return *this;
        }

        bool operator==(const Base& other) const
        {
            return mData == other.mData;
        }

        bool operator!=(const Base& other) const
        {
            return mData != other.mData;
        }

        bool operator!() const
        {
            return !mData;
        }

        Base(Base&& other) : mData(other.mData)
        {
            other.mData = nullptr;
        }

        Base& operator=(Base&& o)
        {
            std::swap(mData, o.mData);
            return * this;
        }

    protected:
        typedef Internal::GitPtr<Internal::BasePrivate> PrivatePtr;
        Private* mData;

    private:
        void addRef() const;
        void delRef() const;

    protected:
        inline void ensureThisIsNotConst()
        {
            // This method is invoked from the GW_D macro. Its only purpose is to error out at
            // compile time, if we casted from a const outer object. This is actually neccessary
            // because QExplicitlySharedDataPointer and GitPtr actually do give a shit about const
            // correctness.
        }

        template<typename T>
        struct DPtrT
        {
            typedef typename T::Private DPtr;

            DPtrT(T* that)
                : d(static_cast<DPtr*>(that->mData))
            {
            }

            DPtrT(T* that, Result& r)
                : d(static_cast<DPtr*>(that->mData))
            {
                if (Q_LIKELY(d)) {
                    if (d->isValidObject(r)) {
                        return;
                    }
                }
                r.setInvalidObject();
                d = nullptr;
            }

                  DPtr* operator->()            { return d; }
            const DPtr* operator->() const      { return d; }
            operator DPtr*()                    { return d; }
            operator const DPtr*() const        { return d; }

        private:
            DPtr* d;
        };

        template<typename T>
        struct DPtrT<const T>
        {
            typedef typename T::Private DPtr;

            DPtrT(const T* that)
                : d(static_cast<const DPtr*>(that->mData))
            {
            }

            DPtrT(const T* that, Result& r)
                : d(static_cast<const DPtr*>(that->mData))
            {
                if (Q_LIKELY(d)) {
                    if (d->isValidObject(r)) {
                        return;
                    }
                }
                r.setInvalidObject();
                d = nullptr;
            }

            const DPtr* operator->() const      { return d; }
            operator const DPtr*() const        { return d; }

            DPtr* unConst() const {
                return const_cast<DPtr*>(d);
            }

        private:
            const DPtr* d;
        };

    };

}
