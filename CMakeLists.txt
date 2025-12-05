cmake_minimum_required(VERSION 3.21)

project(
  LibSinex
  VERSION 1.1.0
  DESCRIPTION ""
  LANGUAGES CXX
)

find_package(Eigen3   REQUIRED)
find_package(datetime REQUIRED)
find_package(geodesy  REQUIRED)

# Pass the library dependencies to subdirectories
set(PROJECT_DEPENDENCIES Eigen3::Eigen geodesy datetime)

# Enable clang-tidy option
option(ENABLE_CLANG_TIDY "Enable clang-tidy checks" OFF)

# Define an option for building tests (defaults to ON)
option(BUILD_TESTING "Enable building of tests" ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)
add_compile_options(-Wall 
  -Wextra 
  -Werror 
  -pedantic 
  -W 
  -Wshadow 
  $<$<CONFIG:Release>:-O2>
  $<$<CONFIG:Release>:-march=native>
  $<$<CONFIG:Debug>:-g>
  $<$<CONFIG:Debug>:-pg> 
  $<$<CONFIG:Debug>:-Wdisabled-optimization>
  )
add_compile_definitions(
  $<$<CONFIG:Debug>:DEBUG>
)

# clang-tidy (targets that follow will be checked)
if(ENABLE_CLANG_TIDY)
  set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-checks=bugprone-*,\
  clang-analyzer-*,\
  cppcoreguidelines-*,\
  llvm-*,\
  modernize-*,\
  performance-*,\
  -modernize-use-trailing-return-type,\
  -cppcoreguidelines-pro-bounds-pointer-arithmetic,\
  -cppcoreguidelines-pro-bounds-constant-array-index,\
  -cppcoreguidelines-pro-type-vararg")
  message(STATUS "clang-tidy is enabled.")
else()
  message(STATUS "clang-tidy is disabled. To enable it, use -DENABLE_CLANG_TIDY=ON.")
endif()

add_library(sinex)
target_include_directories(sinex PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
  $<INSTALL_INTERFACE:include/sinex>
)

add_subdirectory(src)

# disable clang-tidy (targets that follow will not be checked)
set(CMAKE_CXX_CLANG_TIDY "")

# The tests
if(BUILD_TESTING)
# The tests
  include(CTest)
  add_subdirectory(test)
  enable_testing()
endif()

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/
	DESTINATION include/sinex
)

install(TARGETS sinex
        EXPORT sinexTargets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
)

install(EXPORT sinexTargets
        FILE sinexTargets.cmake
        NAMESPACE dso::
        DESTINATION lib/cmake
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
  "sinexConfigVersion.cmake"
  VERSION ${sinex_version}
  COMPATIBILITY AnyNewerVersion
)

install(FILES "sinexConfig.cmake" "${CMAKE_CURRENT_BINARY_DIR}/sinexConfigVersion.cmake"
        DESTINATION lib/cmake
)
