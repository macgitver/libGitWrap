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

    Git::RefName rn = Git::RefName(QLatin1String("refs/heads/master"));
    EXPECT_TRUE (rn.isValid());

    EXPECT_TRUE (rn.isBranch());
    EXPECT_TRUE (rn.isSpecial());
    EXPECT_FALSE(rn.isTag());
    EXPECT_FALSE(rn.isCommitNote());
    EXPECT_FALSE(rn.isHead());
    EXPECT_FALSE(rn.isMergeHead());
    EXPECT_FALSE(rn.isNamespaced());
    EXPECT_FALSE(rn.isPeculiar());
    EXPECT_FALSE(rn.isStage());

}
