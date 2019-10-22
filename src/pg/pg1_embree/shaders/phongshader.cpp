//
// Created by zvone on 26-Sep-19.
//
#include <stdafx.h>
#include <math/vector.h>
#include <geometry/material.h>
#include <shaders/phongshader.h>
#include <engine/light.h>
#include <engine/camera.h>
#include <glm/geometric.hpp>


bool PhongShader::phongAmbient_ = true;
bool PhongShader::phongDiffuse_ = true;
bool PhongShader::phongSpecular_ = true;
float PhongShader::specularStrength_ = 1.f;
float PhongShader::ambientValue_ = 1.f;


PhongShader::PhongShader(Camera *camera, Light *light, RTCScene *rtcScene, std::vector<Surface *> *surfaces,
                         std::vector<Material *> *materials) :
    Shader(camera, light, rtcScene, surfaces, materials) {}


Color4f PhongShader::traceRay(const RtcRayHitIor &rayHit, int depth) {
  if (rayHit.hit.geomID == RTC_INVALID_GEOMETRY_ID) {
    return getBackgroundColor(rayHit);
  }
  
  const RTCGeometry geometry = rtcGetGeometry(*rtcScene_, rayHit.hit.geomID);
  // get interpolated normal
  Normal3f normal = glm::normalize(getNormal(geometry, rayHit));
  // and texture coordinates
  const Coord2f tex_coord = getTexCoords(geometry, rayHit);
  
  //Acquire material from hit object
  Material *material = static_cast<Material *>(rtcGetGeometryUserData(geometry));
  
  
  if (phongAmbient_ || phongDiffuse_ || phongSpecular_) {
    //ambient
    Vector3 ambient = material->ambient;
    
    //diffuse
    Vector3 origin(rayHit.ray.org_x, rayHit.ray.org_y, rayHit.ray.org_z);
    Vector3 direction(rayHit.ray.dir_x, rayHit.ray.dir_y, rayHit.ray.dir_z);
    Vector3 worldPos = origin + direction * rayHit.ray.tfar;
    Vector3 lightDir = light_->getPosition() - worldPos;
    
    lightDir = glm::normalize(lightDir);
    
    float diff = glm::dot(normal, lightDir);
    
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
    viewDir = glm::normalize(viewDir);
    Vector3 reflectDir = glm::reflect(normal, lightDir);
    float spec = powf(glm::dot(viewDir, reflectDir), material->shininess);
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
