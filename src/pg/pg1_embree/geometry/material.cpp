#include <stdafx.h>
#include <geometry/material.h>
#include <utils/utils.h>

const char Material::kDiffuseMapSlot = 0;
const char Material::kSpecularMapSlot = 1;
const char Material::kNormalMapSlot = 2;
const char Material::kOpacityMapSlot = 3;

Material::Material() {
  // defaultní materiál
  ambient = Vector3(0.1f, 0.1f, 0.1f);
  diffuse = Vector3(0.4f, 0.4f, 0.4f);
  specular = Vector3(0.8f, 0.8f, 0.8f);
  
  emission = Vector3(0.0f, 0.0f, 0.0f);
  
  absorption = Vector3(0.f, 0.f, 0.f);
  
  reflectivity = static_cast<float>( 0.99 );
  shininess = 1;
  
  ior = -1;
  
  memset(textures_, 0, sizeof(*textures_) * NO_TEXTURES);
  
  name_ = "default";
}

Material::Material(std::string &name, const Vector3 &ambient, const Vector3 &diffuse,
                   const Vector3 &specular, const Vector3 &emission, const float reflectivity,
                   const float shininess, const float ior, Texture **textures, const int no_textures) {
  name_ = name;
  
  this->ambient = ambient;
  this->diffuse = c_linear(diffuse);
  this->specular = specular;
  
  this->emission = emission;
  
  this->reflectivity = reflectivity;
  this->shininess = shininess;
  
  this->ior = ior;
  
  if (textures) {
    memcpy(textures_, textures, sizeof(textures) * no_textures);
  }
  
  shadingType = ShadingType::None;
}

Material::~Material() {
  for (int i = 0; i < NO_TEXTURES; ++i) {
    if (textures_[i]) {
      delete[] textures_[i];
      textures_[i] = nullptr;
    }
  }
}

void Material::set_name(const char *name) {
  name_ = std::string(name);
}

std::string Material::get_name() const {
  return name_;
}

void Material::set_texture(const int slot, Texture *texture) {
  textures_[slot] = texture;
}

Texture *Material::get_texture(const int slot) const {
  return textures_[slot];
}
