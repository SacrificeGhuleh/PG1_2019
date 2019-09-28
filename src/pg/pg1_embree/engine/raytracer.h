#pragma once

#include <array>
#include <engine/simpleguidx11.h>
#include <geometry/surface.h>
#include <engine/camera.h>
#include <shaders/shaderenum.h>

class Shader;

class Light;

/*! \class Raytracer
\brief General ray tracer class.

\author Tomáš Fabián
\version 0.1
\date 2018
*/
class Raytracer : public SimpleGuiDX11 {
public:
  Raytracer(const int width, const int height,
            const float fov_y, const Vector3 view_from, const Vector3 view_at,
            const char *config = "threads=0,verbose=3");
  
  ~Raytracer();
  
  int InitDeviceAndScene(const char *config);
  
  int ReleaseDeviceAndScene();
  
  void LoadScene(const std::string file_name);
  
  Color4f get_pixel(const int x, const int y, const float t = 0.0f) override;
  
  int Ui();


private:
  std::vector<Surface *> surfaces_;
  std::vector<Material *> materials_;
  
  RTCDevice device_;
  RTCScene scene_;
  Camera camera_;
  
  
  ShaderEnum activeShader_;
  
  Light *light_;
  
  std::array<Shader *, static_cast<int>(ShaderEnum::ShadersCount)> shaders_;
  
  float ambientValue_;
  float specularStrength_;
  
  bool phongAmbient_;
  bool phongDiffuse_;
  bool phongSpecular_;
  Color4f defaultBgColor_;
};
