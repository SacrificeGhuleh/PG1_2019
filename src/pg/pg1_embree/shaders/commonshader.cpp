//
// Created by zvone on 24-Oct-19.
//

#include <stdafx.h>

#include <shaders/commonshader.h>

#include <math/mymath.h>

#include <engine/light.h>
#include <engine/camera.h>

#include <geometry/material.h>
#include <utils/utils.h>
#include "torrancesparrowbrdf.h"


ShadingType CommonShader::useShader = ShadingType::None;
float CommonShader::ior = 1;

bool CommonShader::softShadows = false;
int CommonShader::lightShadowsSamples = 16;

CommonShader::CommonShader(Camera *camera,
                           Light *light,
                           RTCScene *rtcScene,
                           std::vector<Surface *> *surfaces,
                           std::vector<Material *> *materials) :
    Shader(camera, light, rtcScene, surfaces, materials) {}

glm::vec4 CommonShader::traceRay(const RtcRayHitIor &rayHit, int depth) {
  if (rayHit.hit.geomID == RTC_INVALID_GEOMETRY_ID) {
    return getBackgroundColor(rayHit);
  }
  
  glm::vec3 resultColor(0, 0, 0);
  
  if (depth <= 0) {
    //return from recursion, alpha 0 because division by alpha
    return glm::vec4(resultColor, 0.0f);
  }
  
  
  const RTCGeometry geometry = rtcGetGeometry(*rtcScene_, rayHit.hit.geomID);
  // get interpolated normal
  const glm::vec3 normal = glm::normalize(getNormal(geometry, rayHit));
  // and texture coordinates
  const glm::vec2 tex_coord = getTexCoords(geometry, rayHit);
  
  //Acquire material from hit object
  Material *material = static_cast<Material *>(rtcGetGeometryUserData(geometry));
  
  /*
   * Common for all shaders
   * */
  const glm::vec3 origin(rayHit.ray.org_x, rayHit.ray.org_y, rayHit.ray.org_z);
//  const glm::vec3 direction(rayHit.ray.dir_x, rayHit.ray.dir_y, rayHit.ray.dir_z);
  const glm::vec3 direction = glm::normalize(glm::vec3(rayHit.ray.dir_x, rayHit.ray.dir_y, rayHit.ray.dir_z));
  const glm::vec3 worldPos = origin + direction * rayHit.ray.tfar;
  const glm::vec3 directionToCamera = -direction;
  const glm::vec3 lightPos = light_->getPosition();
  const glm::vec3 lightDir = glm::normalize(light_->getPosition() - worldPos);
  
  glm::vec3 shaderNormal = normal;
  
  float dotNormalCamera = glm::dot(shaderNormal, directionToCamera);
  //Flip normal if invalid
  if (correctNormals_) {
    if (dotNormalCamera < 0) {
      shaderNormal *= -1.f;
//      dotNormalCamera *= -1.f;
      dotNormalCamera = glm::dot(shaderNormal, directionToCamera);
    }
    assert(dotNormalCamera >= 0);
  }
  
  /*
   * End of common for all shaders
   * */
  
  
  const ShadingType selectedShader = (useShader == ShadingType::None) ? material->shadingType : useShader;
  
  switch (selectedShader) {
    case ShadingType::None: {
      resultColor = traceMaterial<ShadingType::None>(rayHit, material, tex_coord, origin, direction, worldPos,
                                                     directionToCamera, lightPos, lightDir, shaderNormal,
                                                     dotNormalCamera, depth);
      break;
    }
    
    case ShadingType::Glass: {
      resultColor = traceMaterial<ShadingType::Glass>(rayHit, material, tex_coord, origin, direction, worldPos,
                                                      directionToCamera, lightPos, lightDir, shaderNormal,
                                                      dotNormalCamera, depth);
      break;
    }
    case ShadingType::Lambert: {
      resultColor = traceMaterial<ShadingType::Lambert>(rayHit, material, tex_coord, origin, direction, worldPos,
                                                        directionToCamera, lightPos, lightDir, shaderNormal,
                                                        dotNormalCamera, depth);
      break;
    }
    case ShadingType::Mirror: {
      resultColor = traceMaterial<ShadingType::Mirror>(rayHit, material, tex_coord, origin, direction, worldPos,
                                                       directionToCamera, lightPos, lightDir, shaderNormal,
                                                       dotNormalCamera, depth);
      break;
    }
    case ShadingType::Phong: {
      resultColor = traceMaterial<ShadingType::Phong>(rayHit, material, tex_coord, origin, direction, worldPos,
                                                      directionToCamera, lightPos, lightDir, shaderNormal,
                                                      dotNormalCamera, depth);
      break;
    }
    case ShadingType::PathTracing: {
      resultColor = traceMaterial<ShadingType::PathTracing>(rayHit, material, tex_coord, origin, direction, worldPos,
                                                            directionToCamera, lightPos, lightDir, shaderNormal,
                                                            dotNormalCamera, depth);
      break;
    }
    case ShadingType::Normals: {
      resultColor = traceMaterial<ShadingType::Normals>(rayHit, material, tex_coord, origin, direction, worldPos,
                                                        directionToCamera, lightPos, lightDir, shaderNormal,
                                                        dotNormalCamera, depth);
      break;
    };
    case ShadingType::TexCoords: {
      resultColor = traceMaterial<ShadingType::TexCoords>(rayHit, material, tex_coord, origin, direction, worldPos,
                                                          directionToCamera, lightPos, lightDir, shaderNormal,
                                                          dotNormalCamera, depth);
      break;
    };
  }
  
  return glm::vec4(resultColor, 1.0f);
}

template<ShadingType T>
glm::vec4 CommonShader::traceMaterial(const RtcRayHitIor &rayHit,
                                      const Material *material,
                                      const glm::vec2 &tex_coord,
                                      const glm::vec3 &origin,
                                      const glm::vec3 &direction,
                                      const glm::vec3 &worldPos,
                                      const glm::vec3 &directionToCamera,
                                      const glm::vec3 &lightPos,
                                      const glm::vec3 &lightDir,
                                      const glm::vec3 &shaderNormal,
                                      const float dotNormalCamera,
                                      const int depth) {
  
  
  std::cout << "Warning, no material\n";
  return glm::vec4(1, 0, 1, 1);
}


template<>
glm::vec4 CommonShader::traceMaterial<ShadingType::None>(const RtcRayHitIor &rayHit,
                                                         const Material *material,
                                                         const glm::vec2 &tex_coord,
                                                         const glm::vec3 &origin,
                                                         const glm::vec3 &direction,
                                                         const glm::vec3 &worldPos,
                                                         const glm::vec3 &directionToCamera,
                                                         const glm::vec3 &lightPos,
                                                         const glm::vec3 &lightDir,
                                                         const glm::vec3 &shaderNormal,
                                                         const float dotNormalCamera,
                                                         const int depth) {
  
  
  return glm::vec4(1.f, 0.f, 0.f, 1.f);
}

template<>
glm::vec4 CommonShader::traceMaterial<ShadingType::Glass>(const RtcRayHitIor &rayHit,
                                                          const Material *material,
                                                          const glm::vec2 &tex_coord,
                                                          const glm::vec3 &origin,
                                                          const glm::vec3 &direction,
                                                          const glm::vec3 &worldPos,
                                                          const glm::vec3 &directionToCamera,
                                                          const glm::vec3 &lightPos,
                                                          const glm::vec3 &lightDir,
                                                          const glm::vec3 &shaderNormal,
                                                          const float dotNormalCamera,
                                                          const int depth) {
  
  
  float materialIor;
  if (useShader == ShadingType::Glass) {
    materialIor = ior;
  } else {
    assert(material->ior >= 0);
    materialIor = material->ior;
  }
  
  if (depth <= 0) {
    return getBackgroundColor(rayHit);
  }
  
  glm::vec4 reflected(0.f, 0.f, 0.f, 1.f);
  glm::vec4 refracted(0.f, 0.f, 0.f, 1.f);
  
  //n1 = ray ior
  //n2 = material ior
  //if n1 != vzduch than n2 = air
  float n1 = rayHit.ior;
  float n2 = materialIor;
  
  if (n1 != IOR_AIR) {
    n2 = IOR_AIR;
  }
  
  assert(n1 >= 0);
  assert(n2 >= 0);
  //0.64 1.54
  const float n1overn2 = (n1 / n2);
//cos1
  float Q1 = dotNormalCamera;
  
  assert(Q1 >= 0);
  
  const glm::vec3 reflectDir = glm::normalize(
      (2.f * (directionToCamera * shaderNormal)) * shaderNormal - directionToCamera);
  
  RtcRayHitIor reflectedRayHit = generateRay(worldPos, reflectDir, tNear_);
  reflectedRayHit.ior = n2;
  
  const float tmp = 1.f - sqr(n1overn2) * (1.f - sqr(Q1));
  if (tmp < 0.f) {
    return traceRay(reflectedRayHit, depth - 1);
  }

//cos2
  const float Q2 = sqrtf(tmp);
  const glm::vec3 refractedDir = glm::normalize(
      (n1overn2 * direction) + ((n1overn2 * Q1 - Q2) * shaderNormal));
// Fresnel
  const float R = Shader::fresnel(n1, n2, Q1, Q2);
  const float coefReflect = R;
  const float coefRefract = 1.f - R;
  
  RtcRayHitIor refractedRayHit = generateRay(worldPos, refractedDir, tNear_);
  refractedRayHit.ior = n2;
  
  reflected = traceRay(reflectedRayHit, depth - 1);
  refracted = traceRay(refractedRayHit, depth - 1);
  
  assert(coefReflect >= 0.f);
  assert(coefReflect <= 1.f);
  
  assert(coefRefract >= 0.f);
  assert(coefRefract <= 1.f);
  
  //C = [Crefl*R + Crefr*(1-R)] * TbeerLambert
  //TbeerLambert = {1,1,1} for air
  //TbeerLambert = {e^-Mr*l,e^-Mg*l,e^-Mb*l} for air, l = delka paprsku, M = absorpce materialu
  glm::vec3 TBeerLambert;
  if (rayHit.ior == IOR_AIR) {
    TBeerLambert = {1.f, 1.f, 1.f};
  } else {
    const float l = rayHit.ray.tfar;
//    const glm::vec3 M = {10, 0.5, 10};
    const glm::vec3 M = material->absorption;
    TBeerLambert = {
        expf(-M.r * l),
        expf(-M.g * l),
        expf(-M.b * l)
    };
  }
  
  const glm::vec3 finalReflect = coefReflect * glm::vec3(reflected.x, reflected.y, reflected.z);
  const glm::vec3 finalRefract = coefRefract * glm::vec3(refracted.x, refracted.y, refracted.z);
  
  return glm::vec4((finalReflect + finalRefract) * TBeerLambert, 1);

//  return coefReflect * reflected + coefRefract * refracted * glm::vec4(TBeerLambert, 1.f);
}

template<>
glm::vec4 CommonShader::traceMaterial<ShadingType::Lambert>(const RtcRayHitIor &rayHit,
                                                            const Material *material,
                                                            const glm::vec2 &tex_coord,
                                                            const glm::vec3 &origin,
                                                            const glm::vec3 &direction,
                                                            const glm::vec3 &worldPos,
                                                            const glm::vec3 &directionToCamera,
                                                            const glm::vec3 &lightPos,
                                                            const glm::vec3 &lightDir,
                                                            const glm::vec3 &shaderNormal,
                                                            const float dotNormalCamera,
                                                            const int depth) {
  glm::vec3 diffuse = dotNormalCamera * getDiffuseColor(material, tex_coord);
  
  //shadow
  float shadowVal = 1;
  float pdf = 0;
  
  if (softShadows) {
    
    shadowVal += shadow(worldPos, lightDir, glm::l2Norm(lightPos - worldPos));
    
    for (int i = 0; i < lightShadowsSamples; i++) {
      const glm::vec3 lDir = hemisphereSampling(lightDir, pdf);
      shadowVal += shadow(worldPos, lDir, glm::l2Norm(lightPos - worldPos));
    }
    
    shadowVal /= static_cast<float>(lightShadowsSamples + 1);
  } else {
    //hard shadow
    shadowVal = shadow(worldPos, lightDir, glm::l2Norm(lightPos));
  }
  
  const float dotNormalLight = glm::dot(shaderNormal, lightDir);
  
  return glm::vec4(
      ((diffuse.x * shadowVal * dotNormalLight)),
      ((diffuse.y * shadowVal * dotNormalLight)),
      ((diffuse.z * shadowVal * dotNormalLight)),
      1);
}

template<>
glm::vec4 CommonShader::traceMaterial<ShadingType::Mirror>(const RtcRayHitIor &rayHit,
                                                           const Material *material,
                                                           const glm::vec2 &tex_coord,
                                                           const glm::vec3 &origin,
                                                           const glm::vec3 &direction,
                                                           const glm::vec3 &worldPos,
                                                           const glm::vec3 &directionToCamera,
                                                           const glm::vec3 &lightPos,
                                                           const glm::vec3 &lightDir,
                                                           const glm::vec3 &shaderNormal,
                                                           const float dotNormalCamera,
                                                           const int depth) {
  //I - N * dot(N, I) * 2
  glm::vec3 reflectDir = glm::reflect(direction, shaderNormal);
  
  glm::vec4 reflected(0.f, 0.f, 0.f, 1.f);
  
  RtcRayHitIor reflectedRayHit = generateRay(worldPos, reflectDir, tNear_);
  reflected = traceRay(reflectedRayHit, depth - 1);
  return reflected;
}

template<>
glm::vec4 CommonShader::traceMaterial<ShadingType::Phong>(const RtcRayHitIor &rayHit,
                                                          const Material *material,
                                                          const glm::vec2 &tex_coord,
                                                          const glm::vec3 &origin,
                                                          const glm::vec3 &direction,
                                                          const glm::vec3 &worldPos,
                                                          const glm::vec3 &directionToCamera,
                                                          const glm::vec3 &lightPos,
                                                          const glm::vec3 &lightDir,
                                                          const glm::vec3 &shaderNormal,
                                                          const float dotNormalCamera,
                                                          const int depth) {
  
  
  glm::vec4 reflected = traceMaterial<ShadingType::Mirror>(rayHit, material, tex_coord, origin, direction, worldPos,
                                                           directionToCamera, lightPos, lightDir, shaderNormal,
                                                           dotNormalCamera, depth);
  
  if (material->reflectivity >= 1.0) {
    return reflected;
  }
  
  //ambient
  glm::vec3 ambient = material->ambient;
  
  //diffuse
  float shadowVal = 0;
  float pdf = 0;
  
  if (softShadows) {
    
    shadowVal += shadow(worldPos, lightDir, glm::l2Norm(lightPos - worldPos));
    
    for (int i = 0; i < lightShadowsSamples; i++) {
      const glm::vec3 lDir = hemisphereSampling(lightDir, pdf);
      shadowVal += shadow(worldPos, lDir, glm::l2Norm(lightPos - worldPos));
    }
    
    shadowVal /= static_cast<float>(lightShadowsSamples + 1);
  } else {
    //hard shadow
    shadowVal = shadow(worldPos, lightDir, glm::l2Norm(lightPos));
  }
  
  glm::vec3 diffuse = shadowVal * dotNormalCamera * getDiffuseColor(material, tex_coord);
  
  //specular
  //I - N * dot(N, I) * 2
  //glm::vec3 lightReflectDir = glm::reflect(lightDir, shaderNormal);
//  float spec = powf(glm::dot(direction, lightReflectDir), material->shininess);
  
  //blinn-phong lightning
  glm::vec3 halfwayDir = glm::normalize(lightDir + direction);
  float spec = powf(std::max<float>(glm::dot(shaderNormal, halfwayDir), 0.0), material->shininess);
  
  glm::vec3 specular = material->specular * spec;
  
//  glm::vec3 torranceSparrow = TorranceSparrowBRDF::getBRDF(material, shaderNormal, lightDir, directionToCamera,
//                                                           worldPos);
  
//  return glm::vec4(std::max<float>(glm::dot(shaderNormal, lightDir), 0) * torranceSparrow, 1);
  return glm::vec4(diffuse + specular * spec, 1) + (reflected * material->reflectivity/* * spec*/);
}

template<>
glm::vec4 CommonShader::traceMaterial<ShadingType::PathTracing>(const RtcRayHitIor &rayHit,
                                                                const Material *material,
                                                                const glm::vec2 &tex_coord,
                                                                const glm::vec3 &origin,
                                                                const glm::vec3 &direction,
                                                                const glm::vec3 &worldPos,
                                                                const glm::vec3 &directionToCamera,
                                                                const glm::vec3 &lightPos,
                                                                const glm::vec3 &lightDir,
                                                                const glm::vec3 &shaderNormal,
                                                                const float dotNormalCamera,
                                                                const int depth) {
  
  glm::vec3 emmision = glm::vec3{material->emission.x, material->emission.y, material->emission.z};
  
  float pdf;
  const glm::vec3 omegaI = hemisphereSampling(shaderNormal, pdf);
  
  const RtcRayHitIor rayHitNew = generateRay(worldPos, omegaI);
  
  const glm::vec4 reflColor = traceRay(rayHitNew, depth - 1);
  
  const glm::vec3 diffuse = getDiffuseColor(material, tex_coord);
  
  const glm::vec3 fR = diffuse * glm::vec3(1. / M_PI);

//  float shadowVal = shadow(worldPos, lightDir, glm::l2Norm(light_->getPosition()));

//  emmision += glm::vec3(reflColor.x, reflColor.y, reflColor.z) * diffuse;
  
  //return glm::vec4(emmision.r, emmision.g, emmision.b, 1);
  
  glm::vec3 finalColor = emmision +
                         glm::vec3(reflColor.x, reflColor.y, reflColor.z) *
                         diffuse *
                         (glm::dot(shaderNormal, omegaI) / pdf);
  
  /*return glm::vec4(
      glm::vec3(reflColor.x, reflColor.y, reflColor.z) *
      glm::dot(shaderNormal, omegaI) *
      diffuse,
      1);*/
  
  return glm::vec4(finalColor.x, finalColor.y, finalColor.z, 1);
}

template<>
glm::vec4 CommonShader::traceMaterial<ShadingType::Normals>(const RtcRayHitIor &rayHit,
                                                            const Material *material,
                                                            const glm::vec2 &tex_coord,
                                                            const glm::vec3 &origin,
                                                            const glm::vec3 &direction,
                                                            const glm::vec3 &worldPos,
                                                            const glm::vec3 &directionToCamera,
                                                            const glm::vec3 &lightPos,
                                                            const glm::vec3 &lightDir,
                                                            const glm::vec3 &shaderNormal,
                                                            const float dotNormalCamera,
                                                            const int depth) {
  
  //Debug dot normal
//  return glm::vec4(glm::vec3(dotNormalCamera), 1.0f);
  return glm::vec4((shaderNormal.x + 1.f) / 2.f,
                   (shaderNormal.x + 1.f) / 2.f,
                   (shaderNormal.x + 1.f) / 2.f,
                   1.0f);
}

template<>
glm::vec4 CommonShader::traceMaterial<ShadingType::TexCoords>(const RtcRayHitIor &rayHit,
                                                              const Material *material,
                                                              const glm::vec2 &tex_coord,
                                                              const glm::vec3 &origin,
                                                              const glm::vec3 &direction,
                                                              const glm::vec3 &worldPos,
                                                              const glm::vec3 &directionToCamera,
                                                              const glm::vec3 &lightPos,
                                                              const glm::vec3 &lightDir,
                                                              const glm::vec3 &shaderNormal,
                                                              const float dotNormalCamera,
                                                              const int depth) {
  
  
  return glm::vec4(tex_coord.x, tex_coord.y, 1.0, 1.0f);
}