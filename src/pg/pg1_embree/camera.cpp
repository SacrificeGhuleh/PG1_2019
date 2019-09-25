#include "stdafx.h"
#include "camera.h"

Camera::Camera(const int width, const int height, const float fov_y,
               const Vector3 view_from, const Vector3 view_at) {
  width_ = width;
  height_ = height;
  fov_y_ = fov_y;
  
  view_from_ = view_from;
  view_at_ = view_at;
  
  f_y_ = static_cast<float>(height_) / (2.f * tanf(fov_y_ / 2.f));
  
  Vector3 z_c = view_from_ - view_at_;
  z_c.Normalize();
  Vector3 x_c = up_.CrossProduct(z_c);
  x_c.Normalize();
  Vector3 y_c = z_c.CrossProduct(x_c);
  y_c.Normalize();
  
  M_c_w_ = Matrix3x3(x_c, y_c, z_c);
}

RTCRay Camera::GenerateRay(const float x_i, const float y_i) const {
  RTCRay ray = RTCRay();
  Vector3 dc = Vector3((static_cast<float>(x_i) - (static_cast<float>(width_) * 0.5f)),
                       (static_cast<float>(height_) * 0.5f) - y_i,
                       -f_y_);
  
  dc.Normalize();
  Vector3 dc_w = M_c_w_ * dc;
  
  ray.org_x = view_from_.x; // ray origin
  ray.org_y = view_from_.y;
  ray.org_z = view_from_.z;
  ray.tnear = FLT_MIN; // start of ray segment
  
  ray.dir_x = dc_w.x; // ray direction
  ray.dir_y = dc_w.y;
  ray.dir_z = dc_w.z;
  ray.time = 0.0f; // time of this ray for motion blur
  
  ray.tfar = FLT_MAX; // end of ray segment (set to hit distance)
  
  ray.mask = 0; // can be used to mask out some geometries for some rays
  ray.id = 0; // identify a ray inside a callback function
  ray.flags = 0; // reserved
  
  return ray;
}
