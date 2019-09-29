//
// Created by zvone on 28-Sep-19.
//

#ifndef PG1_2019_NORMALSSHADER_H
#define PG1_2019_NORMALSSHADER_H

#include <shaders/shader.h>
#include <stdafx.h>

class NormalsShader : public Shader{
public:
  NormalsShader(Camera *camera, Light *light, RTCScene *rtcscene, std::vector<Surface *> *surfaces,
                std::vector<Material *> *materials);

  virtual Color4f traceRay(const RTCRayHit &rayHit, int depth = 0) override;
  static bool correctNormals_;
};


#endif //PG1_2019_NORMALSSHADER_H
