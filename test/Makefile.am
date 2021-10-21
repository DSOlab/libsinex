noinst_PROGRAMS = \
        testSinexReadBlocks.out \
	testSinexBlockSiteId.out \
	testSinexBlockSiteReceiver.out \
        testParameterExists.out \
	testGetSolution.out

MCXXFLAGS = \
	-std=c++17 \
	-g \
	-pg \
	-Wall \
	-Wextra \
	-Werror \
	-pedantic \
	-W \
	-Wshadow \
	-Wdisabled-optimization \
	-DDEBUG

AM_LIBS = -lggdatetime -lggeodesy

testSinexReadBlocks_out_SOURCES   = test_sinex.cpp
testSinexReadBlocks_out_CXXFLAGS  = $(MCXXFLAGS) -I$(top_srcdir)/src 
testSinexReadBlocks_out_LDADD     = $(top_srcdir)/src/libsinex.la $(AM_LIBS)

testSinexBlockSiteId_out_SOURCES   = test_site_id.cpp
testSinexBlockSiteId_out_CXXFLAGS  = $(MCXXFLAGS) -I$(top_srcdir)/src 
testSinexBlockSiteId_out_LDADD     = $(top_srcdir)/src/libsinex.la $(AM_LIBS)

testSinexBlockSiteReceiver_out_SOURCES   = test_site_receiver.cpp
testSinexBlockSiteReceiver_out_CXXFLAGS  = $(MCXXFLAGS) -I$(top_srcdir)/src 
testSinexBlockSiteReceiver_out_LDADD     = $(top_srcdir)/src/libsinex.la $(AM_LIBS)

testParameterExists_out_SOURCES   = test_parameter_exists.cpp
testParameterExists_out_CXXFLAGS  = $(MCXXFLAGS) -I$(top_srcdir)/src 
testParameterExists_out_LDADD     = $(top_srcdir)/src/libsinex.la $(AM_LIBS)

testGetSolution_out_SOURCES   = test_get_solution.cpp
testGetSolution_out_CXXFLAGS  = $(MCXXFLAGS) -I$(top_srcdir)/src 
testGetSolution_out_LDADD     = $(top_srcdir)/src/libsinex.la $(AM_LIBS)
