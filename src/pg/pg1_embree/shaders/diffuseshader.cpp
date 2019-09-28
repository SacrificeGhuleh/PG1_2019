//
// Created by zvone on 28-Sep-19.
//

#include <stdafx.h>
#include <math/vector.h>
#include <geometry/material.h>
#include <shaders/diffuseshader.h>
#include <engine/light.h>
#include <engine/camera.h>


DiffuseShader::DiffuseShader(Camera *camera, Light *light, RTCScene *rtcscene, std::vector<Surface *> *surfaces,
                             std::vector<Material *> *materials) : Shader(camera, light, rtcscene, surfaces,
                                                                          materials) {}


Color4f DiffuseShader::traceRay(const RTCRayHit &rayHit) {
  if(rayHit.hit.geomID == RTC_INVALID_GEOMETRY_ID){
    return *defaultBgColor_;
  }
  
  RTCGeometry geometry = rtcGetGeometry(*rtcScene_, rayHit.hit.geomID);
  /**
   * Acquire material from hit object
   */
  Material *material = (Material *) (rtcGetGeometryUserData(geometry));
  return Color4f(material->diffuse, 1.0f);
}
