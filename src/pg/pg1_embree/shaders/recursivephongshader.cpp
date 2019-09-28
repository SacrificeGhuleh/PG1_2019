//
// Created by zvone on 28-Sep-19.
//

#include <stdafx.h>
#include <math/vector.h>
#include <geometry/material.h>
#include <shaders/recursivephongshader.h>
#include <engine/light.h>
#include <engine/camera.h>

RecursivePhongShader::RecursivePhongShader(Camera *camera, Light *light, RTCScene *rtcscene,
                                           std::vector<Surface *> *surfaces, std::vector<Material *> *materials)
    : Shader(camera, light, rtcscene, surfaces, materials) {}
Color4f RecursivePhongShader::traceRay(const RTCRayHit &rayHit) {
  if(rayHit.hit.geomID == RTC_INVALID_GEOMETRY_ID){
    return *defaultBgColor_;
  }
  return Shader::traceRay(rayHit);
}
