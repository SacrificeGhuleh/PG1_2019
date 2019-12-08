//
// Created by zvone on 08-Dec-19.
//

#include "pathtracerhelper.h"

PathTracerHelper::PathTracerHelper(int height, int width) : width_(width), height_(height), traces_(0) {
  pixelsSum_ = new float[width * height * 4]();
  for (int row = 0; row < height_; row++) {
    for (int col = 0; col < width_; col++) {
      const int offset = (row * width_ + col) * 4;
      
      pixelsSum_[offset + 0] = 0;
      pixelsSum_[offset + 1] = 0;
      pixelsSum_[offset + 2] = 0;
      pixelsSum_[offset + 3] = 0;
    }
  }
}

PathTracerHelper::~PathTracerHelper() {
  delete[] pixelsSum_;
}

glm::vec4 PathTracerHelper::getPixel(int row, int col) {
  glm::vec4 pix;
  const int offset = (row * width_ + col) * 4;
  
  pix.r = pixelsSum_[offset + 0];
  pix.g = pixelsSum_[offset + 1];
  pix.b = pixelsSum_[offset + 2];
  pix.w = pixelsSum_[offset + 3];
  return pix;
}

void PathTracerHelper::setPixel(int row, int col, const glm::vec4 &pix) {
  const int offset = (row * width_ + col) * 4;
  
  pixelsSum_[offset + 0] += pix.r;
  pixelsSum_[offset + 1] += pix.g;
  pixelsSum_[offset + 2] += pix.b;
  pixelsSum_[offset + 3] += pix.w;
}

glm::vec4 PathTracerHelper::getInterpolatedPixel(int row, int col) {
  glm::vec4 pix = getPixel(row, col);
  return pix / glm::vec4(pix.w);
}

void PathTracerHelper::incrementTraces() {
  traces_++;
}

void PathTracerHelper::resetTraces() {
  for (int row = 0; row < height_; row++) {
    for (int col = 0; col < width_; col++) {
      const int offset = (row * width_ + col) * 4;
      
      pixelsSum_[offset + 0] = 0;
      pixelsSum_[offset + 1] = 0;
      pixelsSum_[offset + 2] = 0;
      pixelsSum_[offset + 3] = 0;
    }
  }
}

int PathTracerHelper::getTracesCount() {
  float max = 0;
  for (int row = 0; row < height_; row++) {
    for (int col = 0; col < width_; col++) {
      const int offset = (row * width_ + col) * 4;
      max = pixelsSum_[offset + 3] > max ? pixelsSum_[offset + 3] : max;
    }
  }
  
  return static_cast<int>(max);
}
