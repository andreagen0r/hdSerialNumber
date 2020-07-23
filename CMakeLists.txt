cmake_minimum_required(VERSION 3.17)

project(HDSerialNumber LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_executable(HDSerialNumber main.cpp)

target_link_libraries(HDSerialNumber udev)


set(CONAN_EXTRA_REQUIRES "")
set(CONAN_EXTRA_OPTIONS "")
include(cmake/Conan.cmake)
run_conan()


include_directories(${fmt_INCLUDE_DIRS})
target_link_libraries(HDSerialNumber CONAN_PKG::fmt)


include(CMakePackageConfigHelpers)

include(FeatureSummary)
feature_summary(WHAT ENABLED_FEATURES DISABLED_FEATURES PACKAGES_FOUND)
feature_summary(FILENAME ${CMAKE_CURRENT_BINARY_DIR}/features.log WHAT ALL)
