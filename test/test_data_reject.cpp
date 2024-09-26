#include "datetime/datetime_write.hpp"
#include "sinex.hpp"
#include <iostream>
#include <vector>

/*
 * Test sites:
 * DIOA, DIOB, DIOC, MALB
 *
 * Test Sinex:
 * dpod2020_023.snx
 */

int main(int argc, char *argv[]) {

  if (argc < 3) {
    fprintf(stderr, "Usage: %s [SINEX_FILE] [SITE1 ... SITEN]\n", argv[0]);
    return 1;
  }

  /* place sites in a vector */
  std::vector<const char *> sites;
  for (int i = 2; i < argc; i++) {
    sites.push_back(argv[i]);
  }

  /* create the sinex instance */
  dso::Sinex snx(argv[1]);

  /* a vector of SiteId's to hold (intermediate) results */
  std::vector<dso::sinex::SiteId> siteids;

  /* parse the block SITE/ID to collect info for the given sites */
  if (snx.parse_block_site_id(sites, /*use domes=*/false, siteids)) {
    fprintf(stderr, "ERROR. Failed matching sites in SINEX file\n");
    return 1;
  }

  /* store results here */
  std::vector<dso::sinex::DataReject> rej;

  {
    /* random datetime interval; collect data rejection periods that fall within
     * this interval
     */
    const auto t1 = dso::datetime<dso::nanoseconds>(
        dso::year(2020), dso::month(1), dso::day_of_month(1),
        dso::nanoseconds(0));
    const auto t2 = dso::datetime<dso::nanoseconds>(
        dso::year(2020), dso::month(2), dso::day_of_month(1),
        dso::nanoseconds(0));
    /* get data rejection info */
    if (snx.parse_block_data_reject(siteids, rej, t1, t2)) {
      fprintf(stderr, "Failed collecting data rejection info\n");
      return 1;
    }
    /* report results */
    char b1[64], b2[64];
    for (const auto &d : rej) {
      dso::to_char<dso::YMDFormat::YYYYMMDD, dso::HMSFormat::HHMMSSF>(d.start,
                                                                      b1);
      dso::to_char<dso::YMDFormat::YYYYMMDD, dso::HMSFormat::HHMMSSF>(d.stop,
                                                                      b2);
      printf("%s %s %s %s\n", d.site_code(), b1, b2, d.comment());
      assert(rej.size() == 0);
      /* should print nothing! */
    }
  }

  {
    /* datetime interval; DIOA is rejected within this period */
    const auto t1 = dso::datetime<dso::nanoseconds>(
        dso::year(2005), dso::day_of_year(349), dso::nanoseconds(0));
    const auto t2 = dso::datetime<dso::nanoseconds>(
        dso::year(2005), dso::day_of_year(351), dso::nanoseconds(0));
    /* get data rejection info t1=15Dec2005, t2=17Dec2005*/
    if (snx.parse_block_data_reject(siteids, rej, t1, t2)) {
      fprintf(stderr, "Failed collecting data rejection info\n");
      return 1;
    }
    /* report results */
    char b1[64], b2[64];
    for (const auto &d : rej) {
      dso::to_char<dso::YMDFormat::YYYYMMDD, dso::HMSFormat::HHMMSSF>(d.start,
                                                                      b1);
      dso::to_char<dso::YMDFormat::YYYYMMDD, dso::HMSFormat::HHMMSSF>(d.stop,
                                                                      b2);
      printf("%s %s %s %s\n", d.site_code(), b1, b2, d.comment());
      /* should resolve this line:
       * DIOA  A    1 D 05:349:00000 06:136:86399 X - Transmission stopped
       * and print
       * DIOA 2005/12/15 00:00:00.000000000 2006/05/16 23:59:59.000000000 Transmission stopped
       */
      assert(rej.size() == 1);
    }
  }

  {
    /* datetime interval; DIOA is rejected from start of this period to
     * 06:136:86399
     */
    const auto t1 = dso::datetime<dso::nanoseconds>(
        dso::year(2005), dso::day_of_year(350), dso::nanoseconds(0));
    /* get data rejection info */
    if (snx.parse_block_data_reject(siteids, rej, t1)) {
      fprintf(stderr, "Failed collecting data rejection info\n");
      return 1;
    }
    /* report results */
    char b1[64], b2[64];
    for (const auto &d : rej) {
      dso::to_char<dso::YMDFormat::YYYYMMDD, dso::HMSFormat::HHMMSSF>(d.start,
                                                                      b1);
      dso::to_char<dso::YMDFormat::YYYYMMDD, dso::HMSFormat::HHMMSSF>(d.stop,
                                                                      b2);
      printf("%s %s %s %s\n", d.site_code(), b1, b2, d.comment());
      /* should resolve this line:
       * DIOA  A    1 D 05:349:00000 06:136:86399 X - Transmission stopped
       * and print
       * DIOA 2005/12/15 00:00:00.000000000 2006/05/16 23:59:59.000000000 Transmission stopped
       * MALB 2009/02/01 00:00:00.000000000 2009/04/05 23:59:59.000000000 Corrupted data 20090201-20090405
       * MALB 2012/04/15 00:00:00.000000000 2012/05/14 23:59:59.000000000 Electric pb 20120415-20120514
       */
      assert(rej.size() == 3);
    }
  }
  
  {
    /* datetime interval; DIOA is rejected from start of this period to
     * 06:136:86399
     */
    const auto t1 = dso::datetime<dso::nanoseconds>(
        dso::year(2006), dso::day_of_year(137), dso::nanoseconds(0));
    /* get data rejection info */
    if (snx.parse_block_data_reject(siteids, rej, t1)) {
      fprintf(stderr, "Failed collecting data rejection info\n");
      return 1;
    }
    /* report results */
    char b1[64], b2[64];
    for (const auto &d : rej) {
      dso::to_char<dso::YMDFormat::YYYYMMDD, dso::HMSFormat::HHMMSSF>(d.start,
                                                                      b1);
      dso::to_char<dso::YMDFormat::YYYYMMDD, dso::HMSFormat::HHMMSSF>(d.stop,
                                                                      b2);
      printf("%s %s %s %s\n", d.site_code(), b1, b2, d.comment());
      /* should print:
       * MALB 2009/02/01 00:00:00.000000000 2009/04/05 23:59:59.000000000 Corrupted data 20090201-20090405
       * MALB 2012/04/15 00:00:00.000000000 2012/05/14 23:59:59.000000000 Electric pb 20120415-20120514
       */
      assert(rej.size() == 2);
    }
  }

  {
    /* datetime interval; DIOA is rejected only at the last day of the given
     * interval
     */
    const auto t2 = dso::datetime<dso::nanoseconds>(
        dso::year(2005), dso::day_of_year(350), dso::nanoseconds(0));
    /* get data rejection info */
    if (snx.parse_block_data_reject(
            siteids, rej, dso::datetime<dso::nanoseconds>::min(), t2)) {
      fprintf(stderr, "Failed collecting data rejection info\n");
      return 1;
    }
    /* report results */
    char b1[64], b2[64];
    for (const auto &d : rej) {
      dso::to_char<dso::YMDFormat::YYYYMMDD, dso::HMSFormat::HHMMSSF>(d.start,
                                                                      b1);
      dso::to_char<dso::YMDFormat::YYYYMMDD, dso::HMSFormat::HHMMSSF>(d.stop,
                                                                      b2);
      printf("%s %s %s %s\n", d.site_code(), b1, b2, d.comment());
      /* should resolve this line:
       * DIOA  A    1 D 05:349:00000 06:136:86399 X - Transmission stopped
       * and print
       * DIOA 2005/12/15 00:00:00.000000000 2006/05/16 23:59:59.000000000 Transmission stopped
       */
      assert(rej.size() == 1);
    }
  }
  
  {
    /* datetime interval; DIOA is rejected only at the last day of the given
     * interval
     */
    const auto t2 = dso::datetime<dso::nanoseconds>(
        dso::year(2005), dso::day_of_year(348), dso::nanoseconds(86399000000000L));
    /* get data rejection info */
    if (snx.parse_block_data_reject(
            siteids, rej, dso::datetime<dso::nanoseconds>::min(), t2)) {
      fprintf(stderr, "Failed collecting data rejection info\n");
      return 1;
    }
    /* report results */
    char b1[64], b2[64];
    for (const auto &d : rej) {
      dso::to_char<dso::YMDFormat::YYYYMMDD, dso::HMSFormat::HHMMSSF>(d.start,
                                                                      b1);
      dso::to_char<dso::YMDFormat::YYYYMMDD, dso::HMSFormat::HHMMSSF>(d.stop,
                                                                      b2);
      printf("%s %s %s %s\n", d.site_code(), b1, b2, d.comment());
      assert(rej.size() == 0);
    }
  }
  
  {
    /* Get the whole list of rejection periods for given sites */
    /* get data rejection info */
    if (snx.parse_block_data_reject(
            siteids, rej)) {
      fprintf(stderr, "Failed collecting data rejection info\n");
      return 1;
    }
    /* report results */
    char b1[64], b2[64];
    for (const auto &d : rej) {
      dso::to_char<dso::YMDFormat::YYYYMMDD, dso::HMSFormat::HHMMSSF>(d.start,
                                                                      b1);
      dso::to_char<dso::YMDFormat::YYYYMMDD, dso::HMSFormat::HHMMSSF>(d.stop,
                                                                      b2);
      printf("%s %s %s %s\n", d.site_code(), b1, b2, d.comment());
      assert(rej.size() == 3);
      /* should print:
       * DIOA 2005/12/15 00:00:00.000000000 2006/05/16 23:59:59.000000000 Transmission stopped
       * MALB 2009/02/01 00:00:00.000000000 2009/04/05 23:59:59.000000000 Corrupted data 20090201-20090405
       * MALB 2012/04/15 00:00:00.000000000 2012/05/14 23:59:59.000000000 Electric pb 20120415-20120514
       */
    }
  }
  
  {
    const auto t2 = dso::datetime<dso::nanoseconds>(
        dso::year(2012), dso::day_of_year(135), dso::nanoseconds(86399000000000L));
    /* get data rejection info */
    if (snx.parse_block_data_reject(
            siteids, rej, dso::datetime<dso::nanoseconds>::min(), t2)) {
      fprintf(stderr, "Failed collecting data rejection info\n");
      return 1;
    }
    /* report results */
    char b1[64], b2[64];
    for (const auto &d : rej) {
      dso::to_char<dso::YMDFormat::YYYYMMDD, dso::HMSFormat::HHMMSSF>(d.start,
                                                                      b1);
      dso::to_char<dso::YMDFormat::YYYYMMDD, dso::HMSFormat::HHMMSSF>(d.stop,
                                                                      b2);
      printf("%s %s %s %s\n", d.site_code(), b1, b2, d.comment());
      assert(rej.size() == 3);
      /* should print:
       * DIOA 2005/12/15 00:00:00.000000000 2006/05/16 23:59:59.000000000 Transmission stopped
       * MALB 2009/02/01 00:00:00.000000000 2009/04/05 23:59:59.000000000 Corrupted data 20090201-20090405
       * MALB 2012/04/15 00:00:00.000000000 2012/05/14 23:59:59.000000000 Electric pb 20120415-20120514
       */
    }
  }
  
  {
    const auto t2 = dso::datetime<dso::nanoseconds>(
        dso::year(2012), dso::day_of_year(105), dso::nanoseconds(86399000000000L));
    /* get data rejection info */
    if (snx.parse_block_data_reject(
            siteids, rej,dso::datetime<dso::nanoseconds>::min(), t2)) {
      fprintf(stderr, "Failed collecting data rejection info\n");
      return 1;
    }
    /* report results */
    char b1[64], b2[64];
    for (const auto &d : rej) {
      dso::to_char<dso::YMDFormat::YYYYMMDD, dso::HMSFormat::HHMMSSF>(d.start,
                                                                      b1);
      dso::to_char<dso::YMDFormat::YYYYMMDD, dso::HMSFormat::HHMMSSF>(d.stop,
                                                                      b2);
      printf("%s %s %s %s\n", d.site_code(), b1, b2, d.comment());
      assert(rej.size() == 2);
      /* should print:
       * DIOA 2005/12/15 00:00:00.000000000 2006/05/16 23:59:59.000000000 Transmission stopped
       * MALB 2009/02/01 00:00:00.000000000 2009/04/05 23:59:59.000000000 Corrupted data 20090201-20090405
       */
    }
  }
  
  {
    const auto t1 = dso::datetime<dso::nanoseconds>(
        dso::year(2009), dso::day_of_year(94), dso::nanoseconds(86399000000000L));
    const auto t2 = dso::datetime<dso::nanoseconds>(
        dso::year(2009), dso::day_of_year(95), dso::nanoseconds(86399000000000L));
    /* get data rejection info */
    if (snx.parse_block_data_reject(
            siteids, rej, t1, t2)) {
      fprintf(stderr, "Failed collecting data rejection info\n");
      return 1;
    }
    /* report results */
    char b1[64], b2[64];
    for (const auto &d : rej) {
      dso::to_char<dso::YMDFormat::YYYYMMDD, dso::HMSFormat::HHMMSSF>(d.start,
                                                                      b1);
      dso::to_char<dso::YMDFormat::YYYYMMDD, dso::HMSFormat::HHMMSSF>(d.stop,
                                                                      b2);
      printf("%s %s %s %s\n", d.site_code(), b1, b2, d.comment());
      assert(rej.size() == 1);
      /* should print:
       * MALB 2009/02/01 00:00:00.000000000 2009/04/05 23:59:59.000000000 Corrupted data 20090201-20090405
       */
    }
  }


  return 0;
}
