//
// Created by zvone on 03-Oct-19.
//

#include <engine/sphericalmap.h>
#include <cmath>
#include <math/mymath.h>
#include <geometry/texture.h>
#include <cassert>

SphericalMap::SphericalMap(std::string filename) {
//  texture_ = std::make_unique<Texture>(filename.c_str());
  texture_ = new Texture(filename.c_str());
  assert(texture_->height() != 0);
  assert(texture_->width() != 0);
}

Color3f SphericalMap::texel(const Vector3 &pos) {
  float theta = acosf(clamp(pos.z, -1.f, 1.f));
  float phi = atan2f(pos.y, pos.x) + static_cast<float>(M_PI);

  float u = 1.0f - phi * 0.5f * static_cast<float>(M_1_PI);
  float v = theta * static_cast<float>(M_1_PI);

  return texture_->get_texel(u, v);
//  return Color3f(0, 1, 0);
}
