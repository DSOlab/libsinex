#ifndef __SINEX_FILE_PARSER_HPP__
#define __SINEX_FILE_PARSER_HPP__

#include "sinex_details.hpp"
#include <fstream>
#include <vector>
#ifdef DEBUG
#include "ggdatetime/datetime_write.hpp"
#include "geodesy/units.hpp"
#include <cstdio>
#endif

namespace dso {

namespace sinex {
/// @class SinexBlockPosition A data block within a SINEX file
struct SinexBlockPosition {
  std::ifstream::pos_type mpos; ///< position from file begining
  const char *mtype;            ///< block description
};                              // SinexBlockPosition

struct SiteId {
  char m_site_code[5] = {'\0'};
  char m_point_code[3] = {'\0'};
  char m_domes[10] = {'\0'};
  char m_obs_code;
  char m_description[23] = {'\0'};
  double m_lon, m_lat; // radians
  double m_hgt;        // height in m
#ifdef DEBUG
  void print() const noexcept {
    printf("Site Code    %s\n", m_site_code);
    printf("Point Code   %s\n", m_point_code);
    printf("Domes Number %s\n", m_domes);
    printf("Obs. Code    %c\n", m_obs_code);
    printf("Description  %s\n", m_description);
    printf("lat/lon/hgt %+7.1f/%+7.1f/%7.2f\n", dso::rad2deg(m_lon),
           dso::rad2deg(m_lat), m_hgt);
  }
#endif
}; // SiteId

struct SiteReceiver {
  char m_site_code[5] = {'\0'};
  char m_point_code[3] = {'\0'};
  char m_soln_id[5] = {'\0'};
  char m_obs_code;
  char m_rec_type[21] = {'\0'};
  char m_rec_serial[6] = {'\0'};
  char m_rec_firmware[12] = {'\0'};
  dso::datetime<dso::seconds> m_start{}, m_stop{};
#ifdef DEBUG
  void print() const noexcept {
    printf("Site Code    %s\n", m_site_code);
    printf("Point Code   %s\n", m_point_code);
    printf("Solution Id  %s\n", m_soln_id);
    printf("Obs. Code    %c\n", m_obs_code);
    printf("Rec. Type    %s\n", m_rec_type);
    printf("Rec. Serial  %s\n", m_rec_serial);
    printf("Rec. Firmw   %s\n", m_rec_firmware);
    printf("From %s to %s\n", dso::strftime_ymd_hmfs(m_start).c_str(),
           dso::strftime_ymd_hmfs(m_stop).c_str());
  }
#endif
}; // SiteReceiver

struct SiteAntenna {
  char m_site_code[5] = {'\0'};
  char m_point_code[3] = {'\0'};
  char m_soln_id[5] = {'\0'};
  char m_obs_code;
  char m_ant_type[21] = {'\0'};
  char m_ant_serial[6] = {'\0'};
  dso::datetime<dso::seconds> m_start{}, m_stop{};
#ifdef DEBUG
  void print() const noexcept {
    printf("Site Code    %s\n", m_site_code);
    printf("Point Code   %s\n", m_point_code);
    printf("Solution Id  %s\n", m_soln_id);
    printf("Obs. Code    %c\n", m_obs_code);
    printf("Ant. Type    %s\n", m_ant_type);
    printf("Ant. Serial  %s\n", m_ant_serial);
    printf("From %s to %s\n", dso::strftime_ymd_hmfs(m_start).c_str(),
           dso::strftime_ymd_hmfs(m_stop).c_str());
  }
#endif
}; // SiteAntenna

struct SolutionEstimate {
  int m_index;
  char m_param_type[7] = {'\0'};
  char m_site_code[5] = {'\0'};
  char m_point_code[3] = {'\0'};
  char m_soln_id[5] = {'\0'};
  char m_units[5] = {'\0'};
  char m_constraint_code = '\0';
  double m_estimate, m_std_deviation;
  dso::datetime<dso::seconds> m_epoch{};

  // int parse_line(const char *line) noexcept;
}; // SolutionEstimate

} // namespace sinex

class Sinex {
private:
  using pos_t = std::ifstream::pos_type;

  std::string m_filename;
  std::ifstream m_stream;
  float m_version;
  char m_agency[4] = {'\0'};
  dso::datetime<dso::seconds> m_created_at, m_data_start, m_data_stop;
  char m_data_agency[4] = {'\0'};
  char m_obs_code, m_constraint_code;
  char m_sol_contents[6] = {'\0'};
  int m_num_estimates;
  std::vector<sinex::SinexBlockPosition> m_blocks;

  std::vector<sinex::SinexBlockPosition>::const_iterator
  find_block(const char *blk) noexcept {
    return std::find_if(m_blocks.cbegin(), m_blocks.cend(),
                        [&](const sinex::SinexBlockPosition &sbp) {
                          return !std::strcmp(sbp.mtype, blk);
                        });
  }

  int goto_block(const char *block) noexcept;

public:
  std::string filename() const noexcept { return m_filename; }

  int parse_first_line() noexcept;
  int mark_blocks() noexcept;

  // parse blocks
  int parse_block_site_id(std::vector<sinex::SiteId> &site_vec,
                          int num_sites_requested = 0,
                          char **sites = nullptr) noexcept;
  
  int parse_block_site_receiver(
      std::vector<sinex::SiteReceiver> &site_vec) noexcept;
  
  int parse_block_site_antenna(
      std::vector<sinex::SiteAntenna> &site_vec) noexcept;
  
  int parse_block_solution_estimate(
      std::vector<sinex::SolutionEstimate> &site_vec) noexcept;
  
  int parse_block_solution_estimate(
      std::vector<sinex::SolutionEstimate> &estimates_vec,
      const std::vector<sinex::SiteId> &sites_vec) noexcept;

  int get_solution_estimate(const char *site_codes[],
                            const dso::datetime<dso::seconds> &t,
                            bool error_if_missing = false) noexcept;

  Sinex(const char *fn);
  Sinex(const Sinex &) = delete;
  Sinex &operator=(const Sinex &) = delete;
  ~Sinex() noexcept {
    if (m_stream.is_open()) m_stream.close();
  }

#ifdef DEBUG
  void print_members() const;
  void print_blocks() const noexcept;
#endif

}; // Sinex

} // namespace dso

#endif //__SINEX_FILE_PARSER_HPP__
