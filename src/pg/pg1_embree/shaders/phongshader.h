//
// Created by zvone on 26-Sep-19.
//

#ifndef PG1_2019_PHONGSHADER_H
#define PG1_2019_PHONGSHADER_H


#include <shaders/shader.h>

class PhongShader : public Shader {
public:
  PhongShader(Camera *camera, Light *light, RTCScene *rtcScene, std::vector<Surface *> *surfaces,
              std::vector<Material *> *materials);
  
  static bool phongAmbient_;
  static bool phongDiffuse_;
  static bool phongSpecular_;
  static float specularStrength_;
  static float ambientValue_;
  static bool correctNormals_;
  
  virtual Color4f traceRay(const RTCRayHit& rayHit) override;
};


#endif //PG1_2019_PHONGSHADER_H
