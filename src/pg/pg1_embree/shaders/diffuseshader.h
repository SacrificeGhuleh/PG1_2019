//
// Created by zvone on 28-Sep-19.
//

#ifndef PG1_2019_DIFFUSESHADER_H
#define PG1_2019_DIFFUSESHADER_H

#include <shaders/shader.h>
#include <stdafx.h>

class DiffuseShader : public Shader {
public:
  Color4f traceRay(const RtcRayHitIor &rayHit, int depth = 0) override;
  
  DiffuseShader(Camera *camera, Light *light, RTCScene *rtcscene, std::vector<Surface *> *surfaces,
                std::vector<Material *> *materials);
};


#endif //PG1_2019_DIFFUSESHADER_H
