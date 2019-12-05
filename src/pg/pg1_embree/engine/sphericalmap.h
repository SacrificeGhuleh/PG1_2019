//
// Created by zvone on 03-Oct-19.
//

#ifndef PG1_2019_SPHERICALMAP_H
#define PG1_2019_SPHERICALMAP_H

#include <string>
#include <math/vector.h>
//#include <geometry/texture.h>
#include <memory>

class Texture;

class SphericalMap {
public:
  explicit SphericalMap(const std::string& filename);
  
  Color3f texel(const Vector3 &pos);

private:
  std::unique_ptr<Texture> texture_;
  //Texture *texture_;
};


#endif //PG1_2019_SPHERICALMAP_H
