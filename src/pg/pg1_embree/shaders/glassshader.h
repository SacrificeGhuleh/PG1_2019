//
// Created by zvone on 03-Oct-19.
//

#ifndef PG1_2019_GLASSSHADER_H
#define PG1_2019_GLASSSHADER_H


#include <shaders/shader.h>
#include <stdafx.h>

class GlassShader : public Shader {
public:
  GlassShader(Camera *camera, Light *light, RTCScene *rtcscene, std::vector<Surface *> *surfaces,
              std::vector<Material *> *materials);
  
  virtual Color4f traceRay(const RTCRayHit &rayHit, int depth) override;
  
  static float reflectivityCoef;
};


#endif //PG1_2019_GLASSSHADER_H
