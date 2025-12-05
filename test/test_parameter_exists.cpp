#include "sinex.hpp"
#include <cassert>
#include <vector>

using namespace dso;

int main() {

  const char *p1 = "TGETOT";
  const char *p2 = "SN";
  const char *p3 = "STAX";
  const char *p4 = "SAT__Y";

  const char *p5 = "SAT__Yfoobar";
  const char *p6 = "SAT__Y foo bar";
  const char *p7 = "foobarSAT__Y";
  const char *p8 = " SAT__Y";
  const char *p9 = "TGETOTT";
  const char *p10 = "TGGETOT";

  /* Using Policy = Strict for comparisson */
  std::vector<const char *> sv1;
  sv1.emplace_back(p1);
  sv1.emplace_back(p2);
  sv1.emplace_back(p3);
  sv1.emplace_back(p4);

  std::vector<const char *> sv2;
  sv2.emplace_back(p7);
  sv2.emplace_back(p8);
  sv2.emplace_back(p10);
  sv2.emplace_back(p5);
  sv2.emplace_back(p6);
  sv2.emplace_back(p9);

  int index;
  /* should identify all parameters in sv1 (Policy=Strict) */
  for (auto const &str : sv1) {
    if (!sinex::parameter_type_exists(str, index)) {
      fprintf(stderr, "[ERROR] Failed to recognize parameter type \"%s\"\n",
              str);
      fprintf(stderr, "Policy = strict\n");
      assert(1 == 2);
    }
  }

  /* should not identify parameters in sv2 (Policy=Strict) */
  for (auto const &str : sv2) {
    if (sinex::parameter_type_exists(str, index)) {
      fprintf(stderr, "[ERROR] Matched erronuous parameter type \"%s\"\n", str);
      fprintf(stderr, "Policy = strict\n");
      assert(1 == 2);
    }
  }

  /* Using Policy = NonStrict for comparisson */
  sv1.clear();
  sv2.clear();

  sv1.emplace_back(p1);
  sv1.emplace_back(p2);
  sv1.emplace_back(p3);
  sv1.emplace_back(p4);
  sv1.emplace_back(p5);
  sv1.emplace_back(p6);
  sv1.emplace_back(p9);

  sv2.emplace_back(p7);
  sv2.emplace_back(p8);
  sv2.emplace_back(p10);

  /* should identify all parameters in sv1 */
  for (auto const &str : sv1) {
    if (!sinex::parameter_type_exists<
            sinex::details::ParameterMatchPolicyType::NonStrict>(str, index)) {
      fprintf(stderr, "[ERROR] Failed to recognize parameter type \"%s\"\n",
              str);
      fprintf(stderr, "Policy = non-strict\n");
      assert(1 == 2);
    }
  }

  /* should identify all parameters in sv2 */
  for (auto const &str : sv2) {
    if (sinex::parameter_type_exists<
            sinex::details::ParameterMatchPolicyType::NonStrict>(str, index)) {
      fprintf(stderr, "[ERROR] Matched erronuous parameter type \"%s\"\n", str);
      fprintf(stderr, "Policy = non-strict\n");
      assert(1 == 2);
    }
  }

  return 0;
}
