PROJECT(MGV_GITWRAP_TESTS)

ADD_DEFINITIONS(-DDATADIR=\"${CMAKE_CURRENT_LIST_DIR}/Data\")

QT_PREPARE(Core -Gui)

SET(SRC_FILES

    Infra/main.cpp
    Infra/TempRepo.cpp
    Infra/TempDirProvider.cpp
    Infra/Fixture.cpp

    TestCommit.cpp

    TestIndex.cpp
    TestRepository.cpp
    TestRefName.cpp
    TestReference.cpp
    TestTag.cpp
)

SET(HDR_FILES

    Infra/TempRepo.hpp
    Infra/TempDirProvider.hpp
    Infra/Fixture.hpp
)

INCLUDE_DIRECTORIES(
    ${libGitWrap_includes}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

QT_MOC( MOC_FILES ${HDR_FILES} )

ADD_CUSTOM_GTEST(
    testGitWrap

    ${SRC_FILES}
    ${HDR_FILES}
    ${MOC_FILES}
)

TARGET_LINK_LIBRARIES(
    testGitWrap

    LINK_PRIVATE
        GitWrap
)

ADVANCE_TO_QT_PROJECT(testGitWrap)
