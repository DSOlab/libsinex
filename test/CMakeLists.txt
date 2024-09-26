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
