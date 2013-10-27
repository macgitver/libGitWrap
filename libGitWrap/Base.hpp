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

#ifndef GITWRAP_BASE_HPP
#define GITWRAP_BASE_HPP

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
        bool isValid() const;
        bool operator==(const Base& other) const;

    protected:
        Base(Internal::BasePrivate& _d);
        QExplicitlySharedDataPointer<Internal::BasePrivate> mData;
    };

}

#endif
