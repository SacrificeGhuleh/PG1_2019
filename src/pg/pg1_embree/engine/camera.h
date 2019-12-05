#ifndef CAMERA_H_
#define CAMERA_H_

#include <math/vector.h>
#include <math/matrix3x3.h>

/*! \class Camera
\brief A simple pin-hole camera.

\author Tomáš Fabián
\version 1.0
\date 2018
*/
class Camera {
public:
  Camera() = default;
  
  Camera(int width, int height, float fov_y,
         const Vector3 &view_from, Vector3 view_at);
  
  /* generate primary ray, top-left pixel image coordinates (xi, yi) are in the range <0, 1) x <0, 1) */
  RTCRay GenerateRay(const float xi, const float yi);
  
  
  int getWidth() const;
  
  int getHeight() const;
  
  float getFovY() const;
  
  const Vector3 getRayDirection(float x_i, float y_i);
  
  const Vector3 &getViewFrom() const;
  
  const Vector3 &getViewAt() const;
  
  const Vector3 &getUp() const;
  
  float getFY() const;
  
  const Matrix3x3 &getMCW();
  
  Vector3 view_from_{}; // ray origin or eye or O
  float f_y_{1.0f}; // focal lenght (px)
private:
  int width_{640}; // image width (px)
  
  int height_{480};  // image height (px)
  float fov_y_{0.785f}; // vertical field of view (rad)
  Vector3 view_at_{}; // target T
  
  Vector3 up_{Vector3(0.0f, 0.0f, 1.0f)}; // up vector
  
  Matrix3x3 M_c_w_{}; // transformation matrix from CS -> WS
};

#endif
