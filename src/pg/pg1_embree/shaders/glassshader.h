//
// Created by zvone on 03-Oct-19.
//

#ifndef PG1_2019_GLASSSHADER_H
#define PG1_2019_GLASSSHADER_H


#include <shaders/shader.h>

class GlassShader : public Shader {
public:
  GlassShader(Camera *camera, Light *light, RTCScene *rtcscene, std::vector<Surface *> *surfaces,
              std::vector<Material *> *materials);
  
  virtual Color4f traceRay(const RtcRayHitIor &rayHit, int depth) override;
  
  static bool addReflect_;
  static bool addRefract_;
  static bool addDiffuseToReflect_;
  static bool addDiffuseToRefract_;
  static bool addAttenuation;
  static float ior_;
  static float attenuation_;
};


#endif //PG1_2019_GLASSSHADER_H
