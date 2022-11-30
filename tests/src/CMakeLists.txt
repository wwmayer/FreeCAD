# Google Test
add_executable(Google_Tests_run main.cpp
               test1.cpp
               test2.cpp
               ${GTestSrc}/src/gtest-all.cc
               ${GMockSrc}/src/gmock-all.cc)
add_test(Google_Tests_run COMMAND Google_Tests_run)
target_link_libraries(Google_Tests_run PRIVATE Threads::Threads)

# Qt Test
include_directories(
    ${QtGui_INCLUDE_DIRS}
    ${QtTest_INCLUDE_DIRS}
)

add_executable(Qt_Tests_run qttestcase.cpp)
add_test(Qt_Tests_run COMMAND Qt_Tests_run)

target_link_libraries(Qt_Tests_run ${QtGui_LIBRARIES} ${QtTest_LIBRARIES})
