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

#include <QStringList>
#include <QRegExp>

#include "libGitWrap/Private/RepoObjectPrivate.hpp"

namespace Git
{

    namespace Internal
    {

        class ReferencePrivate;

        struct CustomMatches
        {
        public:
            CustomMatches()
                : id(-1)
                , payload(NULL)
            {
            }

            CustomMatches(const QRegExp& _re, int _id, void* _payload)
                : id(_id)
                , regExp(_re)
                , payload(_payload)
            {
            }

        public:
            int     id;
            QRegExp regExp;
            void*   payload;
        };

        class RefNameMatches
        {
        public:
            static RefNameMatches& self();

        public:
            const QRegExp           reNamespaces;
            const QRegExp           reRemote;
            const QRegExp           reScopes;
            QVector<CustomMatches>  customMatches;
            int                     nextId;

        private:
            static RefNameMatches* sSelf;
            RefNameMatches();
        };

        class RefNamePrivate : public RepoObjectPrivate
        {
        protected:
            RefNamePrivate(const RefNamePrivate* refName);
        public:
            RefNamePrivate(RepositoryPrivate* repo);
            RefNamePrivate(RepositoryPrivate* repo, const QString& name);

        public:
            RefNamePrivate();

        public:
            bool isAnalyzed     : 1;

            bool isStage        : 1;
            bool isBranch       : 1;
            bool isTag          : 1;
            bool isNote         : 1;
            bool isHead         : 1;
            bool isMergeHead    : 1;
            bool isCommitNote   : 1;
            bool isPecuiliar    : 1;

            QString fqrn;

            QString remote;
            QString name;
            QStringList scopes;
            QStringList namespaces;
            QVector<int> customMatches;

        public:
            static ReferencePrivate* createRefObject(Result& result, Repository::Private *repo,
                                                     const QString& name, git_reference* lgo);

        public:
            virtual bool isRealReference() const;
            void ensureAnalyzed();
            void analyze();

            ReferencePrivate* cloned(git_reference* lgo);

        private:
            template<class T>
            typename T::Private* cloneAs(git_reference* lgo) const
            {
                return new typename T::Private(lgo, this);
            }

        private:
            void scopeTest(QString sub);
            bool analyzeCustom();
        };

    }

}
