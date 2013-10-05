
PROJECT(MGV_GITWRAP_TESTS)

QT_PREPARE(Core -Gui)

SET(SRC_FILES

    TestRepository.cpp
    TestRefName.cpp
)

INCLUDE_DIRECTORIES(${libGitWrap_includes})

ADD_QT_GTEST(testGitWrap ${SRC_FILES})

TARGET_LINK_LIBRARIES(testGitWrap GitWrap)
