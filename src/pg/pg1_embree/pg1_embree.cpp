#include <stdafx.h>
#include <tutorials.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <math/vector.h>
#include <iostream>
#include <math/mymath.h>

int main() {
  printf("PG1, (c)2011-2018 Tomas Fabian\n\n");
  
  _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
  _MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);

  return staticSceneSphere();
  //return staticSceneShip();
  
  //return tutorial_1();
  //return tutorial_2();
  //return tutorial_3("data/6887_allied_avenger.obj");
  
  
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
    
    float cos_02 = sqrtf(1.f - sqr<float>(n1overn2) * (1 - sqr<float>(cos_01)));
    
    glm::vec3 l = n1overn2 * d + (n1overn2 * cos_01 - cos_02) * n;
    
    glm::vec3 r = (2.f * (glm::dot(n, -d))) * n - (-d);
    std::cout << glm::to_string(r) << std::endl;
    
    
    float Rs = sqr<float>((n2 * cos_02 - n1 * cos_01) / (n2 * cos_02 + n1 * cos_01));
    float Rp = sqr<float>((n2 * cos_01 - n1 * cos_02) / (n2 * cos_01 + n1 * cos_02));
    float R = (Rs + Rp) / 2.f;
    float T = 1.f - R;
    
    std::cout << "Rs: " << Rs << std::endl;
    std::cout << "Rp: " << Rp << std::endl;
    std::cout << "R: " << R << std::endl;
    std::cout << "T: " << T << std::endl;
  }
  return 0;
}
