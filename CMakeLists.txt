cmake_minimum_required(VERSION 3.21)

project(
  LibSinex
  VERSION 1.1.0
  DESCRIPTION ""
  LANGUAGES CXX
)

find_package(Eigen3 3.3 REQUIRED)
find_package(datetime 1 REQUIRED)
find_package(geodesy 1 REQUIRED)

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

add_library(sinex)
target_include_directories(sinex PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
  $<INSTALL_INTERFACE:include/sinex>
)

add_subdirectory(src)

# The tests
include(CTest)
add_subdirectory(test)
enable_testing()

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
