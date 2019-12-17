//
// Created by zvone on 16-Dec-19.
//
#include <stdafx.h>

#include <glm/vec3.hpp>

#include <algorithm>
#include <geometry/material.h>
#include <glm/glm.hpp>

#include "torrancesparrowbrdf.h"


float TorranceSparrowBRDF::exponent = 50;
float TorranceSparrowBRDF::refractiveIndex = 1.4;
float TorranceSparrowBRDF::kdfresnel = 1;

float TorranceSparrowBRDF::blinnDistribution(float cosalpha) {
  static float exp_plus_2 = exponent + 2.0f;
  
  return exp_plus_2 * (1. / M_PI) * pow(cosalpha, exponent);
}

float TorranceSparrowBRDF::geometryTerm(const glm::vec3 &wi,
                                        const glm::vec3 &wo,
                                        const glm::vec3 &n,
                                        const glm::vec3 &h) {
  float nh = dot(n, h);
  float nwo = dot(n, wo);
  float nwi = dot(n, wi);
  float woh = dot(wo, h);
  float inv_woh = woh ? (1.0f / woh) : 1.0f;
  
  float term0 = 2.0f * nh * nwo * inv_woh;
  float term1 = 2.0f * nh * nwi * inv_woh;
  
  return std::min(1.0f, std::min(term0, term1));
}

float TorranceSparrowBRDF::fresnelReflection(float cosbeta) {
  static float n1 = IOR_AIR;
  float n2 = refractiveIndex;
  float omc = 1.0f - cosbeta;
  float R0 = ((n1 - n2) * (n1 - n2)) / ((n1 + n2) * (n1 + n2));
  
  return R0 + (1.0f - R0) * omc * omc * omc * omc * omc;
}

glm::vec3 TorranceSparrowBRDF::getBRDF(const Material *material,
                                       const glm::vec3 &normal,
                                       const glm::vec3 &lightDir,
                                       const glm::vec3 &directionToCamera,
                                       const glm::vec3 &worldPos) {
  
  static float oneOverFour = 1.0f / 4.0f;
  float costheta_i = std::max(0.0f, dot(normal, lightDir));
  if (costheta_i > 0.0f)  // theta_i < 90°
  {
    glm::vec3 kd = material->diffuse;
    glm::vec3 ks = material->specular;
    
    glm::vec3 h = glm::normalize(lightDir + directionToCamera);
    
    
    float cosalpha_h = std::max(0.0f, dot(normal, h));
    float D = blinnDistribution(cosalpha_h);
    float G = geometryTerm(lightDir, directionToCamera, normal, h);
    float F = fresnelReflection(std::max(0.0f, dot(directionToCamera, h)));
    float cosphi = std::max(0.0f, dot(directionToCamera, normal));
    float f = (kdfresnel) ? (1.0f - F) : 1.0f;
    
    return kd * glm::vec3(f * (1. / M_PI)) + ks * ((D * F * G * oneOverFour) / (costheta_i * cosphi));
  }
  
  return glm::vec3(0.0f);
}
