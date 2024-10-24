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

add_executable(test_site_id_ndomes test_site_id_ndomes.cpp)
target_link_libraries(test_site_id_ndomes PRIVATE sinex)
find_program(site_id_ndomes.sh NAMES bash REQUIRED)
add_test(NAME site_id_ndomes 
         COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/site_id_ndomes.sh $<TARGET_FILE:test_site_id_ndomes> ${SNX})

add_executable(test_site_id_wdomes test_site_id_wdomes.cpp)
target_link_libraries(test_site_id_wdomes PRIVATE sinex)
find_program(site_id_wdomes.sh NAMES bash REQUIRED)
add_test(NAME site_id_wdomes 
         COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/site_id_wdomes.sh $<TARGET_FILE:test_site_id_ndomes> ${SNX})

add_executable(test_site_antenna test_site_antenna.cpp)
target_link_libraries(test_site_antenna PRIVATE sinex)
find_program(site_antenna.sh NAMES bash REQUIRED)
add_test(NAME site_antenna 
         COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/site_antenna.sh $<TARGET_FILE:test_site_antenna> ${SNX})

add_executable(test_get_solution test_get_solution.cpp)
target_link_libraries(test_get_solution PRIVATE sinex)
find_program(get_solution.sh NAMES bash REQUIRED)
add_test(NAME get_solution
         COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/get_solution.sh $<TARGET_FILE:test_get_solution> ${SNX})

add_executable(test_real_harmonics test_real_harmonics.cpp)
target_link_libraries(test_real_harmonics PRIVATE sinex)
add_test(NAME real_harmonics COMMAND test_real_harmonics)
