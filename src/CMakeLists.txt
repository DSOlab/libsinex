target_sources(sinex PRIVATE
    ${CMAKE_SOURCE_DIR}/src/extrapolate_coordinates.cpp
    ${CMAKE_SOURCE_DIR}/src/filter_solution_estimates.cpp
    ${CMAKE_SOURCE_DIR}/src/parse_data_reject.cpp
    ${CMAKE_SOURCE_DIR}/src/parse_sinex_date.cpp
    ${CMAKE_SOURCE_DIR}/src/parse_site_antenna.cpp
    ${CMAKE_SOURCE_DIR}/src/parse_site_eccentricity.cpp
    ${CMAKE_SOURCE_DIR}/src/parse_site_id.cpp
    ${CMAKE_SOURCE_DIR}/src/parse_site_receiver.cpp
    ${CMAKE_SOURCE_DIR}/src/parse_solution_epoch.cpp
    ${CMAKE_SOURCE_DIR}/src/parse_solution_estimate.cpp
    ${CMAKE_SOURCE_DIR}/src/sinex.cpp
    ${CMAKE_SOURCE_DIR}/src/sinex_observation_code.cpp
)
