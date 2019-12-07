//
// Created by zvone on 26-Sep-19.
//

#ifndef PG1_2019_LIGHT_H
#define PG1_2019_LIGHT_H


#include <math/vector.h>

class Light {
public:
  Light(const Vector3 &position, const Vector3 &color) : position_{position}, color_{color} {}
  
  virtual ~Light();

private:
  Vector3 position_;
public:
  const Vector3 &getPosition() const {
    return position_;
  }
  
  const Vector3 &getColor() const {
    return color_;
  }

private:
  Vector3 color_;
};


#endif //PG1_2019_LIGHT_H
