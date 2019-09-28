//
// Created by zvone on 28-Sep-19.
//

#ifndef PG1_2019_RECURSIVEPHONGSHADER_H
#define PG1_2019_RECURSIVEPHONGSHADER_H


#include <shaders/shader.h>
#include <stdafx.h>

class RecursivePhongShader : Shader {
  virtual Color4f traceRay(const RTCRayHit& rayHit) override;

public:
  RecursivePhongShader(Camera *camera, Light *light, RTCScene *rtcscene, std::vector<Surface *> *surfaces,
                       std::vector<Material *> *materials);
};


#endif //PG1_2019_RECURSIVEPHONGSHADER_H
