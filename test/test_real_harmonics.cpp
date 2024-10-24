#include "real_harmonics.hpp"
#include <cassert>

using namespace dso; 

int main() {

  {
    RealHarmonics h;
    
    h.add_harmonic(1,1,1);
    assert(h.num_harmonics() ==1);
    assert(h(0) == 1);
    assert(h(0)+1 == 1);
    assert(h(0)+2 == 1);
    
    h.add_harmonic(2,2,2);
    assert(h.num_harmonics() ==2);
    assert(h(1) == 2);
    assert(h(1)+1 == 2);
    assert(h(1)+2 == 2);

    
    h.add_harmonic(3.5,3.5,3.5);
    assert(h.num_harmonics() ==3);
    assert(h(2) == 3.5);
    assert(h(2)+1 == 3.5);
    assert(h(2)+2 == 3.5);

    for (int i = 0; i < 5; i++) {
      h.add_harmonic(i * .1, i * .2, i * .4);
      assert(h.num_harmonics() == 4 + i);
      assert(h(i) == i * .1);
      assert(h(i) + 1 == i * .2);
      assert(h(i) + 2 == i * .4);
    }

    h(1)[0] = 2.1;
    h(1)[1] = 2.2;
    h(1)[2] = 2.3;
    assert(h(1)+0 == 2.1);
    assert(h(1)+1 == 2.2);
    assert(h(1)+2 == 2.3);

    auto rh = h;
    assert(h(0) == 1);
    assert(h(0)+1 == 1);
    assert(h(0)+2 == 1);
    assert(h(1)+0 == 2.1);
    assert(h(1)+1 == 2.2);
    assert(h(1)+2 == 2.3);
    assert(h(2) == 3.5);
    assert(h(2)+1 == 3.5);
    assert(h(2)+2 == 3.5);
  }
  
  {
    RealHarmonics h(7);
    
    assert(h.num_harmonics() == 7);

    for (int i=0 ;i<7; i++) {
      h(i)[0] = i*.1+.1;
      h(i)[1] = i*.1+.2;
      h(i)[2] = i*.1+.3;
    }

    h.add_harmonic(2,2,2);
    h.add_harmonic(2,2,2);
    
    for (int i=0 ;i<7; i++) {
      assert(h(i)[0] == i*.1+.1);
      assert(h(i)[1] == i*.1+.2);
      assert(h(i)[2] == i*.1+.3);
    }

    auto h2 = h;
    assert(h.num_harmonics() == h2.num_harmonics());
    h.add_harmonic(2,2,2);
    h.add_harmonic(2,2,2);
    assert(h.num_harmonics() == h2.num_harmonics()+2);
    h2.add_harmonic(2,2,2);
    h2.add_harmonic(2,2,2);
    h2.add_harmonic(2,2,2);
    h2.add_harmonic(2,2,2);
    h2.add_harmonic(2,2,2);
    assert(h.num_harmonics() == h2.num_harmonics()-3);
    
    for (int i=0 ;i<7; i++) {
      assert(h2(i)[0] == i*.1+.1);
      assert(h2(i)[1] == i*.1+.2);
      assert(h2(i)[2] == i*.1+.3);
    }
  }

  return 0;

}
