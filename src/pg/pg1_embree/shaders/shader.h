//
// Created by zvone on 26-Sep-19.
//

#ifndef PG1_2019_SHADER_H
#define PG1_2019_SHADER_H

#include <math/vector.h>
#include <vector>

class Material;

class Camera;

class Light;

class Surface;

class Material;

class Shader {
public:
  Shader(Camera *camera, Light *light, RTCScene *rtcscene,
         std::vector<Surface *> *surfaces,
         std::vector<Material *> *materials);
  
  virtual Color4f getPixel(const int x, const int y);
  virtual Color4f traceRay(const RTCRayHit &rayHit, int depth = 0);
  
  RTCRayHit shootRay(const float x, const float y);
  
  Color3f getDiffuseColor(const Material *material, const Coord2f &tex_coord);

protected:
  Camera *camera_;
  Light *light_;
  RTCScene *rtcScene_;
  std::vector<Surface *> *surfaces_;
  std::vector<Material *> *materials_;
  Color4f *defaultBgColor_;
public:
  void setDefaultBgColor(Color4f *defaultBgColor);
  static bool flipTextureU_;
  static bool flipTextureV_;
  static bool supersampling_;
  static int samplingSize_;
  static int recursionDepth_;
};


#endif //PG1_2019_SHADER_H
