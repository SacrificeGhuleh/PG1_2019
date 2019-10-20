//
// Created by zvone on 28-Sep-19.
//

#include <stdafx.h>
#include <math/vector.h>
#include <geometry/material.h>
#include <shaders/normalsshader.h>
#include <engine/light.h>
#include <engine/camera.h>
#include <glm/geometric.hpp>


NormalsShader::NormalsShader(Camera *camera, Light *light, RTCScene *rtcscene, std::vector<Surface *> *surfaces,
                             std::vector<Material *> *materials) : Shader(camera, light, rtcscene, surfaces,
                                                                          materials) {}

Color4f NormalsShader::traceRay(const RtcRayHitIor &rayHit, int depth) {
  if (rayHit.hit.geomID == RTC_INVALID_GEOMETRY_ID) {
    return getBackgroundColor(rayHit);
  }
  RTCGeometry geometry = rtcGetGeometry(*rtcScene_, rayHit.hit.geomID);
  /**
   * Acquire material from hit object
   */
  Normal3f normal;
  // get interpolated normal
  rtcInterpolate0(geometry, rayHit.hit.primID, rayHit.hit.u, rayHit.hit.v,
                  RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 0, &normal.x, 3);
  
  Vector3 origin(rayHit.ray.org_x, rayHit.ray.org_y, rayHit.ray.org_z);
  Vector3 direction(rayHit.ray.dir_x, rayHit.ray.dir_y, rayHit.ray.dir_z);
  Vector3 worldPos = origin + direction * rayHit.ray.tfar;
  Vector3 lightDir = light_->getPosition() - worldPos;
  
  lightDir = glm::normalize(lightDir);
  normal = glm::normalize(normal);
  
  float diff = glm::dot(normal, lightDir);
  
  if (correctNormals_) {
    if (diff < 0) {
      normal *= -1.f;
      diff *= -1.f;
    }
  }
  return Color4f(normal, 1.0f);
}
