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

#include "libGitWrap/Tag.hpp"

#include "libGitWrap/Private/ObjectPrivate.hpp"
#include "libGitWrap/Private/GitWrapPrivate.hpp"

namespace Git
{

    Tag::Tag()
    {
    }

    Tag::Tag( Internal::ObjectPrivate& _d )
        : Object(_d)
    {
        Result r;
        if( ( type( r ) != otTag ) || !r )
        {
            mData = NULL;
        }
    }

    Tag::Tag( const Tag& o )
        : Object( o )
    {
    }

}
