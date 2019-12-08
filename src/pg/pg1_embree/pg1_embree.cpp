#include <stdafx.h>
#include <tutorials.h>
#include <tests/test.h>

int main() {
  printf("PG1, (c)2011-2018 Tomas Fabian\n\n");
  
  _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
  _MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);
  
  testAll();
  
  try {
    //  return staticSceneSphere();
//      return staticSceneGeoSpheres();
    //  return staticSceneShip();
    staticSceneCornellbox();
  }
  catch (std::exception &e) {
    std::cout << "Exception occured: " << e.what() << '\n';
  }
  
  std::cout << "Total allocations runtime: " << totalAllocations << '\n';
  std::cout << "Total deletes runtime: " << totalDelete << '\n';
  return 0;
}
