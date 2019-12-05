#include <stdafx.h>
#include <geometry/texture.h>
#include <utils/utils.h>
#include <algorithm>

template<class T>
struct Pixel {
  union {
    struct {
      T r;
      T g;
      T b;
      union {
        T i;
        T a;
      };
    };
    T *data;
  };
  
};


union {
  float f;
  BYTE b[4];
} floatByteUnion;

Texture::Texture(const char *file_name) {
  // image format
  FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
  // pointer to the image, once loaded
  FIBITMAP *dib = nullptr;
  // pointer to the image data
  BYTE *bits = nullptr;
  
  // check the file signature and deduce its format
  fif = FreeImage_GetFileType(file_name, 0);
  // if still unknown, try to guess the file format from the file extension
  if (fif == FIF_UNKNOWN) {
    fif = FreeImage_GetFIFFromFilename(file_name);
  }
  // if known
  if (fif != FIF_UNKNOWN) {
    // check that the plugin has reading capabilities and load the file
    if (FreeImage_FIFSupportsReading(fif)) {
      dib = FreeImage_Load(fif, file_name);
    }
    // if the image loaded
    if (dib) {
      // retrieve the image data
      bits = FreeImage_GetBits(dib);
      //FreeImage_ConvertToRawBits()
      // get the image width and height
      width_ = (FreeImage_GetWidth(dib));
      height_ = (FreeImage_GetHeight(dib));
      
      // if each of these is ok
      if ((bits != 0) && (width_ != 0) && (height_ != 0)) {
        // texture loaded
        scan_width_ = FreeImage_GetPitch(dib); // in bytes
        pixel_size_ = FreeImage_GetBPP(dib) / 8; // in bytes
        
        data_ = new BYTE[scan_width_ * height_]; // BGR(A) format
        FreeImage_ConvertToRawBits(
            data_,
            dib,
            scan_width_,
            pixel_size_ * 8U,
            FI_RGBA_RED_MASK,
            FI_RGBA_GREEN_MASK,
            FI_RGBA_BLUE_MASK,
            TRUE);
      }
      
      FreeImage_Unload(dib);
      bits = nullptr;
      
      for (int x = 0; x < width_; x++) {
        for (int y = 0; y < height_; y++) {
          const unsigned int offset = y * scan_width_ + x * pixel_size_;
          float b = c_linear(static_cast<float>(data_[offset + 0]) / 255.0f);
          float g = c_linear(static_cast<float>(data_[offset + 1]) / 255.0f);
          float r = c_linear(static_cast<float>(data_[offset + 2]) / 255.0f);
          
          data_[offset + 0] = b * 255.f;
          data_[offset + 1] = g * 255.f;
          data_[offset + 2] = r * 255.f;
        }
      }
    }
  }
}

Texture::~Texture() {
  if (data_) {
    // free FreeImage's copy of the data
    delete[] data_;
    data_ = nullptr;
    
    width_ = 0;
    height_ = 0;
  }
}

Color3f Texture::get_texel(const float u, const float v) const {
//    assert( ( u >= 0.0f && u <= 1.0f ) && ( v >= 0.0f && v <= 1.0f ) );
  
  const unsigned int x = std::max(0.f, std::min(static_cast<float>(width_) - 1.f, (u * static_cast<float>(width_))));
  const unsigned int y = std::max(0.f, std::min(static_cast<float>(height_) - 1.f, (v * static_cast<float>(height_))));
  
  float b = 0;
  float g = 0;
  float r = 0;
  float a = 1;
  
  int pix_size;
  
  if (pixel_size_ == 3 * sizeof(uint8_t)) pix_size = sizeof(uint8_t);
  if (pixel_size_ == 4 * sizeof(uint8_t)) pix_size = sizeof(uint8_t);
  if (pixel_size_ == 3 * sizeof(float)) pix_size = sizeof(float);
  if (pixel_size_ == 4 * sizeof(float)) pix_size = sizeof(float);
  
  unsigned char *p1 = &data_[x * pixel_size_ + y * scan_width_ + (0 * pix_size)];
  unsigned char *p2 = &data_[x * pixel_size_ + y * scan_width_ + (1 * pix_size)];
  unsigned char *p3 = &data_[x * pixel_size_ + y * scan_width_ + (2 * pix_size)];
  unsigned char *p4 = &data_[x * pixel_size_ + y * scan_width_ + (3 * pix_size)];
  
  
  if (pixel_size_ == 3 * sizeof(uint8_t)) {
    
    b = (p1[0]) / 255.0f;
    g = (p2[0]) / 255.0f;
    r = (p3[0]) / 255.0f;
    
  } else if (pixel_size_ == 4 * sizeof(uint8_t)) {
    
    b = (p1[0]) / 255.0f;
    g = (p2[0]) / 255.0f;
    r = (p3[0]) / 255.0f;
    a = (p4[0]) / 255.0f;
  } else if (pixel_size_ == 3 * sizeof(float)) {
    memcpy(&b, &p1, sizeof(float));
    memcpy(&g, &p2, sizeof(float));
    memcpy(&r, &p3, sizeof(float));
  } else if (pixel_size_ == 4 * sizeof(float)) {
    b = static_cast<float>(p1[0]);
    g = static_cast<float>(p2[0]);
    r = static_cast<float>(p3[0]);
    a = static_cast<float>(p4[0]);
  }
  
  
  return /*c_linear*/(Color3f{r, g, b/*,a*/});

//
//  const unsigned int x = std::max(0.f, std::min(static_cast<float>(width_) - 1.f, (u * static_cast<float>(width_))));
//  const unsigned int y = std::max(0.f, std::min(static_cast<float>(height_) - 1.f, (v * static_cast<float>(height_))));
//
//  const int x0 = static_cast<int>(floor(x));
//  const int y0 = static_cast<int>(floor(y));
//
//  const int x1 = std::min<int>(width_ - 1, x0 + 1);
//  const int y1 = std::min<int>(height_ - 1, y0 + 1);
//
//  unsigned char *p1 = &data_[x0 * pixel_size_ + y0 * scan_width_];
//  unsigned char *p2 = &data_[x1 * pixel_size_ + y0 * scan_width_];
//  unsigned char *p3 = &data_[x1 * pixel_size_ + y1 * scan_width_];
//  unsigned char *p4 = &data_[x0 * pixel_size_ + y1 * scan_width_];
//
//  const float kx = x - x0;
//  const float ky = y - y0;
//
//
//  float b = (p1[0] * (1 - kx) * (1 - ky)) +
//                  (p2[0] *      kx  * (1 - ky)) +
//                  (p3[0] *      kx  *      ky)  +
//                  (p4[0] * (1 - kx) *      ky);
//
//  float g = (p1[1] * (1 - kx) * (1 - ky)) +
//                  (p2[1] *      kx  * (1 - ky)) +
//                  (p3[1] *      kx  *      ky)  +
//                  (p4[1] * (1 - kx) *      ky);
//
//  float r = (p1[2] * (1 - kx) * (1 - ky)) +
//                  (p2[2] *      kx  * (1 - ky)) +
//                  (p3[2] *      kx  *      ky)  +
//                  (p4[2] * (1 - kx) *      ky);
//
//
//
//  b *= static_cast<float>(1.0 / 255.0);
//  g *= static_cast<float>(1.0 / 255.0);
//  r *= static_cast<float>(1.0 / 255.0);

//  Color4f final_color = (Color4f(p1[0], p1[1], p1[2], p1[3]) * (1 - kx) * (1 - ky) +
//                         Color4f(p2[0], p2[1], p2[2], p2[3]) * kx * (1 - ky) +
//                         Color4f(p3[0], p3[1], p3[2], p3[3]) * kx * ky +
//                         Color4f(p4[0], p4[1], p4[2], p4[3]) * (1 - kx) * ky) *
//                        static_cast<float>(1.0 / 255.0);
//  
  return c_linear(Color3f{r, g, b});
}

unsigned int Texture::width() const {
  return width_;
}

unsigned int Texture::height() const {
  return height_;
}
