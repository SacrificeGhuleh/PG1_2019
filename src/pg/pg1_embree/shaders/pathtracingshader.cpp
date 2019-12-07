//
// Created by zvone on 24-Oct-19.
//

#include <stdafx.h>
#include <math/vector.h>
#include <geometry/material.h>
#include <shaders/pathtracingshader.h>
#include <engine/light.h>
#include <engine/camera.h>
#include <utils/utils.h>

int PathTracingShader::samples_ = 300;
bool PathTracingShader::useParentGetPixel_ = false;

PathTracingShader::PathTracingShader(Camera *camera, Light *light, RTCScene *rtcscene, std::vector<Surface *> *surfaces,
                                     std::vector<Material *> *materials) : Shader(camera, light, rtcscene, surfaces,
                                                                                  materials) {}

//Color4f PathTracingShader::traceRay(const RtcRayHitIor &rayHit, int depth) {
//  if (rayHit.hit.geomID == RTC_INVALID_GEOMETRY_ID) {
////    return Color4f(1, 1, 1, 1);
//    return getBackgroundColor(rayHit);
//  }
//
////  if (depth <= 0) {
////    return Color4f(0, 0, 0, 1);
////
////  }
//
//  const RTCGeometry geometry = rtcGetGeometry(*rtcScene_, rayHit.hit.geomID);
//  // get interpolated normal
//  const Normal3f normal = glm::normalize(getNormal(geometry, rayHit));
//  // and texture coordinates
//  const Coord2f tex_coord = getTexCoords(geometry, rayHit);
//
//  //Acquire material from hit object
//  Material *material = static_cast<Material *>(rtcGetGeometryUserData(geometry));
//
//  const Vector3 origin(rayHit.ray.org_x, rayHit.ray.org_y, rayHit.ray.org_z);
//  const Vector3 direction(rayHit.ray.dir_x, rayHit.ray.dir_y, rayHit.ray.dir_z);
//  const Vector3 worldPos = origin + direction * rayHit.ray.tfar;
//
//  Color4f emmision = Color4f{material->emission.x, material->emission.y, material->emission.z, 1};
//  if (emmision.r != 0 && emmision.g != 0 && emmision.b != 0) {
//    return emmision;
//  }
//
//  Vector3 rd = Vector3(rayHit.ray.dir_x, rayHit.ray.dir_y, rayHit.ray.dir_z);
//  Vector3 rv = Vector3(-rd.x, -rd.y, -rd.z);
//
//
//  Vector3 omegaI = hemisphereSampling(normal);
//  float pdf = 1. / (2. * M_PI);
//
//  RtcRayHitIor rayHitNew = generateRay(worldPos, omegaI);
//
//  Color4f l_i = traceRay(rayHitNew/*, depth-1*/);
//  Vector3 fR = material->diffuse * glm::vec3(1. / M_PI);
//
//  Vector3 resultColor = fR * glm::vec3(l_i.x, l_i.y, l_i.z) * (glm::dot(normal, omegaI) / pdf);
//  return Color4f(resultColor, 1);
//}


Color4f PathTracingShader::traceRay(const RtcRayHitIor &rayHit, int depth) {
  if (!rayHit.collided()) {
    return getBackgroundColor(rayHit);
  }
  const RTCGeometry geometry = rtcGetGeometry(*rtcScene_, rayHit.hit.geomID);
  // get interpolated normal
  const Normal3f normal = glm::normalize(getNormal(geometry, rayHit));
  // and texture coordinates
  const Coord2f tex_coord = getTexCoords(geometry, rayHit);
  
  //Acquire material from hit object
  Material *material = static_cast<Material *>(rtcGetGeometryUserData(geometry));
  
  const Vector3 origin(rayHit.ray.org_x, rayHit.ray.org_y, rayHit.ray.org_z);
  const Vector3 direction(rayHit.ray.dir_x, rayHit.ray.dir_y, rayHit.ray.dir_z);
  const Vector3 worldPos = origin + direction * rayHit.ray.tfar;
  
  Color4f emmision = Color4f{material->emission.x, material->emission.y, material->emission.z, 1};
  if (emmision.r != 0 && emmision.g != 0 && emmision.b != 0) {
    return emmision;
  }
  
  const Vector3 rayDir = Vector3(rayHit.ray.dir_x, rayHit.ray.dir_y, rayHit.ray.dir_z);
  const Vector3 rayView = Vector3(-rayDir.x, -rayDir.y, -rayDir.z);
  float pdf;
  const Vector3 omegaI = hemisphereSampling(normal, pdf);
  
  const glm::vec3 Le = material->ior < 0 ? glm::vec3(0, 0, 0) : glm::vec3(5, 5, 5);
  
  RtcRayHitIor rayHitNew = generateRay(worldPos, omegaI);
  
  Color4f reflColor = traceRay(rayHitNew);
  Vector3 fR = material->diffuse * glm::vec3(1. / M_PI);
  
  float cosTheta = glm::dot(normal, omegaI);
  glm::vec3 reflColor3 = glm::vec3(reflColor.x, reflColor.y, reflColor.z);
  const float brdf = 0.5f / M_PI;
  
  glm::vec3 pix = Le +
                  material->diffuse *
                  reflColor3 *
                  cosTheta *
                  brdf *
                  (1.0f / (cosTheta / (float)M_PI));
  
  return Color4f(pix, 1);
  
  
//  return Color4f(
//      glm::vec3(reflColor.x, reflColor.y, reflColor.z) *
//      cosTheta *
//      (float) M_PI *
//      pdf,
//      1); // Jitted ray dir
  
}


glm::vec3 PathTracingShader::hemisphereSampling(glm::vec3 normal, float &pdf) {
  const float M_2PI = 2.f * M_PI;
  const float randomU = Random();
  const float randomV = Random();
  
  const float x = 2.f * cosf(M_2PI * randomU) * sqrtf(randomV * (1.f - randomV));
  const float y = 2.f * sinf(M_2PI * randomU) * sqrtf(randomV * (1.f - randomV));
  const float z = 1.f - 2.f * randomV;
  
  Vector3 omegaI = glm::normalize(Vector3(x, y, z));
  
  if (glm::dot(omegaI, normal) < 0) {
//    omegaI *= -1;
    omegaI = -omegaI;
  }
  const float pdfConst = 1. / 2. * M_PI;
  pdf = pdfConst;
  
  return omegaI;
}

Color4f PathTracingShader::getPixel(int x, int y) {
  if (useParentGetPixel_) {
    return Shader::getPixel(x, y);
  }
  
  Color4f finalColor(0, 0, 0, 1);
  #pragma omp parallel for schedule(dynamic, 5) shared(finalColor)
  for (int i = 0; i < samples_; i++) {
    float offsetX = (Random(-0.5f, 0.5f));
    float offsetY = (Random(-0.5f, 0.5f));
    
    RtcRayHitIor ray = shootRay(static_cast<float>(x) + offsetX, static_cast<float>(y) + offsetY);
    finalColor += traceRay(ray, recursionDepth_);
  }
  //assert((int)finalColor.a == (int)samples_);
  finalColor = finalColor / finalColor.a;
  return finalColor;// / static_cast<float>(samples_);
}
