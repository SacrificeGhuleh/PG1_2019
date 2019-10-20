#include <stdafx.h>
#include <geometry/texture.h>
#include <utils/utils.h>

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
//  assert( ( u >= 0.0f && u <= 1.0f ) && ( v >= 0.0f && v <= 1.0f ) );
  
  const unsigned int x = std::max(0.f, std::min(static_cast<float>(width_) - 1.f, (u * static_cast<float>(width_))));
  const unsigned int y = std::max(0.f, std::min(static_cast<float>(height_) - 1.f, (v * static_cast<float>(height_))));
  
  const unsigned int offset = y * scan_width_ + x * pixel_size_;
  const float b = static_cast<float>(data_[offset + 0]) / 255.0f;
  const float g = static_cast<float>(data_[offset + 1]) / 255.0f;
  const float r = static_cast<float>(data_[offset + 2]) / 255.0f;
  
  return c_linear(Color3f{r, g, b});
  return Color3f{r, g, b};
}

unsigned int Texture::width() const {
  return width_;
}

unsigned int Texture::height() const {
  return height_;
}
