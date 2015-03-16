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

#include <QSharedData>

#include "libGitWrap/GitWrap.hpp"   // for convenience

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
        Base();
        Base(const Base& other);
        virtual ~Base();
        Base& operator=(const Base& other);
        bool operator==(const Base& other) const;
        bool operator!=(const Base& other) const;

        bool operator!() const;

    public:
        bool isValid() const;

    protected:
        typedef QExplicitlySharedDataPointer<Internal::BasePrivate> PrivatePtr;
        Base(const PrivatePtr& _d);
        PrivatePtr mData;

    protected:
        inline void ensureThisIsNotConst()
        {
            // This method is invoked from the GW_D macro. Its only purpose is to error out at
            // compile time, if we casted from a const outer object. This is actually neccessary
            // because QExplicitlySharedDataPointer seems to give a shit about const
            // correctness.
        }
    };


    inline bool Base::operator!() const
    {
        return !isValid();
    }

}
