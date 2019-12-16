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
            glm::vec3 view_from,
            glm::vec3 view_at,
            glm::vec3 lightPos,
            glm::vec3 lightColor,
            const char *config = "threads=0,verbose=3");
  
  virtual ~Raytracer();
  
  int InitDeviceAndScene(const char *config);
  
  int ReleaseDeviceAndScene();
  
  void LoadScene(std::string file_name);
  
  glm::vec4 get_pixel(int x, int y, float t = 0.0f) override;
  
  int Ui() override;
  
  virtual void saveImage(const std::string &filename);


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
  glm::vec4 defaultBgColor_;
  SphericalMap *sphericalMap_;
  int currentShadingIdx_;
};
