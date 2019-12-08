//
// Created by zvone on 08-Dec-19.
//

#ifndef PG1_2019_PATHTRACERHELPER_H
#define PG1_2019_PATHTRACERHELPER_H


#include <glm/vec4.hpp>

class PathTracerHelper {
public:
  PathTracerHelper(int height, int width);
  
  glm::vec4 getPixel(int row, int col);
  
  glm::vec4 getInterpolatedPixel(int row, int col);
  
  void incrementTraces();
  
  void resetTraces();
  
  void setPixel(int row, int col, const glm::vec4 &pix);
  
  int getTracesCount();
  
  virtual ~PathTracerHelper();

private:
  
  int width_;
  int height_;
  
  float *pixelsSum_;
  int traces_;
  
};


#endif //PG1_2019_PATHTRACERHELPER_H
