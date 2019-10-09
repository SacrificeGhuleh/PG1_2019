//
// Created by zvone on 28-Sep-19.
//

#include <stdafx.h>
#include <shaders/recursivephongshader.h>
#include <math/vector.h>
#include <geometry/material.h>
#include <engine/light.h>
#include <engine/camera.h>
#include <glm/geometric.hpp>

float RecursivePhongShader::reflectivityCoef = 1.f;
static float tNear = 0.01f;

RecursivePhongShader::RecursivePhongShader(
    Camera *camera,
    Light *light,
    RTCScene *rtcscene,
    std::vector<Surface *> *surfaces,
    std::vector<Material *> *materials)
    : Shader(camera, light, rtcscene, surfaces, materials) {}

Color4f RecursivePhongShader::traceRay(const RTCRayHit &rayHit, int depth) {
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
  
  RTCRayHit reflectedRayHit = generateRay(worldPos, reflectDir, tNear);
  reflected = traceRay(reflectedRayHit, depth - 1);
  
  //Vector4 C(diffuse + (specular * Vector3(reflected)), 1.0f);
  //return C;
  return reflected;
}
