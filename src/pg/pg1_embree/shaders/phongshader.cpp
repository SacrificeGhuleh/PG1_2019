//
// Created by zvone on 26-Sep-19.
//
#include <stdafx.h>
#include <math/vector.h>
#include <geometry/material.h>
#include <shaders/phongshader.h>
#include <engine/light.h>
#include <engine/camera.h>


bool PhongShader::phongAmbient_ = true;
bool PhongShader::phongDiffuse_ = true;
bool PhongShader::phongSpecular_ = true;
float PhongShader::specularStrength_ = 1.f;
float PhongShader::ambientValue_ = 1.f;


PhongShader::PhongShader(Camera *camera, Light *light, RTCScene *rtcScene, std::vector<Surface *> *surfaces,
                         std::vector<Material *> *materials) : Shader(camera, light, rtcScene, surfaces, materials) {}


Color4f PhongShader::traceRay(const RTCRayHit &rayHit, int depth) {
  if(rayHit.hit.geomID == RTC_INVALID_GEOMETRY_ID){
    return getBackgroundColor(rayHit);
  }
  
  RTCGeometry geometry = rtcGetGeometry(*rtcScene_, rayHit.hit.geomID);
  Normal3f normal;
  // get interpolated normal
  rtcInterpolate0(geometry, rayHit.hit.primID, rayHit.hit.u, rayHit.hit.v,
                  RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 0, &normal.x, 3);
  // and texture coordinates
  Coord2f tex_coord;
  rtcInterpolate0(geometry, rayHit.hit.primID, rayHit.hit.u, rayHit.hit.v,
                  RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 1, &tex_coord.u, 2);
  
  /**
   * Acquire material from hit object
   */
  Material *material = (Material *) (rtcGetGeometryUserData(geometry));
  
  
  if (phongAmbient_ || phongDiffuse_ || phongSpecular_) {
    //ambient
    Vector3 ambient = material->ambient;
    
    //diffuse
    Vector3 origin(&rayHit.ray.org_x);
    Vector3 direction(&rayHit.ray.dir_x);
    Vector3 worldPos = origin + direction * rayHit.ray.tfar;
    Vector3 lightDir = light_->getPosition() - worldPos;
    lightDir.Normalize();
    normal.Normalize();
    float diff = normal.DotProduct(lightDir);
    if (correctNormals_) {
      //Flip normal if invalid
      if (diff < 0) {
        normal *= -1.f;
        diff *= -1.f;
      }
    }
    Vector3 diffuse = diff * getDiffuseColor(material, tex_coord);
    
    //specular
    Vector3 viewDir = (origin - worldPos);
    viewDir.Normalize();
    Vector3 reflectDir = normal.reflect(lightDir);
    float spec = powf(viewDir.DotProduct(reflectDir), material->shininess);
    Vector3 specular(specularStrength_ * spec);
    
    //sum results
    Vector3 resultColor(0.f);
    if (phongAmbient_) {
      resultColor = resultColor + ambient;
    }
    if (phongSpecular_) {
      resultColor = resultColor + specular;
    }
    if (phongDiffuse_) {
      resultColor = resultColor + diffuse;
    }
    return Color4f(resultColor, 1.0f);
  } else {
    return Color4f(material->diffuse, 1.0f);
  }
}
