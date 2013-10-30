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

#include "libGitWrap/Result.hpp"
#include "libGitWrap/ObjectId.hpp"
#include "libGitWrap/Repository.hpp"
#include "libGitWrap/Reference.hpp"

#include "Infra/Fixture.hpp"
#include "Infra/TempRepo.hpp"

typedef Fixture ReferenceFixture;

TEST_F(ReferenceFixture, CanLookup)
{
    TempRepoOpener tempRepo(this, "SimpleRepo1");
    Git::Repository repo(tempRepo);
    ASSERT_TRUE(repo.isValid());

    Git::Result r;
    Git::Reference ref = repo.lookupRef(r, QLatin1String("refs/heads/master"));
    ASSERT_TRUE(r);
    ASSERT_TRUE(ref.isValid());
    ASSERT_FALSE(ref.wasDestroyed());

    EXPECT_TRUE(ref.isCurrentBranch());
    EXPECT_TRUE(ref.isLocal());
    EXPECT_FALSE(ref.isRemote());

    EXPECT_STREQ("refs/heads/master", qPrintable(ref.name()));

    ref = repo.lookupRef(r, QLatin1String("master"));
    ASSERT_FALSE(r);
    ASSERT_FALSE(ref.isValid());
    r.clear();
}

TEST_F(ReferenceFixture, CanLookupShorthand)
{
    TempRepoOpener tempRepo(this, "SimpleRepo1");
    Git::Repository repo(tempRepo);
    ASSERT_TRUE(repo.isValid());

    Git::Result r;
    Git::Reference ref = repo.lookupRef(r, QLatin1String("master"), true);
    ASSERT_TRUE(r);
    ASSERT_TRUE(ref.isValid());
    ASSERT_FALSE(ref.wasDestroyed());

    EXPECT_TRUE(ref.isCurrentBranch());
    EXPECT_TRUE(ref.isLocal());
    EXPECT_FALSE(ref.isRemote());

    EXPECT_STREQ("refs/heads/master", qPrintable(ref.name()));
}

TEST_F(ReferenceFixture, CanDestroyRef)
{
    TempRepoOpener tempRepo(this, "SimpleRepo1");
    Git::Repository repo(tempRepo);
    ASSERT_TRUE(repo.isValid());

    Git::Result r;
    Git::Reference ref = repo.lookupRef(r, QLatin1String("master"), true);
    ASSERT_FALSE(ref.wasDestroyed());

    ref.destroy(r);
    ASSERT_TRUE(r);
    ASSERT_TRUE(ref.wasDestroyed());

    ref.objectId(r);
    ASSERT_FALSE(r);
    r.clear();

    // counter check
    Git::Repository repo2 = repo.reopen(r);
    ASSERT_TRUE(r);

    QStringList sl = repo2.allBranchNames(r);
    ASSERT_TRUE(r);
    ASSERT_EQ(0, sl.count());
}
