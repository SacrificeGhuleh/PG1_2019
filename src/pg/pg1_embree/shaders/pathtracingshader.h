//
// Created by zvone on 24-Oct-19.
//

#ifndef PG1_2019_PATHTRACINGSHADER_H
#define PG1_2019_PATHTRACINGSHADER_H

#include <shaders/shader.h>

class PathTracingShader :public Shader {
public:
  PathTracingShader(Camera *camera, Light *light, RTCScene *rtcscene, std::vector<Surface *> *surfaces,
                    std::vector<Material *> *materials);
  
  
  Color4f traceRay(const RtcRayHitIor &rayHit, int depth = 0) override;
  
  virtual Color4f getPixel(int x, int y) override;
  
  glm::vec3 hemisphereSampling(glm::vec3 normal, float& pdf);
  static int samples_;
  static bool useParentGetPixel_;
};


#endif //PG1_2019_PATHTRACINGSHADER_H
