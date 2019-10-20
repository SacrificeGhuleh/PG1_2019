//
// Created by zvone on 03-Oct-19.
//

#include <stdafx.h>

#include <shaders/glassshader.h>

#include <math/vector.h>
#include <math/mymath.h>

#include <geometry/material.h>

#include <engine/light.h>
#include <engine/camera.h>

static float tNear = 0.01f;

bool GlassShader::addReflect_ = true;
bool GlassShader::addRefract_ = true;
bool GlassShader::addDiffuseToReflect_ = false;
bool GlassShader::addDiffuseToRefract_ = false;

GlassShader::GlassShader(Camera *camera, Light *light, RTCScene *rtcscene, std::vector<Surface *> *surfaces,
                         std::vector<Material *> *materials) : Shader(camera, light, rtcscene, surfaces, materials) {}


Color4f GlassShader::traceRay(const RtcRayHitIor &rayHit, int depth, bool switchIor) {
  if (rayHit.hit.geomID == RTC_INVALID_GEOMETRY_ID) {
    return getBackgroundColor(rayHit);
  }
  
  RTCGeometry geometry = rtcGetGeometry(*rtcScene_, rayHit.hit.geomID);
  Normal3f normal;
  // get interpolated normal
  rtcInterpolate0(geometry, rayHit.hit.primID, rayHit.hit.u, rayHit.hit.v,
                  RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 0, &normal.x, 3);
  // and texture coordinates
  Coord2f tex_coord;
  rtcInterpolate0(geometry, rayHit.hit.primID, rayHit.hit.u, rayHit.hit.v,
                  RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 1, &tex_coord.u, 2);
  
  /**
   * Acquire material from hit object
   */
  Material *material = (Material *) (rtcGetGeometryUserData(geometry));
  //ambient
  Vector3 ambient = material->ambient;
  
  //diffuse
  Vector3 origin(rayHit.ray.org_x, rayHit.ray.org_y, rayHit.ray.org_z);
  Vector3 direction(rayHit.ray.dir_x, rayHit.ray.dir_y, rayHit.ray.dir_z);
  Vector3 worldPos = origin + direction * rayHit.ray.tfar;
  Vector3 lightDir = light_->getPosition() - worldPos;
  
  lightDir = glm::normalize(lightDir);
  normal = glm::normalize(normal);
  
  float diff = glm::dot(normal, lightDir);
  
  //Flip normal if invalid
  if (correctNormals_) {
    if (diff < 0) {
      normal *= -1.f;
      diff *= -1.f;
    }
  }
  
  Vector3 diffuse = diff * getDiffuseColor(material, tex_coord);
  
  //specular
  Vector3 viewDir = (origin - worldPos);
  viewDir = glm::normalize(viewDir);
  Vector3 reflectDir = glm::reflect(normal, lightDir);
  float spec = powf(glm::dot(viewDir, reflectDir), material->shininess);
  Vector3 specular(spec);
  
  if (depth <= 0) {
    return Color4f(specular + diffuse, 1.0f);
  }
  
  Color4f reflected(0.f, 0.f, 0.f, 0.f);
  Color4f refracted(0.f, 0.f, 0.f, 0.f);

//  RtcRayHitIor reflectedRayHit = generateRay(worldPos, reflectDir, tNear);
//  reflected = traceRay(reflectedRayHit, depth - 1);
//
  Vector3 rayDirection(rayHit.ray.dir_x, rayHit.ray.dir_y, rayHit.ray.dir_z);
  Vector3 norm = normal;
  //n1 = ray ior
  //n2 = material ior
  //if n1 != vzduch than n2 = air
  
  
  float n1 = rayHit.ior;
  float n2 = material->ior;
  
  if (n1 != IOR_AIR) {
    n2 = IOR_AIR;
  }
  
  if (n2 < 0) {
    n2 = IOR_AIR;
  }
  
  float n1overn2 = (n1 / n2);
  
  //cos1
  float Q1 = glm::dot(-norm, rayDirection);
  if (Q1 < 0) {
    norm = -norm;
    Q1 = glm::dot(-norm, rayDirection);
  }
  assert(Q1 >= 0);
  
  //cos2
  float Q2 = sqrtf(1.f - sqr(n1overn2) * (1 - sqr(Q1)));
  
  Vector3 l = n1overn2 * rayDirection + (n1overn2 * Q1 - Q2) * norm;
  Vector3 refractedDir = (2 * glm::dot(norm, -rayDirection)) * norm - (-rayDirection);

//Vector3 refractedDir = (-n1overn2 * rayDirection - (n1overn2 * Q1 + Q2) * norm);
  refractedDir = glm::normalize(refractedDir);
  
  RtcRayHitIor refractedRayHit = generateRay(
      Vector3(worldPos.x, worldPos.y, worldPos.z),
      Vector3(refractedDir.x, refractedDir.y, refractedDir.z),
      tNear);
  refractedRayHit.ior = n2;
  
  refracted = traceRay(refractedRayHit, depth - 1);
  
  reflectDir = __max((2.0f * glm::dot(normal, -rayDirection)), 0.0f) * normal + rayDirection;
  reflectDir = glm::normalize(reflectDir);
  
  RtcRayHitIor reflectedRayHit = generateRay(worldPos, reflectDir, tNear);
  reflectedRayHit.ior = n1;
  reflected = traceRay(reflectedRayHit, depth - 1);
  
  // Fresnel
  float R = fresnel(n1, n2, Q2, Q1);
  
  float coefReflect = R;
  float coefRefract = 1.f - R;


//  Vector4 C = (coefReflect * reflected /** Vector4(diffuse, 1.f)*/) + (coefRefract * refracted /** Vector4(diffuse, 1.f)*/);
  
  assert(coefReflect >= 0.f);
  assert(coefReflect <= 1.f);
  
  assert(coefRefract >= 0.f);
  assert(coefRefract <= 1.f);
//
  Vector4 C = {0.f, 0.f, 0.f, 1.f};
  
  //C = [Crefl*R + Crefr*(1-R)] * TbeerLambert
  //TbeerLambert = {1,1,1} for air
  //TbeerLambert = {e^-Mr*l,e^-Mg*l,e^-Mb*l} for air, l = delka paprsku, M = absorpce materialu
  
  if (addReflect_) {
    C += coefReflect * reflected;
    if (addDiffuseToReflect_) {
      C *= Vector4(diffuse, 1.f);
    }
  }
  
  if (addRefract_) {
    C += coefRefract * refracted;
    if (addDiffuseToRefract_) {
      C *= Vector4(diffuse, 1.f);
    }
  }
  return C;
}

Color4f GlassShader::traceRay(const RtcRayHitIor &rayHit, int depth) {
  return traceRay(rayHit, depth, false);
}
