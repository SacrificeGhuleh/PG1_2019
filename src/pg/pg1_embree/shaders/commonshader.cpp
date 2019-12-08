//
// Created by zvone on 24-Oct-19.
//

#include <stdafx.h>

#include <shaders/commonshader.h>

#include <math/vector.h>
#include <math/mymath.h>

#include <engine/light.h>
#include <engine/camera.h>

#include <geometry/material.h>
#include <utils/utils.h>


ShadingType CommonShader::useShader = ShadingType::None;
float CommonShader::ior = 1;

CommonShader::CommonShader(Camera *camera,
                           Light *light,
                           RTCScene *rtcScene,
                           std::vector<Surface *> *surfaces,
                           std::vector<Material *> *materials) :
    Shader(camera, light, rtcScene, surfaces, materials) {}

Color4f CommonShader::traceRay(const RtcRayHitIor &rayHit, int depth) {
  if (rayHit.hit.geomID == RTC_INVALID_GEOMETRY_ID) {
    return getBackgroundColor(rayHit);
  }
  
  const RTCGeometry geometry = rtcGetGeometry(*rtcScene_, rayHit.hit.geomID);
  // get interpolated normal
  const Normal3f normal = glm::normalize(getNormal(geometry, rayHit));
  // and texture coordinates
  const Coord2f tex_coord = getTexCoords(geometry, rayHit);
  
  //Acquire material from hit object
  Material *material = static_cast<Material *>(rtcGetGeometryUserData(geometry));
  
  
  const Vector3 origin(rayHit.ray.org_x, rayHit.ray.org_y, rayHit.ray.org_z);
  const Vector3 direction(rayHit.ray.dir_x, rayHit.ray.dir_y, rayHit.ray.dir_z);
  const Vector3 worldPos = origin + direction * rayHit.ray.tfar;
  
  //sum results
  Vector3 resultColor(0, 0, 0);
  
  if (depth <= 0) {
    resultColor = traceMaterial<ShadingType::Phong>(rayHit, material, tex_coord, normal, worldPos, depth);
    return Color4f(0,0,0, 1.0f);
//    return Color4f(resultColor, 1.0f);
  }
  
  const ShadingType selectedShader = (useShader == ShadingType::None) ? material->shadingType : useShader;
  
  switch (selectedShader) {
    case ShadingType::None: {
      resultColor = traceMaterial<ShadingType::None>(rayHit, material, tex_coord, normal, worldPos, depth);
      break;
    }
    
    case ShadingType::Glass: {
      resultColor = traceMaterial<ShadingType::Glass>(rayHit, material, tex_coord, normal, worldPos, depth);
      break;
    }
    case ShadingType::Lambert: {
      resultColor = traceMaterial<ShadingType::Lambert>(rayHit, material, tex_coord, normal, worldPos, depth);
      break;
    }
    case ShadingType::Mirror: {
      resultColor = traceMaterial<ShadingType::Mirror>(rayHit, material, tex_coord, normal, worldPos, depth);
      break;
    }
    case ShadingType::Phong: {
      resultColor = traceMaterial<ShadingType::Phong>(rayHit, material, tex_coord, normal, worldPos, depth);
      break;
    }
    case ShadingType::PathTracing: {
      resultColor = traceMaterial<ShadingType::PathTracing>(rayHit, material, tex_coord, normal, worldPos, depth);
      break;
    }
    case ShadingType::Normals: {
      resultColor = traceMaterial<ShadingType::Normals>(rayHit, material, tex_coord, normal, worldPos, depth);
      break;
    };
    case ShadingType::TexCoords: {
      resultColor = traceMaterial<ShadingType::TexCoords>(rayHit, material, tex_coord, normal, worldPos, depth);
      break;
    };
  }
  
  return Color4f(resultColor, 1.0f);
}

template<ShadingType T>
Color4f CommonShader::traceMaterial(
    const RtcRayHitIor &rayHit,
    const Material *material,
    const Coord2f &tex_coord,
    const glm::vec3 &normal,
    const glm::vec3 &worldPos,
    int depth) {
  std::cout << "Warning, no material\n";
  return Color4f(1, 0, 1, 1);
}

glm::vec3 CommonShader::hemisphereSampling(glm::vec3 normal, float &pdf) {
  const float M_2PI = 2.f * M_PI;
  const float randomU = Random();
  const float randomV = Random();
  
  const float x = 2.f * cosf(M_2PI * randomU) * sqrtf(randomV * (1.f - randomV));
  const float y = 2.f * sinf(M_2PI * randomU) * sqrtf(randomV * (1.f - randomV));
  const float z = 1.f - 2.f * randomV;
  
  Vector3 omegaI = glm::normalize(Vector3(x, y, z));
  
  if (glm::dot(omegaI, normal) < 0) {
//    omegaI *= -1;
    omegaI = -omegaI;
  }
  const float pdfConst = 1. / 2. * M_PI;
  pdf = pdfConst;
  
  return omegaI;
}

template<>
Color4f CommonShader::traceMaterial<ShadingType::None>(
    const RtcRayHitIor &rayHit,
    const Material *material,
    const Coord2f &tex_coord,
    const glm::vec3 &normal,
    const glm::vec3 &worldPos,
    int depth) {
  return Color4f(1.f, 0.f, 0.f, 1.f);
}

template<>
Color4f CommonShader::traceMaterial<ShadingType::Glass>(
    const RtcRayHitIor &rayHit,
    const Material *material,
    const Coord2f &tex_coord,
    const glm::vec3 &normal,
    const glm::vec3 &worldPos,
    int depth) {
  float materialIor;
  if(useShader == ShadingType::Glass){
    materialIor = ior;
  }
  else{
    assert(material->ior >= 0);
    materialIor = material->ior;
  };
  
  if (depth <= 0) {
    return getBackgroundColor(rayHit);
  }
  
  Color4f reflected(0.f, 0.f, 0.f, 1.f);
  Color4f refracted(0.f, 0.f, 0.f, 1.f);
  
  glm::vec3 normalForGlass = glm::normalize(normal);
  const glm::vec3 rayDirection(rayHit.ray.dir_x, rayHit.ray.dir_y, rayHit.ray.dir_z);
  const glm::vec3 rayToObserver = -rayDirection;
  
  //n1 = ray ior
  //n2 = material ior
  //if n1 != vzduch than n2 = air
  float n1 = rayHit.ior;
  float n2 = materialIor;
  
  if (n1 != IOR_AIR) {
    n2 = IOR_AIR;
  }
  
  assert(n1 >= 0);
  assert(n2 >= 0);
  //0.64 1.54
  const float n1overn2 = (n1 / n2);
//cos1
  float Q1 = glm::dot(normalForGlass, rayToObserver);
  
  if (Q1 < 0) {
    normalForGlass = -normalForGlass;
    Q1 = glm::dot(normalForGlass, rayToObserver);
  }
  assert(Q1 >= 0);
  
  const glm::vec3 reflectDir = glm::normalize(
      (2.f * (rayToObserver * normalForGlass)) * normalForGlass - rayToObserver);
  
  RtcRayHitIor reflectedRayHit = generateRay(worldPos, reflectDir, tNear_);
  reflectedRayHit.ior = n2;
  
  const float tmp = 1.f - sqr(n1overn2) * (1.f - sqr(Q1));
  if (tmp < 0.f) {
    return traceRay(reflectedRayHit, depth - 1);
  }

//cos2
  const float Q2 = sqrtf(tmp);
  const glm::vec3 refractedDir = glm::normalize(
      (n1overn2 * rayDirection) + ((n1overn2 * Q1 - Q2) * normalForGlass));
// Fresnel
  const float R = Shader::fresnel(n1, n2, Q1, Q2);
  const float coefReflect = R;
  const float coefRefract = 1.f - R;
  
  RtcRayHitIor refractedRayHit = generateRay(worldPos, refractedDir, tNear_);
  refractedRayHit.ior = n2;
  
  reflected = traceRay(reflectedRayHit, depth - 1);
  refracted = traceRay(refractedRayHit, depth - 1);
  
  assert(coefReflect >= 0.f);
  assert(coefReflect <= 1.f);
  
  assert(coefRefract >= 0.f);
  assert(coefRefract <= 1.f);
  
  //C = [Crefl*R + Crefr*(1-R)] * TbeerLambert
  //TbeerLambert = {1,1,1} for air
  //TbeerLambert = {e^-Mr*l,e^-Mg*l,e^-Mb*l} for air, l = delka paprsku, M = absorpce materialu
  glm::vec3 TBeerLambert;
  if (rayHit.ior == IOR_AIR) {
    TBeerLambert = {1.f, 1.f, 1.f};
  } else {
    const float l = rayHit.ray.tfar;
//    const glm::vec3 M = {10, 0.5, 10};
    const glm::vec3 M = material->absorption;
    TBeerLambert = {
        expf(-M.r * l),
        expf(-M.g * l),
        expf(-M.b * l)
    };
  }
  return coefReflect * reflected + coefRefract * refracted * glm::vec4(TBeerLambert, 1.f);
}

template<>
Color4f CommonShader::traceMaterial<ShadingType::Lambert>(
    const RtcRayHitIor &rayHit,
    const Material *material,
    const Coord2f &tex_coord,
    const glm::vec3 &normal,
    const glm::vec3 &worldPos,
    int depth) {
  return Color4f(0.f, 0.f, 1.f, 1.f);
}

template<>
Color4f CommonShader::traceMaterial<ShadingType::Mirror>(
    
    const RtcRayHitIor &rayHit,
    const Material *material,
    const Coord2f &tex_coord,
    const glm::vec3 &normal,
    const glm::vec3 &worldPos,
    int depth) {
  
  //ambient
  Vector3 ambient = material->ambient;
  
  //diffuse
  Vector3 origin(rayHit.ray.org_x, rayHit.ray.org_y, rayHit.ray.org_z);
  Vector3 direction(rayHit.ray.dir_x, rayHit.ray.dir_y, rayHit.ray.dir_z);
//  Vector3 worldPos = origin + direction * rayHit.ray.tfar;
  Vector3 lightDir = light_->getPosition() - worldPos;
  glm::vec3 shaderNormal = glm::normalize(normal);
  
  lightDir = glm::normalize(lightDir);
  
  float diff = glm::dot(normal, lightDir);
  
  //Flip normal if invalid
  if (correctNormals_) {
    if (diff < 0) {
      shaderNormal *= -1.f;
      diff *= -1.f;
    }
  }
  
  Vector3 diffuse = diff * getDiffuseColor(material, tex_coord);
  
  //specular
  Vector3 viewDir = (origin - worldPos);
  viewDir = glm::normalize(viewDir);
  Vector3 reflectDir = glm::reflect(normal, lightDir);
  float spec = powf(glm::dot(viewDir, reflectDir), material->shininess);
  Vector3 specular(spec);
  
  Color4f reflected(0.f, 0.f, 0.f, 1.f);
  
  RtcRayHitIor reflectedRayHit = generateRay(worldPos, reflectDir, tNear_);
  reflected = traceRay(reflectedRayHit, depth - 1);
  
  //Vector4 C(diffuse + (specular * Vector3(reflected)), 1.0f);
  //return C;
  return reflected;
}

template<>
Color4f CommonShader::traceMaterial<ShadingType::Phong>(
    const RtcRayHitIor &rayHit,
    const Material *material,
    const Coord2f &tex_coord,
    const glm::vec3 &normal,
    const glm::vec3 &worldPos,
    int depth) {
  //ambient
  Vector3 ambient = material->ambient;
  
  //diffuse
  const Vector3 lightDir = glm::normalize(light_->getPosition() - worldPos);
  const Vector3 origin(rayHit.ray.org_x, rayHit.ray.org_y, rayHit.ray.org_z);
  
  const glm::vec3 rayDirection(rayHit.ray.dir_x, rayHit.ray.dir_y, rayHit.ray.dir_z);
  const glm::vec3 rayToObserver = -rayDirection;
  
  glm::vec3 normalForPhong = glm::normalize(normal);
  
  float diff = glm::dot(normalForPhong, lightDir);
  
  //Flip normal if invalid
  if (correctNormals_) {
    if (diff < 0) {
      normalForPhong *= -1.f;
      diff *= -1.f;
    }
  }
  
  Vector3 diffuse = diff * getDiffuseColor(material, tex_coord);
  
  //specular
  Vector3 viewDir = (origin - worldPos);
  viewDir = glm::normalize(viewDir);
  Vector3 phongReflectDir = glm::reflect(normalForPhong, lightDir);
  float spec = powf(glm::dot(viewDir, phongReflectDir), material->shininess);
  
  const glm::vec3 reflectDir = glm::normalize(
      (2.f * (rayToObserver * normalForPhong)) * normalForPhong - rayToObserver);
  
  Vector3 specular;
  if (depth <= 0) {
    specular = Vector3(spec);
  } else {
    RtcRayHitIor reflectedRayHit = generateRay(worldPos, reflectDir, tNear_);
    
    glm::vec4 reflected = traceRay(reflectedRayHit, depth - 1);
    specular = Vector3(reflected.x * spec, reflected.y * spec, reflected.z * spec);
  }
  
  //shadow
  float shadowVal = shadow(worldPos, lightDir, glm::l2Norm(light_->getPosition()));
  
  return Color4f(
      (ambient.x + (shadowVal * diffuse.x) + specular.x),
      (ambient.y + (shadowVal * diffuse.y) + specular.y),
      (ambient.z + (shadowVal * diffuse.z) + specular.z),
      1);
}

template<>
Color4f CommonShader::traceMaterial<ShadingType::PathTracing>(
    const RtcRayHitIor &rayHit,
    const Material *material,
    const Coord2f &tex_coord,
    const glm::vec3 &normal,
    const glm::vec3 &worldPos,
    int depth) {
  
  Color3f emmision = Color3f{material->emission.x, material->emission.y, material->emission.z};
  /*if (emmision.r != 0 && emmision.g != 0 && emmision.b != 0) {
    return Color4f(emmision.r, emmision.g, emmision.b, 1);
  }*/
  
  const Vector3 rayDir = Vector3(rayHit.ray.dir_x, rayHit.ray.dir_y, rayHit.ray.dir_z);
  const Vector3 rayView = Vector3(-rayDir.x, -rayDir.y, -rayDir.z);
  
  Vector3 lightDir = light_->getPosition() - worldPos;
  glm::vec3 shaderNormal = glm::normalize(normal);
  
  lightDir = glm::normalize(lightDir);
  
  float diff = glm::dot(normal, lightDir/*rayDir*/);
  
  //Flip normal if invalid
  if (correctNormals_) {
    if (diff < 0) {
      shaderNormal *= -1.f;
      diff *= -1.f;
    }
  }
  
  float pdf;
  const Vector3 omegaI = hemisphereSampling(shaderNormal, pdf);
  
  const RtcRayHitIor rayHitNew = generateRay(worldPos, omegaI);
  
  const Color4f reflColor = traceRay(rayHitNew, depth - 1);
  

  
  
  Vector3 diffuse = diff * getDiffuseColor(material, tex_coord);
  
  const Vector3 fR = diffuse * glm::vec3(1. / M_PI);
  
//  float shadowVal = shadow(worldPos, lightDir, glm::l2Norm(light_->getPosition()));
  
  emmision += glm::vec3(reflColor.x, reflColor.y, reflColor.z) * diffuse;
  
  return Color4f(emmision.r, emmision.g, emmision.b, 1);
  
  return Color4f(
      glm::vec3(reflColor.x, reflColor.y, reflColor.z) *
      glm::dot(shaderNormal, omegaI) *
      diffuse,
      1);
}

template<>
Color4f CommonShader::traceMaterial<ShadingType::Normals>(
    const RtcRayHitIor &rayHit,
    const Material *material,
    const Coord2f &tex_coord,
    const glm::vec3 &normal,
    const glm::vec3 &worldPos,
    int depth) {
  Vector3 lightDir = light_->getPosition() - worldPos;
  
  lightDir = glm::normalize(lightDir);
  glm::vec3 normalForShader = glm::normalize(normal);
  
  float diff = glm::dot(normal, lightDir);
  if (correctNormals_) {
    if (diff < 0) {
      normalForShader *= -1.f;
      diff *= -1.f;
    }
  }
  return Color4f(normalForShader, 1.0f);
}

template<>
Color4f CommonShader::traceMaterial<ShadingType::TexCoords>(
    const RtcRayHitIor &rayHit,
    const Material *material,
    const Coord2f &tex_coord,
    const glm::vec3 &normal,
    const glm::vec3 &worldPos,
    int depth) {
  return Color4f(tex_coord.x,tex_coord.y,1.0, 1.0f);
}