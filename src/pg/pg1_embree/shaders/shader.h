//
// Created by zvone on 26-Sep-19.
//

#ifndef PG1_2019_SHADER_H
#define PG1_2019_SHADER_H

#include <vector>
#include <array>
//#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <engine/rtcrayhitior.h>

class Material;

class Camera;

class Light;

class Surface;

class Material;

class SphericalMap;

class PathTracerHelper;

enum class SuperSamplingType : int {
  None = 0,
  Uniform,
  RandomFinite,
  RandomInfinite,
  SuperSamplingCount
};

class Shader {
public:
  Shader(Camera *camera,
         Light *light,
         RTCScene *rtcscene,
         std::vector<Surface *> *surfaces,
         std::vector<Material *> *materials);
  
  //Gets one pixel
  virtual glm::vec4 getPixel(int x, int y);
  
  RtcRayHitIor shootRay(float x, float y);
  
  
  glm::vec3 getDiffuseColor(const Material *material, const glm::vec2 &tex_coord);
  
  static float fresnel(float n1, float n2, float Q1, float Q2);

protected:
  
  RtcRayHitIor generateRay(const glm::vec3 &origin, const glm::vec3 &direction, float tnear = tNear_);
  
  virtual glm::vec4 traceRay(const RtcRayHitIor &rayHit, int depth);
  
  glm::vec4 getBackgroundColor(const RtcRayHitIor &rayHit);
  
  
  glm::vec3 hemisphereSampling(const glm::vec3 &normal, float &pdf);
  
  Camera *camera_;
  Light *light_;
  RTCScene *rtcScene_;
  std::vector<Surface *> *surfaces_;
  std::vector<Material *> *materials_;
  glm::vec4 *defaultBgColor_;
  SphericalMap *sphericalMap_;
  
  
  static glm::vec3 getNormal(RTCGeometry geometry, const RtcRayHitIor &rayHit);
  
  static glm::vec2 getTexCoords(RTCGeometry geometry, const RtcRayHitIor &rayHit);
  
  float shadow(const glm::vec3 &pos, const glm::vec3 &lightDir, const float dist);

//should be pivate, but for debug...
public:
  PathTracerHelper *pathTracerHelper;
  
  void setSphericalMap(SphericalMap *sphericalMap);
  
  void setDefaultBgColor(glm::vec4 *defaultBgColor);
  
  static bool flipTextureU_;
  static bool flipTextureV_;
  static bool supersampling_;
  static bool supersamplingRandom_;
  static bool sphereMap_;
  static bool correctNormals_;
  static int samplingSize_;
  static int samplingSizeX_;
  static int samplingSizeY_;
  static int recursionDepth_;
  static float tNear_;
  static bool changeShader_;
  
  static SuperSamplingType superSamplingType_;
};


#endif //PG1_2019_SHADER_H
