#ifndef MY_MATH_H_
#define MY_MATH_H_

#define _USE_MATH_DEFINES

#include <cmath>
#include <cfloat>
#include <cassert>
#include <math/vector.h>

template<class T>
inline T sqr(const T x) {
  return x * x;
}

inline Normal3f normalize(const Normal3f &n) {
  float tmp = sqr(n.x) + sqr(n.y) + sqr(n.z);
  
  if (fabsf(tmp) > FLT_EPSILON) {
    tmp = 1.0f / tmp;
    return Normal3f{n.x * tmp, n.y * tmp, n.z * tmp};
  }
  
  return n;
}

inline float deg2rad(const float x) {
  return x * float(M_PI) / 180.0f;
}

inline float clamp(const float v, const float lo, const float hi) {
//  assert(!(hi < lo));
  return (v < lo) ? lo : (hi < v) ? hi : v;
}

#endif
