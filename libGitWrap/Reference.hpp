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

#ifndef GIT_REFERENCE_H
#define GIT_REFERENCE_H

#include "libGitWrap/RepoObject.hpp"

namespace Git
{

    namespace Internal
    {
        class ReferencePrivate;
    }

    /**
     * @ingroup     GitWrap
     * @brief       Represents a git reference
     *
     */
    class GITWRAP_API Reference : public RepoObject
    {
    public:
        typedef Internal::ReferencePrivate Private;

        enum Type
        {
            Direct, Symbolic, Invalid = -1
        };

    public:
        Reference();
        Reference(Internal::ReferencePrivate& _d);
        Reference( const Reference& other );
        ~Reference();
        Reference& operator=( const Reference& other );

        bool operator==( const Reference& other ) const;
        bool operator!=( const Reference& other ) const;

        int compare( const Reference& other ) const;

    public:
        static Reference create(
                Result& result,
                Repository repo,
                const QString& name,
                const ObjectId& sha);

        static Reference create(
                Result& result,
                Repository repo,
                const QString& name,
                const ObjectCommit& commit);

    public:
        QString name() const;
        QString prefix() const;
        QString shorthand() const;

        RefName nameAnalyzer() const;

        Type type( Result& result ) const;
        ObjectId objectId( Result& result ) const;
        QString target( Result& result ) const;

        Reference resolved( Result& result ) const;
        ObjectId resolveToObjectId( Result& result ) const;

        Object peeled(Result& result, ObjectType ot) const;

        bool isCurrentBranch() const;
        bool isBranch() const;
        bool isLocal() const;
        bool isRemote() const;

        void checkout( Result& result,
                       bool force = false,
                       bool updateHEAD = true,
                       const QStringList &paths = QStringList() ) const;

        void destroy( Result& result );

        void setAsHEAD(Result& result) const;

        void move( Result &result, const ObjectCommit &target );
        void rename(Result &result, const QString &newName , bool force = false );

        void updateHEAD(Result &result) const;
    };

}

Q_DECLARE_METATYPE( Git::Reference )

#endif
