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

#include "libGitWrap/GitWrap.hpp"
#include "libGitWrap/ChangeListConsumer.hpp"

namespace Git
{

    /**
     * @ingroup     GitWrap
     * @brief       Callback interface to consume a list of differences
     *
     */
    class GITWRAP_API PatchConsumer : public ChangeListConsumer
    {
    public:
        PatchConsumer();
        virtual ~PatchConsumer();

    public:
        virtual bool startHunkChange( int newStart, int newLines, int oldStart, int oldLines,
                                      const QString& header );

        virtual bool appendContext( const QString& content );
        virtual bool appendAddition( const QString& content );
        virtual bool appendDeletion( const QString& content );
    };

}
