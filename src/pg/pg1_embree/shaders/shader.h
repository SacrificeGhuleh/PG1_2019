//
// Created by zvone on 26-Sep-19.
//

#ifndef PG1_2019_SHADER_H
#define PG1_2019_SHADER_H

#include <vector>
#include <array>
#include <math/vector.h>
#include <engine/rtcrayhitior.h>

class Material;

class Camera;

class Light;

class Surface;

class Material;

class SphericalMap;

class Shader {
public:
  Shader(Camera *camera,
         Light *light,
         RTCScene *rtcscene,
         std::vector<Surface *> *surfaces,
         std::vector<Material *> *materials);
  
  //Gets one pixel
  virtual Color4f getPixel(int x, int y);
  
  //Gets 2x2 pixels
  virtual std::array<Color4f, 4> getPixel4(int x, int y);
  
  //Gets 2x4 pixels (2 height, 4 width)
  virtual std::array<Color4f, 8> getPixel8(int x, int y);
  
  //Gets 4x4 pixels (4 height, 4 width)
  virtual std::array<Color4f, 16> getPixel16(int x, int y);
  
  virtual Color4f traceRay(const RtcRayHitIor &rayHit, int depth = 0);
  
  RtcRayHitIor shootRay(float x, float y);
  
  RtcRayHitIor4 shootRay4(std::array<float, 4> &x, std::array<float, 4> &y);
  
  RtcRayHitIor8 shootRay8(std::array<float, 8> &x, std::array<float, 8> &y);
  
  RtcRayHitIor16 shootRay16(std::array<float, 16> &x, std::array<float, 16> &y);
  
  RtcRayHitIor4 shootRay4(float x, float y);
  
  RtcRayHitIor8 shootRay8(float x, float y);
  
  RtcRayHitIor16 shootRay16(float x, float y);
  
  Color3f getDiffuseColor(const Material *material, const Coord2f &tex_coord);
  
  static float fresnel(float n1, float n2, float Q1, float Q2);

protected:
  
  RtcRayHitIor generateRay(const glm::vec3 &origin, const glm::vec3 &direction, float tnear = tNear_);
  
  RtcRayHitIor4
  generateRay4(const glm::vec3 &origin,
               const std::array<const glm::vec3, 4> &direction,
               float tnear = tNear_);
  
  RtcRayHitIor8
  generateRay8(const glm::vec3 &origin,
               const std::array<const glm::vec3, 8> &direction,
               float tnear = tNear_);
  
  RtcRayHitIor16
  generateRay16(const glm::vec3 &origin,
                const std::array<const glm::vec3, 16> &direction,
                float tnear = tNear_);
  
  Color4f getBackgroundColor(const RtcRayHitIor &rayHit);
  
  Camera *camera_;
  Light *light_;
  RTCScene *rtcScene_;
  std::vector<Surface *> *surfaces_;
  std::vector<Material *> *materials_;
  Color4f *defaultBgColor_;
  SphericalMap *sphericalMap_;
  
  static glm::vec3 getNormal(RTCGeometry geometry, const RtcRayHitIor &rayHit);
  
  static glm::vec2 getTexCoords(RTCGeometry geometry, const RtcRayHitIor &rayHit);
  
  float shadow(const glm::vec3 &pos, const glm::vec3 &lightDir, const float dist);

public:
  void setSphericalMap(SphericalMap *sphericalMap);
  
  void setDefaultBgColor(Color4f *defaultBgColor);
  
  static bool flipTextureU_;
  static bool flipTextureV_;
  static bool supersampling_;
  static bool supersamplingRandom_;
  static bool sphereMap_;
  static bool correctNormals_;
  static int samplingSize_;
  static int recursionDepth_;
  static float tNear_;
  
  
};


#endif //PG1_2019_SHADER_H
