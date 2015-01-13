/*
 * MacGitver
 * Copyright (C) 2015 The MacGitver-Developers <dev@macgitver.org>
 *
 * (C) Nils Fenner <nils@macgitver.org>
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

#include "libGitWrap/Commit.hpp"
#include "libGitWrap/ObjectId.hpp"
#include "libGitWrap/Repository.hpp"
#include "libGitWrap/Result.hpp"
#include "libGitWrap/Signature.hpp"
#include "libGitWrap/Tag.hpp"

#include "Infra/Fixture.hpp"
#include "Infra/TempRepo.hpp"

typedef Fixture TagFixture;

TEST_F(TagFixture, CanCreateTag)
{
    Git::Result r;
    Git::Repository repo( TempRepoOpener(this, "SimpleRepo1", r) );
    CHECK_GIT_RESULT( r );
    ASSERT_TRUE( repo.isValid() );

    Git::Signature tagger = Git::Signature::defaultSignature(r, repo);
    CHECK_GIT_RESULT( r );

    // TODO: We need an existing commit to tag. Test will fail!
    Git::Commit target;

    Git::ObjectId tagId = Git::Tag::create( r, QString::fromUtf8("test-tag"),
                                            target, tagger,
                                            QString::fromUtf8("Tag created from test."),
                                            false );
    CHECK_GIT_RESULT( r );
    ASSERT_FALSE( tagId.isNull() );
}

TEST_F(TagFixture, CanCreateLightweightTag)
{
    Git::Result r;
    Git::Repository repo( TempRepoOpener(this, "SimpleRepo1", r) );
    CHECK_GIT_RESULT( r );
    ASSERT_TRUE( repo.isValid() );

    // TODO: We need an existing commit to tag. Test will fail!
    Git::Commit target;

    Git::ObjectId tagId = Git::Tag::createLight( r, QString::fromUtf8("test-lightweight-tag"),
                                                 target, false );
    CHECK_GIT_RESULT( r );
    ASSERT_FALSE( tagId.isNull() );
}
