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

#ifndef GIT_REF_NAME_HPP
#define GIT_REF_NAME_HPP

#include <QStringList>
#include <QRegExp>

#include "GitWrap.hpp"

namespace Git
{

    namespace Internal
    {

        class RefNamePrivate;

    }

    class GITWRAP_API RefName
    {
    public:
        RefName();
        RefName(const RefName& other);
        explicit RefName(const QString& refName);
        ~RefName();
        RefName& operator=(const RefName& other);
        bool isValid() const;

    public:
        bool isRemote();

        bool isTag();
        bool isBranch();

        bool isStage();
        bool isHead();
        bool isMergeHead();
        bool isCommitNote();
        bool isSpecial();
        bool isPeculiar();

        bool isScoped();
        bool isNamespaced();

        bool isCustom();

        QStringList namespaces();
        QStringList scopes();

        QString namespaceName();
        QString remote();
        QString scopeName();
        QString name();
        QString fullName();
        QString localName();
        QString scopePrefix();

        QString tagName();
        QString branchName();

        bool matchesCustomRule(int id);

    public:
        static int registerExpression(void* data, const QRegExp& regExp);
        static void unregisterExpression(int id);
        static void* expressionData(int id);

    private:
        Internal::GitPtr< Internal::RefNamePrivate > d;
    };

}

#endif
