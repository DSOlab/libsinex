add_executable(test_data_reject test_data_reject.cpp)
target_link_libraries(test_data_reject PRIVATE sinex)
find_program(data_reject.sh NAMES bash REQUIRED)
find_file(SNX 
  NAMES "dpod2020_023.snx"
  PATHS ${CMAKE_SOURCE_DIR}/data
  NO_DEFAULT_PATH
  REQUIRED)
add_test(NAME data_reject 
         COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/data_reject.sh $<TARGET_FILE:test_data_reject> ${SNX})

add_executable(test_parameter_exists test_parameter_exists.cpp)
target_link_libraries(test_parameter_exists PRIVATE sinex)
add_test(NAME test_parameter_exists COMMAND test_parameter_exists)

add_executable(test_sinex test_sinex.cpp)
target_link_libraries(test_sinex PRIVATE sinex)
add_test(NAME test_sinex COMMAND test_sinex ${SNX})

add_executable(test_site_eccentricity test_site_eccentricity.cpp)
target_link_libraries(test_site_eccentricity PRIVATE sinex)
find_program(site_eccentricity.sh NAMES bash REQUIRED)
add_test(NAME site_eccentricity 
         COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/site_eccentricity.sh $<TARGET_FILE:test_site_eccentricity> ${SNX})
