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

#include "gtest/gtest.h"

#include "libGitWrap/RefName.hpp"

TEST(RefName, Analyze) {
    const char* sz = "refs/heads/master";
    Git::RefName rn = Git::RefName(QLatin1String(sz));
    EXPECT_TRUE (rn.isValid());

    EXPECT_TRUE (rn.isBranch());
    EXPECT_FALSE(rn.isSpecial());
    EXPECT_FALSE(rn.isTag());
    EXPECT_FALSE(rn.isCommitNote());
    EXPECT_FALSE(rn.isHead());
    EXPECT_FALSE(rn.isMergeHead());
    EXPECT_FALSE(rn.isNamespaced());
    EXPECT_FALSE(rn.isScoped());
    EXPECT_FALSE(rn.isPeculiar());
    EXPECT_FALSE(rn.isStage());
    EXPECT_FALSE(rn.isCustom());
    EXPECT_STREQ(qPrintable(rn.name()), "master");
    EXPECT_STREQ(qPrintable(rn.branchName()), "master");
    EXPECT_EQ(rn.namespaces().count(), 0);
    EXPECT_EQ(rn.scopes().count(), 0);
    EXPECT_STREQ(qPrintable(rn.scopeName()), "");
    EXPECT_STREQ("", qPrintable(rn.remote()));
}

TEST(RefName, Namespaces) {
    const char* sz = "refs/namespaces/foo/refs/heads/master";
    Git::RefName rn = Git::RefName(QLatin1String(sz));
    EXPECT_TRUE (rn.isValid());

    EXPECT_TRUE (rn.isBranch());
    EXPECT_FALSE(rn.isSpecial());
    EXPECT_FALSE(rn.isTag());
    EXPECT_FALSE(rn.isCommitNote());
    EXPECT_FALSE(rn.isHead());
    EXPECT_FALSE(rn.isMergeHead());
    EXPECT_TRUE (rn.isNamespaced());
    EXPECT_FALSE(rn.isScoped());
    EXPECT_FALSE(rn.isPeculiar());
    EXPECT_FALSE(rn.isStage());
    EXPECT_FALSE(rn.isCustom());
    EXPECT_STREQ(qPrintable(rn.name()), "master");
    EXPECT_STREQ(qPrintable(rn.branchName()), "master");
    ASSERT_EQ(rn.namespaces().count(), 1);
    EXPECT_STREQ(qPrintable(rn.namespaces()[0]), "foo");
    EXPECT_STREQ(qPrintable(rn.namespaceName()), "foo");
    EXPECT_EQ(rn.scopes().count(), 0);
    EXPECT_STREQ(qPrintable(rn.scopeName()), "");
    EXPECT_STREQ("", qPrintable(rn.remote()));
}

TEST(RefName, NestedNamespaces) {
    const char* sz = "refs/namespaces/bar/refs/namespaces/foo/refs/heads/master";
    Git::RefName rn = Git::RefName(QLatin1String(sz));
    EXPECT_TRUE (rn.isValid());

    EXPECT_TRUE (rn.isBranch());
    EXPECT_FALSE(rn.isSpecial());
    EXPECT_FALSE(rn.isTag());
    EXPECT_FALSE(rn.isCommitNote());
    EXPECT_FALSE(rn.isHead());
    EXPECT_FALSE(rn.isMergeHead());
    EXPECT_TRUE (rn.isNamespaced());
    EXPECT_FALSE(rn.isScoped());
    EXPECT_FALSE(rn.isPeculiar());
    EXPECT_FALSE(rn.isStage());
    EXPECT_FALSE(rn.isCustom());
    EXPECT_STREQ(qPrintable(rn.name()), "master");
    EXPECT_STREQ(qPrintable(rn.branchName()), "master");
    ASSERT_EQ(rn.namespaces().count(), 2);
    EXPECT_STREQ(qPrintable(rn.namespaces()[0]), "bar");
    EXPECT_STREQ(qPrintable(rn.namespaces()[1]), "foo");
    EXPECT_STREQ(qPrintable(rn.namespaceName()), "bar/foo");
    EXPECT_EQ(rn.scopes().count(), 0);
    EXPECT_STREQ(qPrintable(rn.scopeName()), "");
    EXPECT_STREQ("", qPrintable(rn.remote()));
}


TEST(RefName, ScopeBranch) {
    const char* sz = "refs/heads/feature/cool";
    Git::RefName rn = Git::RefName(QLatin1String(sz));
    EXPECT_TRUE (rn.isValid());

    EXPECT_TRUE (rn.isBranch());
    EXPECT_FALSE(rn.isSpecial());
    EXPECT_FALSE(rn.isTag());
    EXPECT_FALSE(rn.isCommitNote());
    EXPECT_FALSE(rn.isHead());
    EXPECT_FALSE(rn.isMergeHead());
    EXPECT_FALSE(rn.isNamespaced());
    EXPECT_TRUE (rn.isScoped());
    EXPECT_FALSE(rn.isPeculiar());
    EXPECT_FALSE(rn.isStage());
    EXPECT_FALSE(rn.isCustom());
    EXPECT_STREQ(qPrintable(rn.name()), "cool");
    EXPECT_STREQ(qPrintable(rn.branchName()), "feature/cool");
    EXPECT_EQ(rn.namespaces().count(), 0);
    ASSERT_EQ(rn.scopes().count(), 1);
    EXPECT_STREQ(qPrintable(rn.scopes()[0]), "feature");
    EXPECT_STREQ(qPrintable(rn.scopeName()), "feature");
    EXPECT_STREQ(qPrintable(rn.namespaceName()), "");
    EXPECT_STREQ("", qPrintable(rn.remote()));
}

TEST(RefName, NestedScopeBranch) {
    const char* sz = "refs/heads/feature/new/cool";
    Git::RefName rn = Git::RefName(QLatin1String(sz));
    EXPECT_TRUE (rn.isValid());

    EXPECT_TRUE (rn.isBranch());
    EXPECT_FALSE(rn.isSpecial());
    EXPECT_FALSE(rn.isTag());
    EXPECT_FALSE(rn.isCommitNote());
    EXPECT_FALSE(rn.isHead());
    EXPECT_FALSE(rn.isMergeHead());
    EXPECT_FALSE(rn.isNamespaced());
    EXPECT_TRUE (rn.isScoped());
    EXPECT_FALSE(rn.isPeculiar());
    EXPECT_FALSE(rn.isStage());
    EXPECT_FALSE(rn.isCustom());
    EXPECT_STREQ(qPrintable(rn.name()), "cool");
    EXPECT_STREQ(qPrintable(rn.branchName()), "feature/new/cool");
    ASSERT_EQ(2, rn.scopes().count());
    EXPECT_STREQ(qPrintable(rn.scopes()[0]), "feature");
    EXPECT_STREQ(qPrintable(rn.scopes()[1]), "new");
    EXPECT_STREQ(qPrintable(rn.scopeName()), "feature/new");
    EXPECT_EQ(0, rn.namespaces().count());
    EXPECT_STREQ(qPrintable(rn.namespaceName()), "");
    EXPECT_STREQ("", qPrintable(rn.remote()));
}

TEST(RefName, ScopedTag) {
    const char* sz = "refs/tags/feature/cool";
    Git::RefName rn = Git::RefName(QLatin1String(sz));
    EXPECT_TRUE (rn.isValid());

    EXPECT_FALSE(rn.isBranch());
    EXPECT_FALSE(rn.isSpecial());
    EXPECT_TRUE (rn.isTag());
    EXPECT_FALSE(rn.isCommitNote());
    EXPECT_FALSE(rn.isHead());
    EXPECT_FALSE(rn.isMergeHead());
    EXPECT_FALSE(rn.isNamespaced());
    EXPECT_TRUE (rn.isScoped());
    EXPECT_FALSE(rn.isPeculiar());
    EXPECT_FALSE(rn.isStage());
    EXPECT_FALSE(rn.isCustom());
    EXPECT_STREQ(qPrintable(rn.name()), "cool");
    EXPECT_STREQ(qPrintable(rn.branchName()), "");
    EXPECT_STREQ(qPrintable(rn.tagName()), "feature/cool");
    EXPECT_EQ(rn.namespaces().count(), 0);
    ASSERT_EQ(rn.scopes().count(), 1);
    EXPECT_STREQ(qPrintable(rn.scopes()[0]), "feature");
    EXPECT_STREQ(qPrintable(rn.scopeName()), "feature");
    EXPECT_STREQ(qPrintable(rn.namespaceName()), "");
    EXPECT_STREQ("", qPrintable(rn.remote()));
}

TEST(RefName, NestedScopedTag) {
    const char* sz = "refs/tags/feature/new/cool";
    Git::RefName rn = Git::RefName(QLatin1String(sz));
    EXPECT_TRUE (rn.isValid());

    EXPECT_FALSE(rn.isBranch());
    EXPECT_FALSE(rn.isSpecial());
    EXPECT_TRUE (rn.isTag());
    EXPECT_FALSE(rn.isCommitNote());
    EXPECT_FALSE(rn.isHead());
    EXPECT_FALSE(rn.isMergeHead());
    EXPECT_FALSE(rn.isNamespaced());
    EXPECT_TRUE (rn.isScoped());
    EXPECT_FALSE(rn.isPeculiar());
    EXPECT_FALSE(rn.isStage());
    EXPECT_FALSE(rn.isCustom());
    EXPECT_STREQ(qPrintable(rn.name()), "cool");
    EXPECT_STREQ(qPrintable(rn.branchName()), "");
    EXPECT_STREQ(qPrintable(rn.tagName()), "feature/new/cool");
    ASSERT_EQ(2, rn.scopes().count());
    EXPECT_STREQ(qPrintable(rn.scopes()[0]), "feature");
    EXPECT_STREQ(qPrintable(rn.scopes()[1]), "new");
    EXPECT_STREQ(qPrintable(rn.scopeName()), "feature/new");
    EXPECT_STREQ(qPrintable(rn.namespaceName()), "");
    EXPECT_EQ(0, rn.namespaces().count());
    EXPECT_STREQ("", qPrintable(rn.remote()));
}

TEST(RefName, Specials_Stage) {
    const char* sz = "refs/stage";

    Git::RefName rn = Git::RefName(QLatin1String(sz));
    EXPECT_TRUE (rn.isValid());

    EXPECT_FALSE(rn.isBranch());
    EXPECT_TRUE (rn.isSpecial());
    EXPECT_FALSE(rn.isTag());
    EXPECT_FALSE(rn.isCommitNote());
    EXPECT_FALSE(rn.isHead());
    EXPECT_FALSE(rn.isMergeHead());
    EXPECT_FALSE(rn.isNamespaced());
    EXPECT_FALSE(rn.isScoped());
    EXPECT_FALSE(rn.isPeculiar());
    EXPECT_TRUE (rn.isStage());
    EXPECT_FALSE(rn.isCustom());
    EXPECT_STREQ(qPrintable(rn.name()), "");
    EXPECT_STREQ(qPrintable(rn.branchName()), "");
    EXPECT_STREQ(qPrintable(rn.tagName()), "");
    EXPECT_STREQ(qPrintable(rn.scopeName()), "");
    EXPECT_STREQ(qPrintable(rn.namespaceName()), "");
    ASSERT_EQ(0, rn.scopes().count());
    EXPECT_EQ(0, rn.namespaces().count());
    EXPECT_STREQ("", qPrintable(rn.remote()));
}

TEST(RefName, Specials_Head) {
    const char* sz = "HEAD";

    Git::RefName rn = Git::RefName(QLatin1String(sz));
    EXPECT_TRUE (rn.isValid());

    EXPECT_FALSE(rn.isBranch());
    EXPECT_TRUE (rn.isSpecial());
    EXPECT_FALSE(rn.isTag());
    EXPECT_FALSE(rn.isCommitNote());
    EXPECT_TRUE (rn.isHead());
    EXPECT_FALSE(rn.isMergeHead());
    EXPECT_FALSE(rn.isNamespaced());
    EXPECT_FALSE(rn.isScoped());
    EXPECT_FALSE(rn.isPeculiar());
    EXPECT_FALSE(rn.isStage());
    EXPECT_FALSE(rn.isCustom());
    EXPECT_STREQ(qPrintable(rn.name()), "");
    EXPECT_STREQ(qPrintable(rn.branchName()), "");
    EXPECT_STREQ(qPrintable(rn.tagName()), "");
    EXPECT_STREQ(qPrintable(rn.scopeName()), "");
    EXPECT_STREQ(qPrintable(rn.namespaceName()), "");
    ASSERT_EQ(0, rn.scopes().count());
    EXPECT_EQ(0, rn.namespaces().count());
    EXPECT_STREQ("", qPrintable(rn.remote()));
}

TEST(RefName, Specials_MergeHead) {
    const char* sz = "MERGE_HEAD";

    Git::RefName rn = Git::RefName(QLatin1String(sz));
    EXPECT_TRUE (rn.isValid());

    EXPECT_FALSE(rn.isBranch());
    EXPECT_TRUE (rn.isSpecial());
    EXPECT_FALSE(rn.isTag());
    EXPECT_FALSE(rn.isCommitNote());
    EXPECT_FALSE(rn.isHead());
    EXPECT_TRUE (rn.isMergeHead());
    EXPECT_FALSE(rn.isNamespaced());
    EXPECT_FALSE(rn.isScoped());
    EXPECT_FALSE(rn.isPeculiar());
    EXPECT_FALSE(rn.isStage());
    EXPECT_FALSE(rn.isCustom());
    EXPECT_STREQ(qPrintable(rn.name()), "");
    EXPECT_STREQ(qPrintable(rn.branchName()), "");
    EXPECT_STREQ(qPrintable(rn.tagName()), "");
    EXPECT_STREQ(qPrintable(rn.scopeName()), "");
    EXPECT_STREQ(qPrintable(rn.namespaceName()), "");
    ASSERT_EQ(0, rn.scopes().count());
    EXPECT_EQ(0, rn.namespaces().count());
    EXPECT_STREQ("", qPrintable(rn.remote()));
}

TEST(RefName, Specials_CommitNotes) {
    const char* sz = "refs/notes/commit";

    Git::RefName rn = Git::RefName(QLatin1String(sz));
    EXPECT_TRUE (rn.isValid());

    EXPECT_FALSE(rn.isBranch());
    EXPECT_TRUE (rn.isSpecial());
    EXPECT_FALSE(rn.isTag());
    EXPECT_TRUE (rn.isCommitNote());
    EXPECT_FALSE(rn.isHead());
    EXPECT_FALSE(rn.isMergeHead());
    EXPECT_FALSE(rn.isNamespaced());
    EXPECT_FALSE(rn.isScoped());
    EXPECT_FALSE(rn.isPeculiar());
    EXPECT_FALSE(rn.isStage());
    EXPECT_FALSE(rn.isCustom());
    EXPECT_STREQ(qPrintable(rn.name()), "");
    EXPECT_STREQ(qPrintable(rn.branchName()), "");
    EXPECT_STREQ(qPrintable(rn.tagName()), "");
    EXPECT_STREQ(qPrintable(rn.scopeName()), "");
    EXPECT_STREQ(qPrintable(rn.namespaceName()), "");
    ASSERT_EQ(0, rn.scopes().count());
    EXPECT_EQ(0, rn.namespaces().count());
    EXPECT_STREQ("", qPrintable(rn.remote()));
}


TEST(RefName, Peculiars) {
    const char* sz = "refs/foobar/commit";

    Git::RefName rn = Git::RefName(QLatin1String(sz));
    EXPECT_TRUE (rn.isValid());

    EXPECT_FALSE(rn.isBranch());
    EXPECT_FALSE(rn.isSpecial());
    EXPECT_FALSE(rn.isTag());
    EXPECT_FALSE(rn.isCommitNote());
    EXPECT_FALSE(rn.isHead());
    EXPECT_FALSE(rn.isMergeHead());
    EXPECT_FALSE(rn.isNamespaced());
    EXPECT_FALSE(rn.isScoped());
    EXPECT_TRUE (rn.isPeculiar());
    EXPECT_FALSE(rn.isStage());
    EXPECT_FALSE(rn.isCustom());
    EXPECT_STREQ(qPrintable(rn.name()), "");
    EXPECT_STREQ(qPrintable(rn.branchName()), "");
    EXPECT_STREQ(qPrintable(rn.tagName()), "");
    EXPECT_STREQ(qPrintable(rn.scopeName()), "");
    EXPECT_STREQ(qPrintable(rn.namespaceName()), "");
    ASSERT_EQ(0, rn.scopes().count());
    EXPECT_EQ(0, rn.namespaces().count());
    EXPECT_STREQ("", qPrintable(rn.remote()));
}

TEST(RefName, RemoteBranch) {
    const char* sz = "refs/remotes/home/new/branch";

    Git::RefName rn = Git::RefName(QLatin1String(sz));
    EXPECT_TRUE (rn.isValid());

    EXPECT_TRUE (rn.isBranch());
    EXPECT_FALSE(rn.isSpecial());
    EXPECT_FALSE(rn.isTag());
    EXPECT_FALSE(rn.isCommitNote());
    EXPECT_FALSE(rn.isHead());
    EXPECT_FALSE(rn.isMergeHead());
    EXPECT_FALSE(rn.isNamespaced());
    EXPECT_TRUE (rn.isScoped());
    EXPECT_FALSE(rn.isPeculiar());
    EXPECT_FALSE(rn.isStage());
    EXPECT_FALSE(rn.isCustom());
    EXPECT_STREQ(qPrintable(rn.name()), "branch");
    EXPECT_STREQ(qPrintable(rn.branchName()), "new/branch");
    EXPECT_STREQ(qPrintable(rn.tagName()), "");
    EXPECT_STREQ(qPrintable(rn.scopeName()), "new");
    EXPECT_STREQ(qPrintable(rn.namespaceName()), "");
    ASSERT_EQ(1, rn.scopes().count());
    EXPECT_STREQ(qPrintable(rn.scopes()[0]), "new");
    EXPECT_EQ(0, rn.namespaces().count());
    EXPECT_STREQ("home", qPrintable(rn.remote()));
}

TEST(RefName, CustomRule) {
    const char* sz = "refs/pull/550/head";
    const char* szNeg = "refs/heads/master";

    QRegExp re(QLatin1String("^refs\\/pull\\/([1-9][0-9]*)\\/head$"));
    int id = Git::RefName::registerExpression((void*)0xAABBCCDD, re);

    Git::RefName rn = Git::RefName(QLatin1String(sz));
    EXPECT_TRUE (rn.isValid());

    EXPECT_FALSE(rn.isBranch());
    EXPECT_FALSE(rn.isSpecial());
    EXPECT_FALSE(rn.isTag());
    EXPECT_FALSE(rn.isCommitNote());
    EXPECT_FALSE(rn.isHead());
    EXPECT_FALSE(rn.isMergeHead());
    EXPECT_FALSE(rn.isNamespaced());
    EXPECT_FALSE(rn.isScoped());
    EXPECT_FALSE(rn.isPeculiar());
    EXPECT_FALSE(rn.isStage());
    EXPECT_TRUE (rn.isCustom());
    EXPECT_TRUE (rn.matchesCustomRule(id));
    EXPECT_STREQ(qPrintable(rn.name()), "");
    EXPECT_STREQ(qPrintable(rn.branchName()), "");
    EXPECT_STREQ(qPrintable(rn.tagName()), "");
    EXPECT_STREQ(qPrintable(rn.scopeName()), "");
    EXPECT_STREQ(qPrintable(rn.namespaceName()), "");
    ASSERT_EQ(0, rn.scopes().count());
    EXPECT_EQ(0, rn.namespaces().count());
    EXPECT_STREQ("", qPrintable(rn.remote()));

    rn = Git::RefName(QLatin1String(szNeg));
    EXPECT_TRUE (rn.isValid());
    EXPECT_FALSE(rn.isCustom());
    EXPECT_FALSE(rn.matchesCustomRule(id));

    ASSERT_EQ(Git::RefName::expressionData(id), (void*)0xAABBCCDD);

    Git::RefName::unregisterExpression(id);

    ASSERT_NE(Git::RefName::expressionData(id), (void*)0xAABBCCDD);
}

