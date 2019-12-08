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
#include <math/mymath.h>
#include <utils/utils.h>

#include <shaders/pathtracerhelper.h>

bool Shader::flipTextureU_ = false;
bool Shader::sphereMap_ = false;
bool Shader::correctNormals_ = true;
bool Shader::flipTextureV_ = true;
bool Shader::supersampling_ = false;
bool Shader::supersamplingRandom_ = false;
bool Shader::changeShader_ = false;
int Shader::samplingSize_ = 3;
int Shader::samplingSizeX_ = 3;
int Shader::samplingSizeY_ = 3;
int Shader::recursionDepth_ = 10;
float Shader::tNear_ = 0.01f;
SuperSamplingType Shader::superSamplingType_ = SuperSamplingType::None;

Shader::Shader(Camera *camera,
               Light *light,
               RTCScene *rtcScene,
               std::vector<Surface *> *surfaces,
               std::vector<Material *> *materials) :
    camera_{camera},
    light_{light},
    rtcScene_{rtcScene},
    surfaces_{surfaces},
    materials_{materials},
    sphericalMap_{nullptr} {}

RtcRayHitIor Shader::shootRay(const float x, const float y) {
  RtcRayHitIor rayHit = generateRay(
      camera_->getViewFrom(),
      camera_->getRayDirection(x + 0, y + 0),
      FLT_MIN);
  return rayHit;
}

RtcRayHitIor4 Shader::shootRay4(float x, float y) {
  RtcRayHitIor4 rayHit = generateRay4(
      camera_->getViewFrom(),
      {
          camera_->getRayDirection(x + 0, y + 0),
          camera_->getRayDirection(x + 1, y + 0),
          camera_->getRayDirection(x + 0, y + 1),
          camera_->getRayDirection(x + 1, y + 1)
      },
      FLT_MIN);
  return rayHit;
}

RtcRayHitIor4 Shader::shootRay4(std::array<float, 4> &x, std::array<float, 4> &y) {
  RtcRayHitIor4 rayHit = generateRay4(
      camera_->getViewFrom(),
      {
          camera_->getRayDirection(x[0], y[0]),
          camera_->getRayDirection(x[1], y[1]),
          camera_->getRayDirection(x[2], y[2]),
          camera_->getRayDirection(x[3], y[3])
      },
      FLT_MIN);
  return rayHit;
}

RtcRayHitIor8 Shader::shootRay8(float x, float y) {
  RtcRayHitIor8 rayHit = generateRay8(
      camera_->getViewFrom(),
      {
          camera_->getRayDirection(x + 0, y + 0),
          camera_->getRayDirection(x + 1, y + 0),
          camera_->getRayDirection(x + 2, y + 0),
          camera_->getRayDirection(x + 3, y + 0),
          camera_->getRayDirection(x + 0, y + 1),
          camera_->getRayDirection(x + 1, y + 1),
          camera_->getRayDirection(x + 2, y + 1),
          camera_->getRayDirection(x + 3, y + 1)
      },
      FLT_MIN);
  return rayHit;
}

RtcRayHitIor8 Shader::shootRay8(std::array<float, 8> &x, std::array<float, 8> &y) {
  RtcRayHitIor8 rayHit = generateRay8(
      camera_->getViewFrom(),
      {
          camera_->getRayDirection(x[0], y[0]),
          camera_->getRayDirection(x[1], y[1]),
          camera_->getRayDirection(x[2], y[2]),
          camera_->getRayDirection(x[3], y[3]),
          camera_->getRayDirection(x[4], y[4]),
          camera_->getRayDirection(x[5], y[5]),
          camera_->getRayDirection(x[6], y[6]),
          camera_->getRayDirection(x[7], y[7])
      },
      FLT_MIN);
  return rayHit;
  
}

RtcRayHitIor16 Shader::shootRay16(float x, float y) {
  RtcRayHitIor16 rayHit = generateRay16(
      camera_->getViewFrom(),
      {
          camera_->getRayDirection(x + 0, y + 0),
          camera_->getRayDirection(x + 1, y + 0),
          camera_->getRayDirection(x + 2, y + 0),
          camera_->getRayDirection(x + 3, y + 0),
          camera_->getRayDirection(x + 0, y + 1),
          camera_->getRayDirection(x + 1, y + 1),
          camera_->getRayDirection(x + 2, y + 1),
          camera_->getRayDirection(x + 3, y + 1),
          camera_->getRayDirection(x + 0, y + 2),
          camera_->getRayDirection(x + 1, y + 2),
          camera_->getRayDirection(x + 2, y + 2),
          camera_->getRayDirection(x + 3, y + 2),
          camera_->getRayDirection(x + 0, y + 3),
          camera_->getRayDirection(x + 1, y + 3),
          camera_->getRayDirection(x + 2, y + 3),
          camera_->getRayDirection(x + 3, y + 3),
      },
      FLT_MIN);
  return rayHit;
}

RtcRayHitIor16 Shader::shootRay16(std::array<float, 16> &x, std::array<float, 16> &y) {
  RtcRayHitIor16 rayHit = generateRay16(
      camera_->getViewFrom(),
      {
          camera_->getRayDirection(x[0], y[0]),
          camera_->getRayDirection(x[1], y[1]),
          camera_->getRayDirection(x[2], y[2]),
          camera_->getRayDirection(x[3], y[3]),
          camera_->getRayDirection(x[4], y[4]),
          camera_->getRayDirection(x[5], y[5]),
          camera_->getRayDirection(x[6], y[6]),
          camera_->getRayDirection(x[7], y[7]),
          camera_->getRayDirection(x[8], y[8]),
          camera_->getRayDirection(x[9], y[9]),
          camera_->getRayDirection(x[10], y[10]),
          camera_->getRayDirection(x[11], y[11]),
          camera_->getRayDirection(x[12], y[12]),
          camera_->getRayDirection(x[13], y[13]),
          camera_->getRayDirection(x[14], y[14]),
          camera_->getRayDirection(x[15], y[15]),
      },
      FLT_MIN);
  return rayHit;
}

Color3f Shader::getDiffuseColor(const Material *material, const Coord2f &tex_coord) {
  Texture *diffTexture = material->get_texture(Material::kDiffuseMapSlot);
  Vector3 geomDiffuse;
  
  if (diffTexture != nullptr) {
    Coord2f correctedTexCoord = tex_coord;
    
    if (flipTextureU_) correctedTexCoord.x = 1.f - correctedTexCoord.x;
    if (flipTextureV_) correctedTexCoord.y = 1.f - correctedTexCoord.y;
    
    geomDiffuse = diffTexture->get_texel(correctedTexCoord.x, correctedTexCoord.y);
  } else {
    geomDiffuse = material->diffuse;
  }
  return geomDiffuse;
}

Color4f Shader::getPixel(const int x, const int y) {
  Color4f finalColor(0, 0, 0, 0);
  
  float offsetX;
  float offsetY;
  
  switch (superSamplingType_) {
    case SuperSamplingType::None: {
      RtcRayHitIor ray = shootRay(static_cast<float>(x), static_cast<float>(y));
      finalColor = traceRay(ray, recursionDepth_);
      break;
    }
    case SuperSamplingType::Uniform: {
      offsetX = -0.5f;
      offsetY = -0.5f;
      const float offsetAdditionX = 1.0f / static_cast<float>(samplingSizeX_);
      const float offsetAdditionY = 1.0f / static_cast<float>(samplingSizeY_);
  
      for (int i = 0; i < samplingSizeX_; i++) {
        for (int j = 0; j < samplingSizeY_; j++) {
          RtcRayHitIor ray = shootRay(static_cast<float>(x) + offsetX, static_cast<float>(y) + offsetY);
          finalColor += traceRay(ray, recursionDepth_);
          offsetX += offsetAdditionX;
        }
        offsetX = -0.5f;
        offsetY += offsetAdditionY;
      }
      break;
    }
    case SuperSamplingType::RandomFinite: {
      offsetX = Random(-0.5f, 0.5f);
      offsetY = Random(-0.5f, 0.5f);
      
      for (int i = 0; i < (samplingSize_) - 1; i++) {
        RtcRayHitIor ray = shootRay(static_cast<float>(x) + offsetX, static_cast<float>(y) + offsetY);
        finalColor += traceRay(ray, recursionDepth_);
      }
      {
        RtcRayHitIor ray = shootRay(static_cast<float>(x), static_cast<float>(y));
        finalColor += traceRay(ray, recursionDepth_);
      }
      break;
    }
    case SuperSamplingType::RandomInfinite: {
      offsetX = Random(-0.5f, 0.5f);
      offsetY = Random(-0.5f, 0.5f);
      RtcRayHitIor ray = shootRay(static_cast<float>(x) + offsetX, static_cast<float>(y) + offsetY);
      pathTracerHelper->setPixel(y, x, traceRay(ray, recursionDepth_));
      
      finalColor = pathTracerHelper->getInterpolatedPixel(y, x);
    }
    case SuperSamplingType::SuperSamplingCount: {
      break;
    }
  }
  
  return finalColor / finalColor.a;
  
}

std::array<Color4f, 4> Shader::getPixel4(int x, int y) {
  RtcRayHitIor4 ray = shootRay4(static_cast<float>(x), static_cast<float>(y));
  std::array<Color4f, 4> retColors;
  
  for (int i = 0; i < 4; i++) {
    RtcRayHitIor rayHit;
    
    rayHit.ray.org_x = ray.ray.org_x[i];
    rayHit.ray.org_y = ray.ray.org_y[i];
    rayHit.ray.org_z = ray.ray.org_z[i];
    rayHit.ray.tnear = ray.ray.tnear[i];
    rayHit.ray.dir_x = ray.ray.dir_x[i];
    rayHit.ray.dir_y = ray.ray.dir_y[i];
    rayHit.ray.dir_z = ray.ray.dir_z[i];
    rayHit.ray.time = ray.ray.time[i];
    rayHit.ray.tfar = ray.ray.tfar[i];
    rayHit.ray.mask = ray.ray.mask[i];
    rayHit.ray.id = ray.ray.id[i];
    rayHit.ray.flags = ray.ray.flags[i];
    rayHit.hit.geomID = ray.hit.geomID[i];
    rayHit.hit.primID = ray.hit.primID[i];
    rayHit.hit.Ng_x = ray.hit.Ng_x[i];
    rayHit.hit.Ng_y = ray.hit.Ng_y[i];
    rayHit.hit.Ng_z = ray.hit.Ng_z[i];
    
    retColors[i] = traceRay(rayHit, recursionDepth_);
  }
  
  return retColors;
}

std::array<Color4f, 8> Shader::getPixel8(int x, int y) {
  RtcRayHitIor8 ray = shootRay8(static_cast<float>(x), static_cast<float>(y));
  std::array<Color4f, 8> retColors;
  
  for (int i = 0; i < 8; i++) {
    RtcRayHitIor rayHit;
    
    rayHit.ray.org_x = ray.ray.org_x[i];
    rayHit.ray.org_y = ray.ray.org_y[i];
    rayHit.ray.org_z = ray.ray.org_z[i];
    rayHit.ray.tnear = ray.ray.tnear[i];
    rayHit.ray.dir_x = ray.ray.dir_x[i];
    rayHit.ray.dir_y = ray.ray.dir_y[i];
    rayHit.ray.dir_z = ray.ray.dir_z[i];
    rayHit.ray.time = ray.ray.time[i];
    rayHit.ray.tfar = ray.ray.tfar[i];
    rayHit.ray.mask = ray.ray.mask[i];
    rayHit.ray.id = ray.ray.id[i];
    rayHit.ray.flags = ray.ray.flags[i];
    rayHit.hit.geomID = ray.hit.geomID[i];
    rayHit.hit.primID = ray.hit.primID[i];
    rayHit.hit.Ng_x = ray.hit.Ng_x[i];
    rayHit.hit.Ng_y = ray.hit.Ng_y[i];
    rayHit.hit.Ng_z = ray.hit.Ng_z[i];
    
    retColors[i] = traceRay(rayHit, recursionDepth_);
  }
  
  return retColors;
}

std::array<Color4f, 16> Shader::getPixel16(int x, int y) {
  RtcRayHitIor16 ray = shootRay16(static_cast<float>(x), static_cast<float>(y));
  std::array<Color4f, 16> retColors;
  
  for (int i = 0; i < 16; i++) {
    RtcRayHitIor rayHit;
    
    rayHit.ray.org_x = ray.ray.org_x[i];
    rayHit.ray.org_y = ray.ray.org_y[i];
    rayHit.ray.org_z = ray.ray.org_z[i];
    rayHit.ray.tnear = ray.ray.tnear[i];
    rayHit.ray.dir_x = ray.ray.dir_x[i];
    rayHit.ray.dir_y = ray.ray.dir_y[i];
    rayHit.ray.dir_z = ray.ray.dir_z[i];
    rayHit.ray.time = ray.ray.time[i];
    rayHit.ray.tfar = ray.ray.tfar[i];
    rayHit.ray.mask = ray.ray.mask[i];
    rayHit.ray.id = ray.ray.id[i];
    rayHit.ray.flags = ray.ray.flags[i];
    rayHit.hit.geomID = ray.hit.geomID[i];
    rayHit.hit.primID = ray.hit.primID[i];
    rayHit.hit.Ng_x = ray.hit.Ng_x[i];
    rayHit.hit.Ng_y = ray.hit.Ng_y[i];
    rayHit.hit.Ng_z = ray.hit.Ng_z[i];
    
    retColors[i] = traceRay(rayHit, recursionDepth_);
  }
  
  return retColors;
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
    const Vector3 rayDir = glm::normalize(Vector3(rayHit.ray.dir_x, rayHit.ray.dir_y, rayHit.ray.dir_z));
    return Color4f(sphericalMap_->texel(rayDir), 1.0f);
  }
  
  return Color4f((defaultBgColor_)->x, (defaultBgColor_)->y, (defaultBgColor_)->z, 0.0f);
}

RtcRayHitIor
Shader::generateRay(const glm::vec3 &origin,
                    const glm::vec3 &direction,
                    const float tnear) {
  RtcRayHitIor rayHit;
  
  rayHit.ray.org_x = origin.x;     // x coordinate of ray origin
  rayHit.ray.org_y = origin.y;     // y coordinate of ray origin
  rayHit.ray.org_z = origin.z;     // z coordinate of ray origin
  rayHit.ray.tnear = tnear;        // start of ray segment
  rayHit.ray.dir_x = direction.x;  // x coordinate of ray direction
  rayHit.ray.dir_y = direction.y;  // y coordinate of ray direction
  rayHit.ray.dir_z = direction.z;  // z coordinate of ray direction
  rayHit.ray.time = 0;             // time of this ray for motion blur
  rayHit.ray.tfar = FLT_MAX;       // end of ray segment (set to hit distance)
  rayHit.ray.mask = 0;             // ray mask
  rayHit.ray.id = 0;               // ray ID
  rayHit.ray.flags = 0;            // ray flags
  rayHit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
  rayHit.hit.primID = RTC_INVALID_GEOMETRY_ID;
  rayHit.hit.Ng_x = 0.0f; // geometry normal
  rayHit.hit.Ng_y = 0.0f;
  rayHit.hit.Ng_z = 0.0f;
  
  // intersect ray with the scene
  RTCIntersectContext context;
  rtcInitIntersectContext(&context);
  rtcIntersect1(*rtcScene_, &context, &rayHit);
  return rayHit;
}


RtcRayHitIor4
Shader::generateRay4(const glm::vec3 &origin,
                     const std::array<const glm::vec3, 4> &direction,
                     float tnear) {
  RtcRayHitIor4 rayHit;
  for (int i = 0; i < 4; i++) {
    rayHit.ray.org_x[i] = origin.x;     // x coordinate of ray origin
    rayHit.ray.org_y[i] = origin.y;     // y coordinate of ray origin
    rayHit.ray.org_z[i] = origin.z;     // z coordinate of ray origin
    rayHit.ray.tnear[i] = tnear;        // start of ray segment
    rayHit.ray.dir_x[i] = direction[i].x;  // x coordinate of ray direction
    rayHit.ray.dir_y[i] = direction[i].y;  // y coordinate of ray direction
    rayHit.ray.dir_z[i] = direction[i].z;  // z coordinate of ray direction
    rayHit.ray.time[i] = 0;             // time of this ray for motion blur
    rayHit.ray.tfar[i] = FLT_MAX;       // end of ray segment (set to hit distance)
    rayHit.ray.mask[i] = 0;             // ray mask
    rayHit.ray.id[i] = 0;               // ray ID
    rayHit.ray.flags[i] = 0;            // ray flags
    rayHit.hit.geomID[i] = RTC_INVALID_GEOMETRY_ID;
    rayHit.hit.primID[i] = RTC_INVALID_GEOMETRY_ID;
    rayHit.hit.Ng_x[i] = 0.0f; // geometry normal
    rayHit.hit.Ng_y[i] = 0.0f;
    rayHit.hit.Ng_z[i] = 0.0f;
  }
  
  RTCIntersectContext context;
  rtcInitIntersectContext(&context);
  RTC_ALIGN(16)
  int valid[4] = {-1, -1, -1, -1};
  rtcIntersect4(valid, *rtcScene_, &context, &rayHit);
  return rayHit;
}

RtcRayHitIor8
Shader::generateRay8(const glm::vec3 &origin,
                     const std::array<const glm::vec3, 8> &direction,
                     float tnear) {
  RtcRayHitIor8 rayHit;
  for (int i = 0; i < 8; i++) {
    rayHit.ray.org_x[i] = origin.x;     // x coordinate of ray origin
    rayHit.ray.org_y[i] = origin.y;     // y coordinate of ray origin
    rayHit.ray.org_z[i] = origin.z;     // z coordinate of ray origin
    rayHit.ray.tnear[i] = tnear;        // start of ray segment
    rayHit.ray.dir_x[i] = direction[i].x;  // x coordinate of ray direction
    rayHit.ray.dir_y[i] = direction[i].y;  // y coordinate of ray direction
    rayHit.ray.dir_z[i] = direction[i].z;  // z coordinate of ray direction
    rayHit.ray.time[i] = 0;             // time of this ray for motion blur
    rayHit.ray.tfar[i] = FLT_MAX;       // end of ray segment (set to hit distance)
    rayHit.ray.mask[i] = 0;             // ray mask
    rayHit.ray.id[i] = 0;               // ray ID
    rayHit.ray.flags[i] = 0;            // ray flags
    rayHit.hit.geomID[i] = RTC_INVALID_GEOMETRY_ID;
    rayHit.hit.primID[i] = RTC_INVALID_GEOMETRY_ID;
    rayHit.hit.Ng_x[i] = 0.0f; // geometry normal
    rayHit.hit.Ng_y[i] = 0.0f;
    rayHit.hit.Ng_z[i] = 0.0f;
  }
  
  RTCIntersectContext context;
  rtcInitIntersectContext(&context);
  RTC_ALIGN(32)
  int valid[8] = {-1, -1, -1, -1, -1, -1, -1, -1};
  rtcIntersect8(valid, *rtcScene_, &context, &rayHit);
  return rayHit;
  
}

RtcRayHitIor16 Shader::generateRay16(const glm::vec3 &origin,
                                     const std::array<const glm::vec3, 16> &direction,
                                     float tnear) {
  RtcRayHitIor16 rayHit;
  for (int i = 0; i < 16; i++) {
    rayHit.ray.org_x[i] = origin.x;     // x coordinate of ray origin
    rayHit.ray.org_y[i] = origin.y;     // y coordinate of ray origin
    rayHit.ray.org_z[i] = origin.z;     // z coordinate of ray origin
    rayHit.ray.tnear[i] = tnear;        // start of ray segment
    rayHit.ray.dir_x[i] = direction[i].x;  // x coordinate of ray direction
    rayHit.ray.dir_y[i] = direction[i].y;  // y coordinate of ray direction
    rayHit.ray.dir_z[i] = direction[i].z;  // z coordinate of ray direction
    rayHit.ray.time[i] = 0;             // time of this ray for motion blur
    rayHit.ray.tfar[i] = FLT_MAX;       // end of ray segment (set to hit distance)
    rayHit.ray.mask[i] = 0;             // ray mask
    rayHit.ray.id[i] = 0;               // ray ID
    rayHit.ray.flags[i] = 0;            // ray flags
    rayHit.hit.geomID[i] = RTC_INVALID_GEOMETRY_ID;
    rayHit.hit.primID[i] = RTC_INVALID_GEOMETRY_ID;
    rayHit.hit.Ng_x[i] = 0.0f; // geometry normal
    rayHit.hit.Ng_y[i] = 0.0f;
    rayHit.hit.Ng_z[i] = 0.0f;
  }
  
  RTCIntersectContext context;
  rtcInitIntersectContext(&context);
  RTC_ALIGN(64)
  int valid[16] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
  rtcIntersect16(valid, *rtcScene_, &context, &rayHit);
  return rayHit;
}

glm::vec3 Shader::getNormal(RTCGeometry geometry, const RtcRayHitIor &rayHit) {
  glm::vec3 normal;
  rtcInterpolate0(geometry, rayHit.hit.primID, rayHit.hit.u, rayHit.hit.v,
                  RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 0, &normal.x, 3);
  return normal;
  
}

glm::vec2 Shader::getTexCoords(RTCGeometry geometry, const RtcRayHitIor &rayHit) {
  glm::vec2 tex_coord;
  rtcInterpolate0(geometry, rayHit.hit.primID, rayHit.hit.u, rayHit.hit.v,
                  RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 1, &tex_coord.x, 2);
  return tex_coord;
}

float Shader::fresnel(const float n1, const float n2, const float Q1, const float Q2) {
  float Rs = sqr((n1 * Q2 - n2 * Q1) / (n1 * Q2 + n2 * Q1));
  float Rp = sqr((n1 * Q1 - n2 * Q2) / (n1 * Q1 + n2 * Q2));
  
  return 0.5f * (Rs + Rp);
}


float Shader::shadow(const glm::vec3 &pos, const glm::vec3 &lightDir, const float dist) {
  RTCHit hit;
  hit.geomID = RTC_INVALID_GEOMETRY_ID;
  hit.primID = RTC_INVALID_GEOMETRY_ID;
  
  RTCRay ray = RTCRay();
  ray.org_x = pos.x; // ray origin
  ray.org_y = pos.y;
  ray.org_z = pos.z;
  
  ray.dir_x = lightDir.x;
  ray.dir_y = lightDir.y;
  ray.dir_z = lightDir.z;
  
  ray.tnear = 0.1f;
  ray.tfar = dist;
  
  ray.time = 0.0f;
  
  ray.mask = 0; // can be used to mask out some geometries for some rays
  ray.id = 0; // identify a ray inside a callback function
  ray.flags = 0; // reserved
  
  RTCIntersectContext context;
  rtcInitIntersectContext(&context);
  rtcOccluded1(*rtcScene_, &context, &ray);
  
  if (ray.tfar < dist) {
    return 0.0;
  } else {
    return 1.0;
  }
}