//
// Created by zvone on 24-Oct-19.
//

#ifndef PG1_2019_SHADINGTYPE_H
#define PG1_2019_SHADINGTYPE_H


enum class ShadingType {
  None = -1,
  Lambert = 3,
  Glass = 4,
  PathTracing = 5,
  Mirror = 6,
  Phong = 7
};


inline bool isShadingType(int type) {
  if (type < 0) return false;
  
  if (type == static_cast<int>(ShadingType::Lambert))
    return true;
  
  if (type == static_cast<int>(ShadingType::Glass))
    return true;
  
  if (type == static_cast<int>(ShadingType::PathTracing))
    return true;
  
  if (type == static_cast<int>(ShadingType::Mirror))
    return true;
  
  if (type == static_cast<int>(ShadingType::Phong))
    return true;
  
  return false;
}

#endif //PG1_2019_SHADINGTYPE_H
