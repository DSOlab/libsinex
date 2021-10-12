lib_LTLIBRARIES = libsinex.la

## Only use the following flags for debuging purposes
libsinex_la_CXXFLAGS = \
	-std=c++17 \
	-g \
	-pg \
	-Wall \
	-Wextra \
	-Werror \
	-pedantic \
	-W \
	-Wshadow \
	-Winline \
	-Wdisabled-optimization \
	-DDEBUG

dist_include_HEADERS = \
	sinex_details.hpp \
        sinex.hpp

dist_libsinex_la_SOURCES = \
        sinex.cpp \
	parse_sinex_date.cpp \
	parse_site_id.cpp \
	parse_site_receiver.cpp \
	parse_site_antenna.cpp \
	parse_solution_estimate.cpp
