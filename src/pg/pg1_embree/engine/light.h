//
// Created by zvone on 26-Sep-19.
//

#ifndef PG1_2019_LIGHT_H
#define PG1_2019_LIGHT_H


#include <glm/vec3.hpp>

class Light {
public:
  Light(const glm::vec3 &position, const glm::vec3 &color) : position_{position}, color_{color} {}
  
  virtual ~Light();

//private:
  const glm::vec3 &getPosition() const {
    return position_;
  }
  
  const glm::vec3 &getColor() const {
    return color_;
  }

//private:
  glm::vec3 color_;
  glm::vec3 position_;
};


#endif //PG1_2019_LIGHT_H
