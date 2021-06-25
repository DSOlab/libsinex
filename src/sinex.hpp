#ifndef __SINEX_HPP__
#define __SINEX_HPP__

#include <fstream>
#include "ggdatetime/dtcalendar.hpp"

/// @brief Resolve a string of type YY:DOY:SECOD to a datetime instance
ngpt::datetime<ngpt::seconds> parse_snx_date(cosnt char *str) {
	char *end;
	const char *start = str;
	int iyr = std::strtol(start, &end, 10);
	if (start == end || *end != ':') {
		throw std::runtime_error('[ERROR] Failed resolving datetime in SINEX file (#1).\n');
	}
	iyr += (iry<=50) ? 2000 : 1900;
	start = end + 1;
	int imm = std::strtol(start, &end, 10);
	if (start == end || *end != ':') {
		throw std::runtime_error('[ERROR] Failed resolving datetime in SINEX file (#2).\n');
	}
	start = end + 1;
	long isc = std::strtol(start, &end, 10);
	if (start == end) {
		throw std::runtime_error('[ERROR] Failed resolving datetime in SINEX file (#3).\n');
	}
	return ngpt::datetime<ngpt::seconds>(ngpt::year(iyr), ngpt::month(imm), ngpt::seconds(isc));
}

namespace sinex_details {
	const char *block_names[] = {
		"FILE/REFERENCE ",
		"FILE/COMMENT",
		"INPUT/HISTORY",
		"INPUT/FILES",
		"INPUT/ACKNOWLEDGEMENTS",
		"NUTATION/DATA",
		"PRECESSION/DATA",
		"SOURCE/ID",
		"SITE/ID",
		"SITE/DATA",
		"SITE/RECEIVER",
		"SITE/ANTENNA",
		"SITE/GPS_PHASE_CENTER",
		"SITE/GAL_PHASE_CENTER",
		"SITE/ECCENTRICITY",
		"SATELLITE/ID",
		"SATELLITE/PHASE_CENTER",
		"BIAS/EPOCHS",
		"SOLUTION/EPOCHS",
		"SOLUTION/STATISTICS",
		"SOLUTION/ESTIMATE",
		"SOLUTION/APRIORI",
		"SOLUTION/MATRIX_ESTIMATE L CORR",
		"SOLUTION/MATRIX_ESTIMATE L COVA",
		"SOLUTION/MATRIX_ESTIMATE L INFO",
		"SOLUTION/MATRIX_ESTIMATE U CORR",
		"SOLUTION/MATRIX_ESTIMATE U COVA",
		"SOLUTION/MATRIX_ESTIMATE U INFO",
		"SOLUTION/MATRIX_APRIORI L CORR",
		"SOLUTION/MATRIX_APRIORI L COVA",
		"SOLUTION/MATRIX_APRIORI L INFO",
		"SOLUTION/MATRIX_APRIORI U CORR",
		"SOLUTION/MATRIX_APRIORI U COVA",
		"SOLUTION/MATRIX_APRIORI U INFO",
		"SOLUTION/NORMAL_EQUATION_VECTOR",
		"SOLUTION/NORMAL_EQUATION_MATRIX L",
		"SOLUTION/NORMAL_EQUATION_MATRIX U",
	};
	constexpr int blocks_names_size = sizeof(blocks_names) / sizeof(char*);

	const char* parameter_types[]  = {
          "STAX", //station X coordinate, m
          "STAY", //station Y coordinate, m
          "STAZ", //station Z coordinate, m
          "VELX", //station X velocity, m/y
          "VELY", //station Y velocity, m/y
          "VELZ", //station Z velocity, m/y
          "XGC", //geocenter X coordinate, m
          "YGC", //geocenter Y coordinate, m
          "ZGC", //geocenter Z coordinate, m
          "RS_RA", //radio source right ascension, rad
          "RS_DE", //radio source declin., rad
          "RS_RAR", //radio source right ascension rate, rad/y
          "RS_DER", //radio source declination rate, rad/y 
          "RS_PL", //radio source parallax, rad 
          "LOD", //length of day, ms 
          "UT", //delta time UT1-UTC, ms 
          "XPO", //X polar motion, mas 
          "YPO", //Y polar motion, mas 
          "XPOR", //X polar motion rate, mas/d 
          "YPOR", //Y polar motion rate, mas/d 
          "NUT_LN", //nutation correction in longitude, mas 
          "NUT_OB", //nutation correction in obliquity, mas 
          "NUTRLN", //nutation rate in longitude, mas/d 
          "NUTROB", //nutation rate in obliquity, mas/d 
          "NUT_X", //nutation correction X, mas 
          "NUT_Y", //nutation correction Y, mas 
          "NUTR_X", //nutation rate in X mas/d 
          "NUTR_Y", //nutation rate in Y mas/d 
          "SAT__X", //Satellite X coord., m 
          "SAT__Y", //Satellite Y coord., m 
          "SAT__Z", //Satellite Z coord., m 
          "SAT_VX", //Satellite X velocity, m/s 
          "SAT_VY", //Satellite Y velocity, m/s 
          "SAT_VZ", //Satellite Z velocity, m/s 
          "SAT_RP", //Radiation pressure, 
          "SAT_GX", //GX scale, 
          "SAT_GZ", //GZ scale, 
          "SATYBI", //GY bias, m/s2 
          "TROTOT", //wet + dry Trop. delay, m 
          "TRODRY", //dry Trop. delay, m 
          "TROWET", //wet Trop. delay, m 
          "TGNTOT", //troposphere gradient in north (wet + dry), m 
          "TGNWET", //troposphere gradient in north (only wet), m 
          "TGNDRY", //troposphere gradient in north (only dry), m 
          "TGETOT", //troposphere gradient in east (wet + dry), m 
          "TGEWET", //troposphere gradient in east (only wet), m 
          "TGEDRY", //troposphere gradient in east (only dry), m 
          "RBIAS", //range bias, m 
          "TBIAS", //time bias, ms 
          "SBIAS", //scale bias, ppb 
          "ZBIAS", //troposphere bias at zenith, m 
          "AXI_OF", //VLBI antenna axis offset, m 
          "SATA_Z", //sat. antenna Z offset m 
          "SATA_X", //sat. antenna X offset, m 
          "SATA_Y", //sat. antenna Y offset, m 
          "CN", //spherical harmonic coefficient C_nm 
          "SN" //spherical harmonic coefficient S_nm
	};
        constexpr int parameter_types_size = sizeof(parameter_types) / sizeof(char *);
}// sinex_details

class Sinex {
private:
	std::string m_filename;
	std:ifstream m_stream;
	float m_version;
	char m_agency[4]={'\0'};
	ngpt::datetime<ngpt::seconds> m_created_at, m_data_start, m_data_end;
	char m_data_agency[4]={'\0'};
	char m_obs_code;
	int m_num_estimates;

};// Sinex

#endif //__SINEX_HPP__
