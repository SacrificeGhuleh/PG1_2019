//
// Created by zvone on 03-Oct-19.
//

#include <stdafx.h>
#include <shaders/glassshader.h>

#include <math/vector.h>
#include <geometry/material.h>
#include <engine/light.h>
#include <engine/camera.h>
#include <glm/geometric.hpp>
#include <math/mymath.h>

float GlassShader::reflectivityCoef = 1.f;
float GlassShader::rCoef = 0.046f;

GlassShader::GlassShader(Camera *camera, Light *light, RTCScene *rtcscene, std::vector<Surface *> *surfaces,
                         std::vector<Material *> *materials) : Shader(camera, light, rtcscene, surfaces, materials) {}


Color4f GlassShader::traceRay(const RTCRayHit &rayHit, int depth, bool switchIor) {
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
    return Color4f(ambient + specular + diffuse, 1.0f);
  }
  Color4f reflected(0.f, 0.f, 0.f, 0.f);
  Color4f refracted(0.f, 0.f, 0.f, 0.f);
  float R = 0.f, T = 0.f;
  {
    RTCRayHit reflectedRayHit = generateRay(worldPos, reflectDir, 0.01f);
    reflected = traceRay(reflectedRayHit, depth - 1);
  }
  
  {
    Vector3 d(rayHit.ray.dir_x, rayHit.ray.dir_y, rayHit.ray.dir_z);
    Vector3 n = normal;
    
    float n1;
    float n2;
    if (switchIor) {
      n1 = 1;
      n2 = material->ior;
    } else {
      n1 = material->ior;
      n2 = 1.f;
    }
    
    float cos_01 = glm::dot(-n, d);
    if (cos_01 < 0) {
      n = -n;
      cos_01 = glm::dot(-n, d);
    }
    
    float n1overn2 = (n1 / n2);
    float cos_02 = sqrtf(1.f - (n1overn2 * n1overn2) * (1 - (cos_01 * cos_01)));
    Vector3 l = n1overn2 * d + (n1overn2 * cos_01 - cos_02) * n;
    Vector3 r = (2 * glm::dot(n, -d)) * n - (-d);
    
    RTCRayHit refractedRayHit = generateRay(
        glm::vec3(worldPos.x, worldPos.y, worldPos.z),
        glm::vec3(r.x, r.y, r.z),
        0.01f);
    
    refracted = traceRay(refractedRayHit, depth - 1, !switchIor);
    
    float Rs = sqr<float>((n2 * cos_02 - n1 * cos_01) / (n2 * cos_02 + n1 * cos_01));
    float Rp = sqr<float>((n2 * cos_01 - n1 * cos_02) / (n2 * cos_01 + n1 * cos_02));
    R = (Rs + Rp) / 2.f;
    T = 1.f - R;
  }
  
  Vector4 C = (R * reflected * Vector4(diffuse, 1.f)) + (T * refracted * Vector4(diffuse, 1.f));
  return C;
}

Color4f GlassShader::traceRay(const RTCRayHit &rayHit, int depth) {
  return traceRay(rayHit, depth, false);
}
