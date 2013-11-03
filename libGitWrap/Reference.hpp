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
#include "libGitWrap/Object.hpp"

namespace Git
{

    namespace Internal
    {
        class ReferencePrivate;
    }

    class GITWRAP_API Reference : public RepoObject
    {
        GW_PRIVATE_DECL(Reference, RepoObject, public);
    public:
        enum Type
        {
            Direct, Symbolic, Invalid = -1
        };

    public:
        Reference();
        Reference(const Reference& other);
        ~Reference();
        Reference& operator=(const Reference& other);

        bool operator==(const Reference& other) const;
        bool operator!=(const Reference& other) const;

        int compare(const Reference& other) const;

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
                const Commit& commit);

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

        template< class T >
        T peeled(Result& result) const;

        bool isCurrentBranch() const;
        bool isBranch() const;
        bool isLocal() const;
        bool isRemote() const;
        bool wasDestroyed() const;

        void checkout( Result& result,
                       CheckoutOptions opts = CheckoutNone,
                       const QStringList &paths = QStringList() ) const;

        void destroy( Result& result );

        void setAsHEAD(Result& result) const;

        void move( Result &result, const Commit &target );
        void rename(Result &result, const QString &newName , bool force = false );

        void updateHEAD(Result &result) const;
    };

    template< class T >
    inline T Reference::peeled(Result& result) const
    {
        return peeled(result, ObjectType(T::ObjectTypeId)).as<T>();
    }

}

Q_DECLARE_METATYPE(Git::Reference)

#endif
