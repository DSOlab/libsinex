#include "sinex.hpp"
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

  // Using Policy = Strict for comparisson
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

  for (auto const &str : sv1) {
    if (!sinex::parameter_type_exists(str)) {
      fprintf(stderr, "[ERROR] Failed to recognize parameter type \"%s\"\n",
              str);
      fprintf(stderr, "Policy = strict\n");
      return 1;
    }
  }

  for (auto const &str : sv2) {
    if (sinex::parameter_type_exists(str)) {
      fprintf(stderr, "[ERROR] Matched erronuous parameter type \"%s\"\n", str);
      fprintf(stderr, "Policy = strict\n");
      return 1;
    }
  }

  // Using Policy = NonStrict for comparisson
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

  for (auto const &str : sv1) {
    if (!sinex::parameter_type_exists<
            sinex::ParameterMatchPolicyType::NonStrict>(str)) {
      fprintf(stderr, "[ERROR] Failed to recognize parameter type \"%s\"\n",
              str);
      fprintf(stderr, "Policy = non-strict\n");
      return 2;
    }
  }

  for (auto const &str : sv2) {
    if (sinex::parameter_type_exists<
            sinex::ParameterMatchPolicyType::NonStrict>(str)) {
      fprintf(stderr, "[ERROR] Matched erronuous parameter type \"%s\"\n", str);
      fprintf(stderr, "Policy = non-strict\n");
      return 2;
    }
  }

  printf("All seem ok!\n");
  return 0;
}