//
// Created by zvone on 03-Oct-19.
//

#include <stdafx.h>
#include <shaders/glassshader.h>

#include <math/vector.h>
#include <geometry/material.h>
#include <engine/light.h>
#include <engine/camera.h>

float GlassShader::reflectivityCoef = 1.f;

GlassShader::GlassShader(Camera *camera, Light *light, RTCScene *rtcscene, std::vector<Surface *> *surfaces,
                         std::vector<Material *> *materials) : Shader(camera, light, rtcscene, surfaces, materials) {}

Color4f GlassShader::traceRay(const RTCRayHit &rayHit, int depth) {
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
    
    Color4f reflected(0.f, 0.f, 0.f, 0.f);
    Color4f refracted(0.f, 0.f, 0.f, 0.f);
    {
      RTCRay reflectedRay = {worldPos.x, worldPos.y, worldPos.z, 0.01f, reflectDir.x, reflectDir.y, reflectDir.z, 0,
                             FLT_MAX, 0, 0, 0};
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
      reflected = traceRay(reflectedRayHit, depth - 1);
    }
    
    {
      Vector3 d = &rayHit.ray.dir_x;
      Vector3 n = normal;
      
      
      float n1 = material->ior;
      float n2 = 1.f;
      
      float cos_01 = (-n).DotProduct(d);
      if (cos_01 < 0) {
        n = -n;
        cos_01 = (-n).DotProduct(d);
      }
      
      /*n1 / n2*/
      float n1overn2 = (n1 / n2);
      
      float cos_02 = sqrtf(1.f - (n1overn2 * n1overn2) * (1 - (cos_01 * cos_01)));
      
      Vector3 l = n1overn2 * d + (n1overn2 * cos_01 - cos_02) * n;
      
      Vector3 r = (2 * (n.DotProduct(-d))) * n - (-d);
      
      RTCRay refractedRay = {worldPos.x, worldPos.y, worldPos.z, 0.01f, r.x, r.y, r.z, 0,
                             FLT_MAX, 0, 0, 0};
      RTCHit refractedHit;
      refractedHit.geomID = RTC_INVALID_GEOMETRY_ID;
      refractedHit.primID = RTC_INVALID_GEOMETRY_ID;
      refractedHit.Ng_x = 0.0f; // geometry normal
      refractedHit.Ng_y = 0.0f;
      refractedHit.Ng_z = 0.0f;
      
      // merge ray and hit structures
      RTCRayHit refractedRayHit;
      refractedRayHit.ray = refractedRay;
      refractedRayHit.hit = refractedHit;
      
      // intersect ray with the scene
      RTCIntersectContext context;
      rtcInitIntersectContext(&context);
      rtcIntersect1(*rtcScene_, &context, &refractedRayHit);
      refracted = traceRay(refractedRayHit, depth - 1);
    }
    
    Color4f C;
    
    Vector3 reflectedColor = ambient + diffuse +
                             (Vector3(reflected.r, reflected.g, reflected.b) * material->reflectivity * reflectivityCoef);
  
  
    Vector3 refractedColor = ambient + diffuse +
                             (Vector3(reflected.r, reflected.g, reflected.b) * material->reflectivity * reflectivityCoef);
    //C = R * C_r + T * C_t
    return Color4f(
        ambient + diffuse +
        (Vector3(reflected.r, reflected.g, reflected.b) * material->reflectivity * reflectivityCoef), 1.0f);
  }
  
  //sum results
  return Color4f(ambient + specular + diffuse, 1.0f);
}
