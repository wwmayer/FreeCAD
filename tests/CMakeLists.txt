CMAKE_MINIMUM_REQUIRED(VERSION 3.1)

PROJECT(Google_tests LANGUAGES CXX)


add_definitions(-DGTEST_LANGUAGE_CXX11)

find_package(Threads REQUIRED)

set(GOOGLETEST_DIR "${CMAKE_SOURCE_DIR}/tests")

if (EXISTS ${GOOGLETEST_DIR})
    set(GTestSrc ${GOOGLETEST_DIR}/googletest)
    set(GMockSrc ${GOOGLETEST_DIR}/googlemock)
endif ()


include_directories(${GTestSrc} ${GTestSrc}/include ${GMockSrc} ${GMockSrc}/include)

add_subdirectory(src)
