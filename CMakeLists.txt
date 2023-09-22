cmake_minimum_required(VERSION 3.0.0)
project(MuDis VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

add_executable(normal_usage normal_usage.cpp skiplist.h)
add_executable(stress_test stress_test.cpp skiplist.h)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
