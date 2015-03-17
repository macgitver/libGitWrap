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

#include "libGitWrap/GitWrap.hpp"

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
        Base(Internal::BasePrivate* bp = GW_NULLPTR)
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

        #if GW_CPP11
        Base(Base&& other) : mData(other.mData)
        {
            other.mData = GW_NULLPTR;
        }

        Base& operator=(Base&& o)
        {
            std::swap(mData, o.mData);
            return * this;
        }
        #endif

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
    };

}
