//
// Created by zvone on 16-Dec-19.
//

#ifndef PG1_2019_TORRANCESPARROWBRDF_H
#define PG1_2019_TORRANCESPARROWBRDF_H

#include <shaders/brdf.h>
#include <glm/vec3.hpp>


class Material;

class TorranceSparrowBRDF : public BRDF {
  static float exponent;
  static float refractiveIndex;
  static float kdfresnel;
  
  static float blinnDistribution(float cosalpha);
  
  static float geometryTerm(const glm::vec3 &wi, const glm::vec3 &wo, const glm::vec3 &n, const glm::vec3 &h);
  
  static float fresnelReflection(float cosbeta);
public:
  static glm::vec3 getBRDF(const Material *material,
                           const glm::vec3 &normal,
                           const glm::vec3 &lightDir,
                           const glm::vec3 &directionToCamera,
                           const glm::vec3 &worldPos);
};


#endif //PG1_2019_TORRANCESPARROWBRDF_H
