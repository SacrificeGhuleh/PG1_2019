#include <stdafx.h>
#include <tutorials.h>
#include <tests/test.h>

int main() {
  printf("PG1, (c)2011-2018 Tomas Fabian\n\n");
  
  _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
  _MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);
  
  testAll();

//  return staticSceneSphere();
//  return staticSceneGeoSpheres();
  return staticSceneShip();
//  return staticSceneCornellbox();

}
