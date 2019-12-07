//
// Created by zvone on 28-Sep-19.
//

#ifndef PG1_2019_RECURSIVEPHONGSHADER_H
#define PG1_2019_RECURSIVEPHONGSHADER_H


#include <shaders/shader.h>

class RecursivePhongShader : public Shader {
public:
  Color4f traceRay(const RtcRayHitIor &rayHit, int depth = 0) override;
  
  //virtual Color4f getPixel(const int x, const int y) override;
  
  RecursivePhongShader(Camera *camera, Light *light, RTCScene *rtcscene, std::vector<Surface *> *surfaces,
                       std::vector<Material *> *materials);
  
  static int recursionDepth;
  static float reflectivityCoef;
};


#endif //PG1_2019_RECURSIVEPHONGSHADER_H
