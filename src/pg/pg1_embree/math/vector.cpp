//
// Created by zvone on 27-Sep-19.
//

//
///* *****************************************************************************
// * VECTOR 2
// * ****************************************************************************/
//
//
//Vector2::Vector2(const float *v) {
//  assert(v != nullptr);
//
//  x = v[0];
//  y = v[1];
//}
//
//void Vector2::Print() {
//  printf("(%0.3f, %0.3f)\n", x, y);
//}
//
//// --- operátory ------
//
//Vector2 operator-(const Vector2 &v) {
//  return Vector2(-v.x, -v.y);
//}
//
//Vector2 operator+(const Vector2 &u, const Vector2 &v) {
//  return Vector2(u.x + v.x, u.y + v.y);
//}
//
//Vector2 operator-(const Vector2 &u, const Vector2 &v) {
//  return Vector2(u.x - v.x, u.y - v.y);
//}
//
//Vector2 operator*(const Vector2 &v, const float a) {
//  return Vector2(a * v.x, a * v.y);
//}
//
//Vector2 operator*(const float a, const Vector2 &v) {
//  return Vector2(a * v.x, a * v.y);
//}
//
//Vector2 operator*(const Vector2 &u, const Vector2 &v) {
//  return Vector2(u.x * v.x, u.y * v.y);
//}
//
//Vector2 operator/(const Vector2 &v, const float a) {
//  return v * (1 / a);
//}
//
//void operator+=(Vector2 &u, const Vector2 &v) {
//  u.x += v.x;
//  u.y += v.y;
//}
//
//void operator-=(Vector2 &u, const Vector2 &v) {
//  u.x -= v.x;
//  u.y -= v.y;
//}
//
//void operator*=(Vector2 &v, const float a) {
//  v.x *= a;
//  v.y *= a;
//}
//
//void operator/=(Vector2 &v, const float a) {
//  const float r = 1 / a;
//
//  v.x *= r;
//  v.y *= r;
//}
//
//
///* *****************************************************************************
// * VECTOR 3
// * ****************************************************************************/
//
//
//Vector3::Vector3(const float *v) {
//  assert(v != nullptr);
//
//  x = v[0];
//  y = v[1];
//  z = v[2];
//}
//
//float Vector3::L2Norm() const {
//  return sqrtf(sqr(x) + sqr(y) + sqr(z));
//}
//
//float Vector3::SqrL2Norm() const {
//  return sqr(x) + sqr(y) + sqr(z);
//}
//
//void Vector3::Normalize() {
//  const float norm = SqrL2Norm();
//
//  if (norm != 0) {
//    const float rn = 1.f / sqrtf(norm);
//
//    x *= rn;
//    y *= rn;
//    z *= rn;
//  }
//}
//
//Vector3 Vector3::CrossProduct(const Vector3 &v) const {
//  return Vector3(
//      y * v.z - z * v.y,
//      z * v.x - x * v.z,
//      x * v.y - y * v.x);
//}
//
//Vector3 Vector3::Abs() const {
//  return Vector3(abs(x), abs(y), abs(z));
//}
//
//Vector3 Vector3::Max(const float a) const {
//  return Vector3(std::max(x, a), std::max(y, a), std::max(z, a));
//}
//
//float Vector3::DotProduct(const Vector3 &v) const {
//  return x * v.x + y * v.y + z * v.z;
//}
//
//char Vector3::LargestComponent(const bool absolute_value) {
//  const Vector3 d = (absolute_value) ? Vector3(abs(x), abs(y), abs(z)) : *this;
//
//  if (d.x > d.y) {
//    if (d.x > d.z) {
//      return 0;
//    } else {
//      return 2;
//    }
//  } else {
//    if (d.y > d.z) {
//      return 1;
//    } else {
//      return 2;
//    }
//  }
//
//  return -1;
//}
//
//void Vector3::Print() {
//  printf("(%0.3f, %0.3f, %0.3f)\n", x, y, z);
//  //printf( "_point %0.3f,%0.3f,%0.3f\n", x, y, z );
//}
//
//// --- operátory ------
//
//Vector3 operator-(const Vector3 &v) {
//  return Vector3(-v.x, -v.y, -v.z);
//}
//
//Vector3 operator+(const Vector3 &u, const Vector3 &v) {
//  return Vector3(u.x + v.x, u.y + v.y, u.z + v.z);
//}
//
//Vector3 operator-(const Vector3 &u, const Vector3 &v) {
//  return Vector3(u.x - v.x, u.y - v.y, u.z - v.z);
//}
//
//Vector3 operator*(const Vector3 &v, const float a) {
//  return Vector3(a * v.x, a * v.y, a * v.z);
//}
//
//Vector3 operator*(const float a, const Vector3 &v) {
//  return Vector3(a * v.x, a * v.y, a * v.z);
//}
//
//Vector3 operator*(const Vector3 &u, const Vector3 &v) {
//  return Vector3(u.x * v.x, u.y * v.y, u.z * v.z);
//}
//
//Vector3 operator/(const Vector3 &v, const float a) {
//  return v * (1 / a);
//}
//
//void operator+=(Vector3 &u, const Vector3 &v) {
//  u.x += v.x;
//  u.y += v.y;
//  u.z += v.z;
//}
//
//void operator-=(Vector3 &u, const Vector3 &v) {
//  u.x -= v.x;
//  u.y -= v.y;
//  u.z -= v.z;
//}
//
//void operator*=(Vector3 &v, const float a) {
//  v.x *= a;
//  v.y *= a;
//  v.z *= a;
//}
//
//void operator/=(Vector3 &v, const float a) {
//  const float r = 1 / a;
//
//  v.x *= r;
//  v.y *= r;
//  v.z *= r;
//}
//
//
//Vector3 Vector3::reflect(const Vector3 &direction) {
//  Vector3 n_norm = *this;
//  Vector3 n_dir = direction;
//
//
//  n_dir.Normalize();
//  n_norm.Normalize();
////  return Reflect(n_dir, n_norm);
////  return (2 * (n_norm.DotProduct(-n_dir)) * n_norm - (-n_dir));
//  return 2 * (n_norm.DotProduct(n_dir)) * n_norm - n_dir;
//}
//
//Vector3 Vector3::Reflect(const Vector3 &I, const Vector3 &N) {
////  return I - N * N.DotProduct(I) * 2;
//  return I - N * I.DotProduct(N) * 2;
//}
//
///* *****************************************************************************
// * VECTOR 4
// * ****************************************************************************/
//
//
//void Vector4::Print() {
//  printf("(%0.3f, %0.3f, %0.3f, %0.3f)\n", x, y, z, w);
//  //printf( "_point %0.3f,%0.3f,%0.3f\n", x, y, z );
//}
//
//// --- operátory ------
//
//Vector4 operator-(const Vector4 &v) {
//  return Vector4(-v.x, -v.y, -v.z, -v.z);
//}
//
//Vector4 operator+(const Vector4 &u, const Vector4 &v) {
//  return Vector4(u.x + v.x, u.y + v.y, u.z + v.z, u.w + v.w);
//}
//
//Vector4 operator-(const Vector4 &u, const Vector4 &v) {
//  return Vector4(u.x - v.x, u.y - v.y, u.z - v.z, u.w - v.w);
//}
//
//Vector4 operator*(const Vector4 &v, const float a) {
//  return Vector4(a * v.x, a * v.y, a * v.z, a * v.w);
//}
//
//Vector4 operator*(const float a, const Vector4 &v) {
//  return Vector4(a * v.x, a * v.y, a * v.z, a * v.w);
//}
//
//Vector4 operator*(const Vector4 &u, const Vector4 &v) {
//  return Vector4(u.x * v.x, u.y * v.y, u.z * v.z, u.w * v.w);
//}
//
//Vector4 operator/(const Vector4 &v, const float a) {
//  return v * (1 / a);
//}
//
//void operator+=(Vector4 &u, const Vector4 &v) {
//  u.x += v.x;
//  u.y += v.y;
//  u.z += v.z;
//  u.w += v.w;
//}
//
//void operator-=(Vector4 &u, const Vector4 &v) {
//  u.x -= v.x;
//  u.y -= v.y;
//  u.z -= v.z;
//  u.w -= v.w;
//}
//
//void operator*=(Vector4 &v, const float a) {
//  v.x *= a;
//  v.y *= a;
//  v.z *= a;
//  v.w *= a;
//}
//
//void operator/=(Vector4 &v, const float a) {
//  const float r = 1 / a;
//
//  v.x *= r;
//  v.y *= r;
//  v.z *= r;
//  v.w *= r;
//}