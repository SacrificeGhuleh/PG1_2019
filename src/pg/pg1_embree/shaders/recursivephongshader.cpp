//
// Created by zvone on 28-Sep-19.
//

#include <stdafx.h>
#include <shaders/recursivephongshader.h>
#include <math/vector.h>
#include <geometry/material.h>
#include <engine/light.h>
#include <engine/camera.h>

float RecursivePhongShader::reflectivityCoef = 1.f;

RecursivePhongShader::RecursivePhongShader(Camera *camera, Light *light, RTCScene *rtcscene,
                                           std::vector<Surface *> *surfaces, std::vector<Material *> *materials)
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
  Vector3 origin(&rayHit.ray.org_x);
  Vector3 direction(&rayHit.ray.dir_x);
  Vector3 worldPos = origin + direction * rayHit.ray.tfar;
  Vector3 lightDir = light_->getPosition() - worldPos;
  lightDir.Normalize();
  normal.Normalize();
  float diff = normal.DotProduct(lightDir);
  
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
  viewDir.Normalize();
  Vector3 reflectDir = normal.reflect(lightDir);
  float spec = powf(viewDir.DotProduct(reflectDir), material->shininess);
  Vector3 specular(spec);
  
  if (depth > 0) {
    
    RTCRay reflectedRay;
    
    reflectedRay.org_x = worldPos.x;    // x coordinate of ray origin
    reflectedRay.org_y = worldPos.y;    // y coordinate of ray origin
    reflectedRay.org_z = worldPos.z;    // z coordinate of ray origin
    
    reflectedRay.tnear = 2.f;           // start of ray segment
    
    reflectedRay.dir_x = reflectDir.x;  // x coordinate of ray direction
    reflectedRay.dir_y = reflectDir.y;  // y coordinate of ray direction
    reflectedRay.dir_z = reflectDir.z;  // z coordinate of ray direction
    
    reflectedRay.time = 0;              // time of this ray for motion blur
    reflectedRay.tfar = FLT_MAX;        // end of ray segment (set to hit distance)
    reflectedRay.mask = 0;              // ray mask
    reflectedRay.id = 0;                // ray ID
    reflectedRay.flags = 0;             // ray flags
    
    
    RTCHit reflectedHit;
    reflectedHit.geomID = RTC_INVALID_GEOMETRY_ID;
    reflectedHit.primID = RTC_INVALID_GEOMETRY_ID;
    reflectedHit.Ng_x = 0.0f; // geometry normal
    reflectedHit.Ng_y = 0.0f;
    reflectedHit.Ng_z = 0.0f;
    
    // merge ray and hit structures
    RTCRayHit reflectedRayHit;
    reflectedRayHit.ray = reflectedRay;
    reflectedRayHit.hit = reflectedHit;
    
    // intersect ray with the scene
    RTCIntersectContext context;
    rtcInitIntersectContext(&context);
    rtcIntersect1(*rtcScene_, &context, &reflectedRayHit);
    Color4f reflected = traceRay(reflectedRayHit, depth - 1);
    return Color4f(
        (Vector3(reflected.r, reflected.g, reflected.b) * material->reflectivity * reflectivityCoef), 1.0f);
  }
  
  //sum results
  return Color4f(specular + diffuse, 1.0f);
}
