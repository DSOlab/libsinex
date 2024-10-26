#include "datetime/calendar.hpp"
#include "site_psd.hpp"
#include <cassert>
#include <random>

using namespace dso;

int main() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distr(0, 12);

    SitePsdModel p1;
    SitePsdModel p2;

  {
    SitePsdModel p;
    for (int i = 0; i < 10; i++) {
      p.add_log_term(MjdEpoch::random(), i*2.1, i*2.2);
      assert(p.num_logarithmic_terms() == i + 1);
      assert(p.log_term_at(i)[0] == i*2.1);
      assert(p.log_term_at(i)[1] == i*2.2);
    }
    p1 = p;
  }

  
  {
    SitePsdModel p;
    for (int i = 0; i < 10; i++) {
      p.add_exp_term(MjdEpoch::random(), i*2.1, i*2.2);
      assert(p.num_exponential_terms() == i + 1);
      assert(p.exp_term_at(i)[0] == i*2.1);
      assert(p.exp_term_at(i)[1] == i*2.2);
    }
    p2 = p;
  }

  /* check p1 */
  {
    for (int i = 0; i < 10; i++) {
      assert(p1.log_term_at(i)[0] == i * 2.1);
      assert(p1.log_term_at(i)[1] == i * 2.2);
    }
  }

  /* add exp terms in p1 */
  {
    for (int i = 0; i < 5; i++) {
      p1.add_exp_term(MjdEpoch::random(), i*2.1, i*2.2);
      assert(p1.num_exponential_terms() == i + 1);
      assert(p1.exp_term_at(i)[0] == i*2.1);
      assert(p1.exp_term_at(i)[1] == i*2.2);
    }
    for (int i = 0; i < 10; i++) {
      assert(p1.log_term_at(i)[0] == i * 2.1);
      assert(p1.log_term_at(i)[1] == i * 2.2);
    }
  }
  
  /* check p2 */
  {
    for (int i = 0; i < 10; i++) {
      assert(p1.exp_term_at(i)[0] == i * 2.1);
      assert(p1.exp_term_at(i)[1] == i * 2.2);
    }
  }

  /* add log terms in p2 */
  {
    for (int i = 0; i < 5; i++) {
      p1.add_log_term(MjdEpoch::random(), i*2.1, i*2.2);
      assert(p1.num_logarithmic_terms() == i + 1);
      assert(p1.log_term_at(i)[0] == i*2.1);
      assert(p1.log_term_at(i)[1] == i*2.2);
    }
    for (int i = 0; i < 10; i++) {
      assert(p1.exp_term_at(i)[0] == i * 2.1);
      assert(p1.exp_term_at(i)[1] == i * 2.2);
    }
  }
  
  return 0;
}
