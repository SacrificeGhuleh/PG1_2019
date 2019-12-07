//
// Created by zvone on 28-Sep-19.
//

#ifndef PG1_2019_NORMALSSHADER_H
#define PG1_2019_NORMALSSHADER_H

#include <shaders/shader.h>

class NormalsShader : public Shader {
public:
  NormalsShader(Camera *camera, Light *light, RTCScene *rtcscene, std::vector<Surface *> *surfaces,
                std::vector<Material *> *materials);
  
  Color4f traceRay(const RtcRayHitIor &rayHit, int depth = 0) override;
};


#endif //PG1_2019_NORMALSSHADER_H
