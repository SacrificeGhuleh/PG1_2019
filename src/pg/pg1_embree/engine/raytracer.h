#pragma once

#include <array>
#include <engine/simpleguidx11.h>
#include <geometry/surface.h>
#include <engine/camera.h>

class SphericalMap;

class CommonShader;

class Light;

/*! \class Raytracer
\brief General ray tracer class.

\author Tomáš Fabián
\version 0.1
\date 2018
*/
class Raytracer : public SimpleGuiDX11 {
public:
  Raytracer(int width,
            int height,
            float fov_y,
            Vector3 view_from,
            Vector3 view_at,
            Vector3 lightPos,
            Vector3 lightColor,
            const char *config = "threads=0,verbose=3");
  
  virtual ~Raytracer();
  
  int InitDeviceAndScene(const char *config);
  
  int ReleaseDeviceAndScene();
  
  void LoadScene(std::string file_name);
  
  Color4f get_pixel(int x, int y, float t = 0.0f) override;
  
  virtual std::array<Color4f, 4> get_pixel4(int x, int y, float t = 0.0f);
  
  virtual std::array<Color4f, 8> get_pixel8(int x, int y, float t = 0.0f);
  
  virtual std::array<Color4f, 16> get_pixel16(int x, int y, float t = 0.0f);
  
  int Ui() override;
  
  virtual void saveImage(const std::string& filename);


private:
  std::vector<Surface *> surfaces_;
  std::vector<Material *> materials_;
  
  RTCDevice device_;
  RTCScene scene_;
  Camera camera_;
  
  Light *light_;
  
  CommonShader *shader_;
  
  float ambientValue_;
  float specularStrength_;
  
  bool phongAmbient_;
  bool phongDiffuse_;
  bool phongSpecular_;
  Color4f defaultBgColor_;
  SphericalMap *sphericalMap_;
  int currentShadingIdx_;
};
