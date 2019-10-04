#include <stdafx.h>
#include <tutorials.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <math/vector.h>
#include <iostream>

int main() {
  printf("PG1, (c)2011-2018 Tomas Fabian\n\n");
  
  _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
  _MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);
  
  //return staticSceneSphere();
  //return staticSceneShip();
  
  //return tutorial_1();
  //return tutorial_2();
//  return tutorial_3("data/6887_allied_avenger.obj");
  
  
  {
    glm::vec3 d(-0.429, -0.903, 0);
    glm::vec3 n(0, 1, 0);
    
    float n1 = 1.5;
    float n2 = 1.f;
    
    float cos_01 = glm::dot(-n, d);;
    if (cos_01 < 0) {
      n = -n;
      cos_01 = glm::dot(-n, d);
    }
  
    float n1overn2 = (n1 / n2);
  
    float cos_02 = sqrtf(1.f - (n1overn2 * n1overn2) * (1 - (cos_01 * cos_01)));
  
    glm::vec3 l = n1overn2 * d + (n1overn2 * cos_01 - cos_02) * n;
  
    glm::vec3 r = (2.f * (glm::dot(n, -d))) * n - (-d);
    std::cout << glm::to_string(r) << std::endl;
  }
  
  {
    Vector3 d = Vector3(-0.429, -0.903, 0);
    Vector3 n = Vector3(0, 1, 0);
    
    
    float n1 = 1.5;
    float n2 = 1.f;
    
    float cos_01 = (-n).DotProduct(d);
    if (cos_01 < 0) {
      n = -n;
      cos_01 = (-n).DotProduct(d);
    }
    
    /*n1 / n2*/
    float n1overn2 = (n1 / n2);
    
    float cos_02 = sqrtf(1.f - (n1overn2 * n1overn2) * (1 - (cos_01 * cos_01)));
    
    Vector3 l = n1overn2 * d + (n1overn2 * cos_01 - cos_02) * n;
    
    Vector3 r = (2 * (n.DotProduct(-d))) * n - (-d);
    r.Print();
  }
  return 0;
//  return tutorial_3("data/geosphere.obj");
}
