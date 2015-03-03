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

#include "libGitWrap/RepoObject.hpp"

namespace Git
{

    namespace Internal
    {

        class RefNamePrivate;

    }

    class GITWRAP_API RefName : public RepoObject
    {
        GW_PRIVATE_DECL(RefName, RepoObject, public)

    public:
        explicit RefName(const Repository& repo, const QString& refName);
        explicit RefName(const QString& refName);

    public:
        bool isRemote();

        bool isTag();
        bool isBranch();
        bool isNote();

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
        QString noteName();
        QString branchName();

        QString shorthand();

        bool matchesCustomRule(int id);

        Reference reference() const;

    public:
        static int registerExpression(void* data, const QRegExp& regExp);
        static void unregisterExpression(int id);
        static void* expressionData(int id);
    };

}
