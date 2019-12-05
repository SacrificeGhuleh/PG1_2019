//
// Created by zvone on 20-Oct-19.
//

#ifndef PG1_2019_TEST_H
#define PG1_2019_TEST_H

#include <iostream>
#include <glm/glm.hpp>
#include <math/mymath.h>
#include <shaders/shader.h>
#include <glm/gtx/string_cast.hpp>


inline void testGlassShader() {
  std::cout << "Testing glass shader" << std::endl << std::endl;
  
  glm::vec3 normal = {0, 1, 0};
  glm::vec3 rayDirection = {-0.429, -0.903, 0};
  glm::vec3 rayToObserver = -rayDirection;
  
  float n1 = 1.5;
  float n2 = 1;
  
  float n1overn2 = (n1 / n2);
  
  //cos1
  float Q1 = glm::dot(normal, rayToObserver);
  float Q1Ref = 0.903;
  std::cout << "Q1: " << Q1 << " Reference: " << Q1Ref << "\n  Diff: " << Q1 - Q1Ref << std::endl;
  
  if (Q1 < 0) {
    normal = -normal;
    Q1 = glm::dot(-normal, rayToObserver);
  }
  
  float tmp = 1.f - sqr(n1overn2) * (1.f - sqr(Q1));
  
  //cos2
  float Q2 = sqrtf(tmp);
  float Q2Ref = 0.766f;
  std::cout << "Q2: " << Q2 << " Reference: " << Q2Ref << "\n  Diff: " << Q2 - Q2Ref << std::endl;
  
  glm::vec3 refractedDir = (n1overn2 * rayDirection) + ((n1overn2 * Q1 - Q2) * normal);
  glm::vec3 refractedDirReference = {-0.643f, -0.766f, 0.f};
  refractedDir = glm::normalize(refractedDir);
  refractedDirReference = glm::normalize(refractedDirReference);
  
  std::cout << "refractedDir: " << glm::to_string(refractedDir) << " Reference: "
            << glm::to_string(refractedDirReference) << "\n  Diff: "
            << glm::to_string(refractedDir - refractedDirReference) << std::endl;

//  glm::vec3 reflectedDir = glm::reflect(normal, rayDirection);
  glm::vec3 reflectedDir = (2.f * (rayToObserver * normal)) * normal - rayToObserver;
  glm::vec3 reflectedDirReference = {-0.429, 0.903, 0};
  reflectedDir = glm::normalize(reflectedDir);
  reflectedDirReference = glm::normalize(reflectedDirReference);
  std::cout << "reflectedDir: " << glm::to_string(reflectedDir) << " Reference: "
            << glm::to_string(reflectedDirReference) << "\n  Diff: "
            << glm::to_string(reflectedDir - reflectedDirReference) << std::endl;
  
  // Fresnel
  float R = Shader::fresnel(n1, n2, Q1, Q2);

//  float Rs = sqr<float>((n2 * Q2 - n1 * Q1) / (n2 * Q2 + n1 * Q1));
//  float Rp = sqr<float>((n2 * Q1 - n1 * Q2) / (n2 * Q1 + n1 * Q2));
//  float R = (Rs + Rp) / 2.f;
//  float T = 1.f - R;
  
  float coefReflect = R;
  float coefReflectReference = 0.046;
  
  float coefRefract = 1.f - R;
  float coefRefractReference = 0.954;
  
  std::cout << "coefReflect: " << coefReflect << " Reference: " << coefReflectReference << "\n  Diff: "
            << coefReflect - coefReflectReference << std::endl;
  std::cout << "coefRefract: " << coefRefract << " Reference: " << coefRefractReference << "\n  Diff: "
            << coefRefract - coefRefractReference << std::endl;

//  std::cout << glm::to_string(r) << std::endl;
}

inline void testAll() {
  std::cout << "Performing tests" << std::endl;
  testGlassShader();
}


#endif //PG1_2019_TEST_H
