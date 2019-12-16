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

glm::vec3 Shader::getDiffuseColor(const Material *material, const glm::vec2 &tex_coord) {
  Texture *diffTexture = material->get_texture(Material::kDiffuseMapSlot);
  glm::vec3 geomDiffuse;
  
  if (diffTexture != nullptr) {
    glm::vec2 correctedTexCoord = tex_coord;
    
    if (flipTextureU_) correctedTexCoord.x = 1.f - correctedTexCoord.x;
    if (flipTextureV_) correctedTexCoord.y = 1.f - correctedTexCoord.y;
    
    geomDiffuse = diffTexture->get_texel(correctedTexCoord.x, correctedTexCoord.y);
  } else {
    geomDiffuse = material->diffuse;
  }
  return geomDiffuse;
}

glm::vec4 Shader::getPixel(const int x, const int y) {
  glm::vec4 finalColor(0, 0, 0, 0);
  
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

void Shader::setDefaultBgColor(glm::vec4 *defaultBgColor) {
  defaultBgColor_ = defaultBgColor;
}

glm::vec4 Shader::traceRay(const RtcRayHitIor &rayHit, int depth) {
  if (rayHit.hit.geomID == RTC_INVALID_GEOMETRY_ID) {
    return getBackgroundColor(rayHit);
  }
  
  return glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);
}

void Shader::setSphericalMap(SphericalMap *sphericalMap) {
  sphericalMap_ = sphericalMap;
}

glm::vec4 Shader::getBackgroundColor(const RtcRayHitIor &rayHit) {
  if (sphericalMap_ != nullptr && sphereMap_) {
    const glm::vec3 rayDir = glm::normalize(glm::vec3(rayHit.ray.dir_x, rayHit.ray.dir_y, rayHit.ray.dir_z));
    return glm::vec4(sphericalMap_->texel(rayDir), 1.0f);
  }
  
  return glm::vec4((defaultBgColor_)->x, (defaultBgColor_)->y, (defaultBgColor_)->z, 1.0f);
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

glm::vec3 Shader::hemisphereSampling(const glm::vec3 &normal, float &pdf) {
  const float M_2PI = 2.f * M_PI;
  const float randomU = Random();
  const float randomV = Random();
  
  const float x = 2.f * cosf(M_2PI * randomU) * sqrtf(randomV * (1.f - randomV));
  const float y = 2.f * sinf(M_2PI * randomU) * sqrtf(randomV * (1.f - randomV));
  const float z = 1.f - 2.f * randomV;
  
  glm::vec3 omegaI = glm::normalize(glm::vec3(x, y, z));
  
  if (glm::dot(omegaI, normal) < 0) {
//    omegaI *= -1;
    omegaI = -omegaI;
  }
  pdf = 1.f / 2.f * M_PI;
  
  return omegaI;
}