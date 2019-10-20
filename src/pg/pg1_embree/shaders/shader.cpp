//
// Created by zvone on 26-Sep-19.
//
#include <stdafx.h>

#include <shaders/shader.h>

#include <geometry/surface.h>

#include <engine/light.h>
#include <engine/camera.h>
#include <engine/sphericalmap.h>

#include <geometry/material.h>
#include <glm/geometric.hpp>
#include <math/mymath.h>

bool Shader::flipTextureU_ = false;
bool Shader::sphereMap_ = true;
bool Shader::correctNormals_ = true;
bool Shader::flipTextureV_ = true;
bool Shader::supersampling_ = false;
int Shader::samplingSize_ = 3;
int Shader::recursionDepth_ = 1;

Shader::Shader(Camera *camera, Light *light, RTCScene *rtcScene, std::vector<Surface *> *surfaces,
               std::vector<Material *> *materials) : camera_{camera}, light_{light}, rtcScene_{rtcScene},
                                                     surfaces_{surfaces}, materials_{materials},
                                                     sphericalMap_{nullptr} {}

RtcRayHitIor Shader::shootRay(const float x, const float y) {
  RtcRayHitIor rayHit = generateRay(camera_->getViewFrom(), camera_->getRayDirection(x, y), FLT_MIN);
  return rayHit;
}

Color3f Shader::getDiffuseColor(const Material *material, const Coord2f &tex_coord) {
  Texture *diffTexture = material->get_texture(Material::kDiffuseMapSlot);
  Vector3 geomDiffuse;
  
  if (diffTexture != NULL) {
    Coord2f correctedTexCoord = tex_coord;
    
    if (flipTextureU_) correctedTexCoord.u = 1.f - correctedTexCoord.u;
    if (flipTextureV_) correctedTexCoord.v = 1.f - correctedTexCoord.v;
    
    geomDiffuse = diffTexture->get_texel(correctedTexCoord.u, /*1.f-*/correctedTexCoord.v);
  } else {
    geomDiffuse = material->diffuse;
  }
  return geomDiffuse;
}

Color4f Shader::getPixel(const int x, const int y) {
  if (supersampling_) {
    Color4f finalColor(0.f);
    float offsetX = -0.5f;
    float offsetY = -0.5f;
    float offsetAddition = 1.0f / static_cast<float>(samplingSize_);
    
    for (int i = 0; i < samplingSize_; i++) {
      for (int j = 0; j < samplingSize_; j++) {
        RtcRayHitIor ray = shootRay(static_cast<float>(x) + offsetX, static_cast<float>(y) + offsetY);
        finalColor += traceRay(ray, recursionDepth_);
        offsetX += offsetAddition;
      }
      offsetX = -0.5f;
      offsetY += offsetAddition;
    }
    return finalColor / static_cast<float>(samplingSize_ * samplingSize_);
  } else {
    RtcRayHitIor ray = shootRay(static_cast<float>(x), static_cast<float>(y));
    return traceRay(ray, recursionDepth_);
  }
}

void Shader::setDefaultBgColor(Color4f *defaultBgColor) {
  defaultBgColor_ = defaultBgColor;
}

Color4f Shader::traceRay(const RtcRayHitIor &rayHit, int depth) {
  if (rayHit.hit.geomID == RTC_INVALID_GEOMETRY_ID) {
    return getBackgroundColor(rayHit);
  }
  
  return Color4f(0.0f, 1.0f, 1.0f, 1.0f);
}

void Shader::setSphericalMap(SphericalMap *sphericalMap) {
  sphericalMap_ = sphericalMap;
}

Color4f Shader::getBackgroundColor(const RtcRayHitIor &rayHit) {
  if (sphericalMap_ != nullptr && sphereMap_) {
    Vector3 rayDir(rayHit.ray.dir_x, rayHit.ray.dir_y, rayHit.ray.dir_z);
    rayDir = glm::normalize(rayDir);
    return Color4f(sphericalMap_->texel(rayDir), 1.0f);
  }
  
  return *defaultBgColor_;
}

RtcRayHitIor Shader::generateRay(const glm::vec3 &origin, const glm::vec3 &direction, const float tnear) {
  RTCRay ray;
  
  ray.org_x = origin.x;     // x coordinate of ray origin
  ray.org_y = origin.y;     // y coordinate of ray origin
  ray.org_z = origin.z;     // z coordinate of ray origin
  
  ray.tnear = tnear;        // start of ray segment
  
  ray.dir_x = direction.x;  // x coordinate of ray direction
  ray.dir_y = direction.y;  // y coordinate of ray direction
  ray.dir_z = direction.z;  // z coordinate of ray direction
  
  ray.time = 0;             // time of this ray for motion blur
  ray.tfar = FLT_MAX;       // end of ray segment (set to hit distance)
  ray.mask = 0;             // ray mask
  ray.id = 0;               // ray ID
  ray.flags = 0;            // ray flags
  
  
  RTCHit hit;
  hit.geomID = RTC_INVALID_GEOMETRY_ID;
  hit.primID = RTC_INVALID_GEOMETRY_ID;
  hit.Ng_x = 0.0f; // geometry normal
  hit.Ng_y = 0.0f;
  hit.Ng_z = 0.0f;
  
  // merge ray and hit structures
  RtcRayHitIor rayHit;
  rayHit.ray = ray;
  rayHit.hit = hit;
  
  // intersect ray with the scene
  RTCIntersectContext context;
  rtcInitIntersectContext(&context);
  rtcIntersect1(*rtcScene_, &context, &rayHit);
  return rayHit;
}


float Shader::fresnel(const float n1, const float n2, const float Q1, const float Q2) {
  float Rs = sqr((n1 * Q2 - n2 * Q1) / (n1 * Q2 + n2 * Q1));
  float Rp = sqr((n1 * Q1 - n2 * Q2) / (n1 * Q1 + n2 * Q2));
//  float R = 0.5f * (Rs + Rp);
  
  return 0.5f * (Rs + Rp);
}