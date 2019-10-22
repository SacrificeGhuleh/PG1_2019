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

bool GlassShader::addReflect_ = true;
bool GlassShader::addRefract_ = true;
bool GlassShader::addDiffuseToReflect_ = false;
bool GlassShader::addDiffuseToRefract_ = false;
bool GlassShader::addAttenuation = false;
float GlassShader::attenuation_ = 0.f;
float GlassShader::ior_ = 1.5f;

GlassShader::GlassShader(Camera *camera, Light *light, RTCScene *rtcscene, std::vector<Surface *> *surfaces,
                         std::vector<Material *> *materials) : Shader(camera, light, rtcscene, surfaces, materials) {}

Color4f GlassShader::traceRay(const RtcRayHitIor &rayHit, int depth) {
  if (rayHit.hit.geomID == RTC_INVALID_GEOMETRY_ID) {
    return getBackgroundColor(rayHit);
  }
  
  const RTCGeometry geometry = rtcGetGeometry(*rtcScene_, rayHit.hit.geomID);
  // get interpolated normal
  const Normal3f normal = glm::normalize(getNormal(geometry, rayHit));
  // and texture coordinates
  const Coord2f tex_coord = getTexCoords(geometry, rayHit);
  
  //Acquire material from hit object
  Material *material = static_cast<Material *>(rtcGetGeometryUserData(geometry));
  
  //ambient
  Vector3 ambient = material->ambient;
  
  //diffuse
  const Vector3 origin(rayHit.ray.org_x, rayHit.ray.org_y, rayHit.ray.org_z);
  const Vector3 direction(rayHit.ray.dir_x, rayHit.ray.dir_y, rayHit.ray.dir_z);
  const Vector3 worldPos = origin + direction * rayHit.ray.tfar;
  const Vector3 lightDir = glm::normalize(light_->getPosition() - worldPos);
  
  glm::vec3 normalForPhong = glm::normalize(normal);
  
  float diff = glm::dot(normalForPhong, lightDir);
  
  //Flip normal if invalid
  if (correctNormals_) {
    if (diff < 0) {
      normalForPhong *= -1.f;
      diff *= -1.f;
    }
  }
  
  Vector3 diffuse = diff * getDiffuseColor(material, tex_coord);
  
  //specular
  Vector3 viewDir = (origin - worldPos);
  viewDir = glm::normalize(viewDir);
  Vector3 phongReflectDir = glm::reflect(normalForPhong, lightDir);
  float spec = powf(glm::dot(viewDir, phongReflectDir), material->shininess);
  Vector3 specular(spec);
  
  if (depth <= 0) {
    return getBackgroundColor(rayHit);
  }
  
  Color4f reflected(0.f, 0.f, 0.f, 1.f);
  Color4f refracted(0.f, 0.f, 0.f, 1.f);
  
  glm::vec3 normalForGlass = glm::normalize(normal);
  const glm::vec3 rayDirection(rayHit.ray.dir_x, rayHit.ray.dir_y, rayHit.ray.dir_z);
  const glm::vec3 rayToObserver = -rayDirection;
  
  //n1 = ray ior
  //n2 = material ior
  //if n1 != vzduch than n2 = air
  float n1 = rayHit.ior;
//  float n2 = material->ior;
  float n2 = ior_;
  
  if (n1 != IOR_AIR) {
    n2 = IOR_AIR;
  }

//  if(n1 < 0 || n2 < 0){
//    n1 = n2 = 1.f;
//  }
  
  assert(n1 >= 0);
  assert(n2 >= 0);
  //0.64 1.54
  const float n1overn2 = (n1 / n2);
//cos1
  float Q1 = glm::dot(normalForGlass, rayToObserver);
  
  if (Q1 < 0) {
    normalForGlass = -normalForGlass;
    Q1 = glm::dot(normalForGlass, rayToObserver);
  }
  assert(Q1 >= 0);
  
  const glm::vec3 reflectDir = glm::normalize(
      (2.f * (rayToObserver * normalForGlass)) * normalForGlass - rayToObserver);
  
  RtcRayHitIor reflectedRayHit = generateRay(worldPos, reflectDir, tNear_);
  reflectedRayHit.ior = n2;
  
  const float tmp = 1.f - sqr(n1overn2) * (1.f - sqr(Q1));
  if (tmp < 0.f) {
    glm::vec4 C = traceRay(reflectedRayHit, depth - 1);
    return C;
  }

//cos2
  const float Q2 = sqrtf(tmp);
  const glm::vec3 refractedDir = glm::normalize((n1overn2 * rayDirection) + ((n1overn2 * Q1 - Q2) * normalForGlass));
// Fresnel
  const float R = Shader::fresnel(n1, n2, Q1, Q2);
  const float coefReflect = R;
  const float coefRefract = 1.f - R;
  
  RtcRayHitIor refractedRayHit = generateRay(worldPos, refractedDir, tNear_);
  refractedRayHit.ior = n2;
  
  reflected = traceRay(reflectedRayHit, depth - 1);
  refracted = traceRay(refractedRayHit, depth - 1);

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
  glm::vec3 TBeerLambert;
  if (rayHit.ior == IOR_AIR) {
    TBeerLambert = {1.f, 1.f, 1.f};
  } else {
    const float l = rayHit.ray.tfar;
    const glm::vec3 M = {attenuation_, attenuation_, attenuation_};
    TBeerLambert = {
        exp(-M.r * l),
        exp(-M.g * l),
        exp(-M.b * l)
    };
  }
  //TBeerLambert = {1, 1, 1};
  
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
  
  if (addAttenuation) {
    C *= glm::vec4(TBeerLambert, 1.f);
  }
  
  
  return C;
}