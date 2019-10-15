#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <FreeImage.h>
#include <math/vector.h>
#include <utils/structs.h>

/*! \class Texture
\brief Single texture.

\author Tomáš Fabián
\version 0.95
\date 2012-2018
*/
class Texture {
public:
  Texture(const char *file_name);
  
  ~Texture();
  
  Color3f get_texel(const float u, const float v) const;
  
  unsigned int width() const;
  
  unsigned int height() const;

private:
  unsigned int width_{0}; // image width (px)
  unsigned int height_{0}; // image height (px)
  unsigned int scan_width_{0}; // size of image row (bytes)
  unsigned int pixel_size_{0}; // size of each pixel (bytes)
  BYTE *data_{nullptr}; // image data in BGR format
};

#endif
