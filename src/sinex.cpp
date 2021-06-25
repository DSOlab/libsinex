#include "sinex.hpp"
#include <cstdlib>

constexpr int max_chars = 128;

int parse_sinex() {
	char line[max_chars];
	char* start, *end;

	if (!m_stream.open()) return 1;

	m_stream.getline(line, max_chars);

	// first line
	if (!std::strncmp(line, "%=SNX", 5)) {
		fprintf("[ERROR] Invalid first SINEX line\n");
		return 1;
	}
	m_version = std::strtof(line+5, &end);
	if (end==line+5) return 1;
	std::memcpy(m_agency, line+11, 3);
	try {
		m_created_at = parse_snx_date(line+14);
	} catch (std::exception&) {
		return 1;
	}
	std::memcpy(m_data_agency, line+28, 3);
	try {
		m_data_start = parse_snx_date(line+31);
		m_data_stop = parse_snx_date(line+44);
	} catch (std::exception&) {
		return 1;
	}
	*m_obs_code = line[58];
	m_num_estimates = std::strtol(line+60, &end, 10);
	if (end==line+60) return 1;

}