//
// Created by zvone on 26-Sep-19.
//

#ifndef PG1_2019_SHADER_H
#define PG1_2019_SHADER_H

#include <vector>
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
  
  virtual Color4f getPixel(const int x, const int y);
  
  virtual Color4f traceRay(const RtcRayHitIor &rayHit, int depth = 0);
  
  RtcRayHitIor shootRay(const float x, const float y);
  
  Color3f getDiffuseColor(const Material *material, const Coord2f &tex_coord);
  
  static float fresnel(const float n1, const float n2, const float Q1, const float Q2); /*{
    float Rs = ((n1 * Q2 - n2 * Q1) / (n1 * Q2 + n2 * Q1)) * ((n1 * Q2 - n2 * Q1) / (n1 * Q2 + n2 * Q1));
    float Rp = ((n1 * Q1 - n2 * Q2) / (n1 * Q1 + n2 * Q2)) * ((n1 * Q1 - n2 * Q2) / (n1 * Q1 + n2 * Q2));
    
    return 0.5f * (Rs + Rp);
  }*/

protected:
  
  RtcRayHitIor generateRay(const glm::vec3 &origin, const glm::vec3 &direction, const float tnear = tNear_);
  
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

public:
  void setSphericalMap(SphericalMap *sphericalMap);
  
  void setDefaultBgColor(Color4f *defaultBgColor);
  
  static bool flipTextureU_;
  static bool flipTextureV_;
  static bool supersampling_;
  static bool sphereMap_;
  static bool correctNormals_;
  static int samplingSize_;
  static int recursionDepth_;
  static float tNear_;
  static int visualizeX_;
  static int visualizeY_;
  
};


#endif //PG1_2019_SHADER_H
