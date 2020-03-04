
#include "../base/BaseHeader.h"
#include "../base/Allocator.h"

#include <iostream>
namespace BR2 {

class BTest {
public:
  int x = 0;
  void test() {
    Allocator<char> ac;

    ac.realloca(256);

    std::cout << ac.count() << std::endl;

    int n = 0;
  }
};

}
