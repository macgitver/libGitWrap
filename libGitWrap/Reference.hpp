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

#include "libGitWrap/Operations/Providers.hpp"

namespace Git
{

    namespace Internal
    {
        class ReferencePrivate;
    }

    class CheckoutBaseOperation;
    class CommitOperation;

    class GITWRAP_API Reference : public RepoObject
    {
        GW_PRIVATE_DECL(Reference, RepoObject, public)

    public:
        int compare(const Reference& other) const;

    public:
        static Reference create(Result& result,
                const Repository& repo,
                const QString& name,
                const ObjectId& sha);

        static Reference create(
                Result& result,
                const Repository& repo,
                const QString& name,
                const Commit& commit);

    public:
        QString name() const;
        QString prefix() const;
        QString shorthand() const;

        RefName nameAnalyzer() const;

        ReferenceTypes type() const;
        ObjectId objectId() const;
        QString target() const;

        Reference resolved( Result& result ) const;
        ObjectId resolveToObjectId( Result& result ) const;

        Object peeled(Result& result, ObjectType ot) const;

        template< class T >
        T peeled(Result& result) const;

        ReferenceKinds kind() const;

        BranchRef asBranch() const;
        TagRef asTag() const;
        NoteRef asNote() const;

        template<class T>
        T as() const;

        bool isCurrentBranch() const;
        bool isBranch() const;
        bool isLocal() const;
        bool isRemote() const;
        bool wasDestroyed() const;

        void destroy( Result& result );

        void setAsDetachedHEAD(Result& result) const;

        void move( Result &result, const Commit &target );
        void rename(Result &result, const QString &newName , bool force = false );

        void updateHEAD(Result &result) const;

    public:
        virtual CommitOperation* commitOperation(const TreeProviderPtr treeProvider, const QString& msg);

    public:
        operator ParentProviderPtr() const;
        operator TreeProviderPtr() const;

    public:
       // -- DEPRECATED FUNCTIONS BEGIN --8>
       /**
        * @brief        Deprecated: Reference::type
        * @deprecated   Use @ref Reference::type() instead.
        */
       GW_DEPRECATED inline ReferenceTypes type(Result& result) const   { return type(); }

       /**
        * @brief        Deprecated: Reference::objectId
        * @deprecated   Use @ref Reference::objectId() instead.
        */
       GW_DEPRECATED inline ObjectId objectId(Result& result) const     { return objectId(); }

       /**
        * @brief        Deprecated: Reference::target
        * @deprecated   Use @ref Reference::target() instead.
        */
       GW_DEPRECATED inline QString target(Result& result) const        { return target(); }

       // <8-- DEPRECATED FUNCTIONS END --
    };

    template< class T >
    inline T Reference::peeled(Result& result) const
    {
        return peeled(result, ObjectType(T::ObjectTypeId)).as<T>();
    }


    class GITWRAP_API ReferenceParentProvider : public ParentProvider
    {
    public:
        ReferenceParentProvider( const Reference& ref );

    public:
        // INTERFACE REALIZATION
        ObjectIdList parents(Result& result) const;
        Repository repository() const;

    private:
        Reference   mRef;
    };

    class GITWRAP_API ReferenceTreeProvider :public TreeProvider
    {
    public:
        ReferenceTreeProvider( const Reference& ref );

    private:
        Reference   mRef;

    public:
        // INTERFACE REALIZATION
        Repository repository() const;
        Tree tree(Result& result);
    };

}

Q_DECLARE_METATYPE(Git::Reference)

#endif
