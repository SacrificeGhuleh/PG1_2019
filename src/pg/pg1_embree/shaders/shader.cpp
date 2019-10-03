//
// Created by zvone on 26-Sep-19.
//
#include <stdafx.h>
#include <shaders/shader.h>
#include <geometry/surface.h>
#include <engine/light.h>
#include <engine/camera.h>
#include <geometry/material.h>

bool Shader::flipTextureU_ = false;
bool Shader::flipTextureV_ = true;
bool Shader::supersampling_ = false;
int Shader::samplingSize_ = 3;
int Shader::recursionDepth_ = 1;

Shader::Shader(Camera *camera, Light *light, RTCScene *rtcScene, std::vector<Surface *> *surfaces,
               std::vector<Material *> *materials) : camera_{camera}, light_{light}, rtcScene_{rtcScene},
                                                     surfaces_{surfaces}, materials_{materials} {}

RTCRayHit Shader::shootRay(const float x, const float y) {
  RTCRay ray = camera_->GenerateRay(x, y);
  
  // setup a hit
  RTCHit hit;
  hit.geomID = RTC_INVALID_GEOMETRY_ID;
  hit.primID = RTC_INVALID_GEOMETRY_ID;
  hit.Ng_x = 0.0f; // geometry normal
  hit.Ng_y = 0.0f;
  hit.Ng_z = 0.0f;
  
  // merge ray and hit structures
  RTCRayHit rayHit;
  rayHit.ray = ray;
  rayHit.hit = hit;
  
  // intersect ray with the scene
  RTCIntersectContext context;
  rtcInitIntersectContext(&context);
  rtcIntersect1(*rtcScene_, &context, &rayHit);
  
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
    float offsetAddition = 1.0f / samplingSize_;
    
    for (int i = 0; i < samplingSize_; i++) {
      for (int j = 0; j < samplingSize_; j++) {
        RTCRayHit ray = shootRay(x + offsetX, y + offsetY);
        finalColor += traceRay(ray, recursionDepth_);
        offsetX += offsetAddition;
      }
      offsetX = -0.5f;
      offsetY += offsetAddition;
    }
    return finalColor / (float) (samplingSize_ * samplingSize_);
  } else {
    RTCRayHit ray = shootRay(x, y);
    return traceRay(ray, recursionDepth_);
  }
}

void Shader::setDefaultBgColor(Color4f *defaultBgColor) {
  defaultBgColor_ = defaultBgColor;
}

Color4f Shader::traceRay(const RTCRayHit &rayHit, int depth) {
  if (rayHit.hit.geomID == RTC_INVALID_GEOMETRY_ID) {
    return *defaultBgColor_;
  }
  
  return Color4f(0.0f, 1.0f, 1.0f, 1.0f);
}
