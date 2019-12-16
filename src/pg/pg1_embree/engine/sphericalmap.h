//
// Created by zvone on 03-Oct-19.
//

#ifndef PG1_2019_SPHERICALMAP_H
#define PG1_2019_SPHERICALMAP_H

#include <string>
#include <glm/vec3.hpp>

#include <memory>

class Texture;

class SphericalMap {
public:
  explicit SphericalMap(const std::string& filename);
  
  virtual ~SphericalMap();
  
  glm::vec3 texel(const glm::vec3 &pos);

private:
  std::unique_ptr<Texture> texture_;
};


#endif //PG1_2019_SPHERICALMAP_H
