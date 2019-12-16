#include <stdafx.h>
#include <tutorials.h>

//Engine
#include <engine/raytracer.h>
#include <engine/light.h>
#include <engine/sphericalmap.h>

//Geometry
#include <geometry/objloader.h>

//Shaders
#include <shaders/commonshader.h>
#include <shaders/pathtracerhelper.h>

//Utils
#include <utils/utils.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <utils/stb_image_write.h>
#include <csignal>


Raytracer::Raytracer(const int width,
                     const int height,
                     const float fov_y,
                     const glm::vec3 view_from,
                     const glm::vec3 view_at,
                     const glm::vec3 lightPos,
                     const glm::vec3 lightColor,
                     const char *config) :
    SimpleGuiDX11(width, height) {
  InitDeviceAndScene(config);
  
  camera_ = Camera(width, height, fov_y, view_from, view_at);
  phongAmbient_ = false;
  phongDiffuse_ = false;
  phongSpecular_ = false;
  ambientValue_ = 1.f;
  specularStrength_ = 1.f;
  currentShadingIdx_ = 0;
  
  defaultBgColor_ = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
  
  light_ = new Light(lightPos, lightColor);

//  sphericalMap_ = new SphericalMap("data/venice_sunset.jpg");
  sphericalMap_ = new SphericalMap("data/field.jpg");
//  sphericalMap_ = new SphericalMap("data/outdoor_umbrellas_4k.hdr");
  
  shader_ = new CommonShader(
      &camera_,
      light_,
      &scene_,
      &surfaces_,
      &materials_);
  shader_->setDefaultBgColor(&defaultBgColor_);
  shader_->setSphericalMap(sphericalMap_);
  shader_->pathTracerHelper = new PathTracerHelper(height, width);
}

Raytracer::~Raytracer() {
  LOG("Destructor called");
  SAFE_DELETE(shader_);
  SAFE_DELETE(light_);
  SAFE_DELETE(sphericalMap_);
  for (Surface *surface : surfaces_) {
    SAFE_DELETE(surface);
  }
  
  ReleaseDeviceAndScene();
}

int Raytracer::InitDeviceAndScene(const char *config) {
  device_ = rtcNewDevice(config);
  error_handler(nullptr, rtcGetDeviceError(device_), "Unable to create a new device.\n");
  rtcSetDeviceErrorFunction(device_, error_handler, nullptr);
  
  ssize_t triangle_supported = rtcGetDeviceProperty(device_, RTC_DEVICE_PROPERTY_TRIANGLE_GEOMETRY_SUPPORTED);
  
  // create a new scene bound to the specified device
  scene_ = rtcNewScene(device_);
  
  //robust ray tracing
  rtcSetSceneFlags(scene_, RTC_SCENE_FLAG_ROBUST);
  
  rtcSetSceneBuildQuality(scene_, RTC_BUILD_QUALITY_HIGH);
  return S_OK;
}

int Raytracer::ReleaseDeviceAndScene() {
  rtcReleaseScene(scene_);
  rtcReleaseDevice(device_);
  
  return S_OK;
}

void Raytracer::LoadScene(const std::string file_name) {
  const int no_surfaces = LoadOBJ(file_name.c_str(), surfaces_, materials_);
  
  // surfaces loop
  for (auto surface : surfaces_) {
    RTCGeometry mesh = rtcNewGeometry(device_, RTC_GEOMETRY_TYPE_TRIANGLE);
    
    glm::vec3 *vertices = (glm::vec3 *) rtcSetNewGeometryBuffer(
        mesh, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3,
        sizeof(glm::vec3), 3 * surface->no_triangles());
    
    Triangle3ui *triangles = (Triangle3ui *) rtcSetNewGeometryBuffer(
        mesh, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3,
        sizeof(Triangle3ui), surface->no_triangles());
    
    rtcSetGeometryUserData(mesh, (void *) (surface->get_material()));
    
    rtcSetGeometryVertexAttributeCount(mesh, 2);
    
    glm::vec3 *normals = (glm::vec3 *) rtcSetNewGeometryBuffer(
        mesh, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 0, RTC_FORMAT_FLOAT3,
        sizeof(glm::vec3), 3 * surface->no_triangles());
    
    glm::vec2 *tex_coords = (glm::vec2 *) rtcSetNewGeometryBuffer(
        mesh, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 1, RTC_FORMAT_FLOAT2,
        sizeof(glm::vec2), 3 * surface->no_triangles());
    
    // triangles loop
    for (int i = 0, k = 0; i < surface->no_triangles(); ++i) {
      Triangle &triangle = surface->get_triangle(i);
      
      // vertices loop
      for (int j = 0; j < 3; ++j, ++k) {
        const Vertex &vertex = triangle.vertex(j);
        
        vertices[k].x = vertex.position.x;
        vertices[k].y = vertex.position.y;
        vertices[k].z = vertex.position.z;
        
        normals[k].x = vertex.normal.x;
        normals[k].y = vertex.normal.y;
        normals[k].z = vertex.normal.z;
        
        tex_coords[k].x = vertex.texture_coords[0].x;
        tex_coords[k].y = vertex.texture_coords[0].y;
      } // end of vertices loop
      
      triangles[i].v0 = k - 3;
      triangles[i].v1 = k - 2;
      triangles[i].v2 = k - 1;
    } // end of triangles loop
    
    rtcCommitGeometry(mesh);
    unsigned int geom_id = rtcAttachGeometry(scene_, mesh);
    rtcReleaseGeometry(mesh);
  } // end of surfaces loop
  
  rtcCommitScene(scene_);
}

glm::vec4 Raytracer::get_pixel(const int x, const int y, const float t) {
  if(x == width_/2 && y == height_/2){
    (void)0;
  }
  return (shader_->getPixel(x, y));
}


int Raytracer::Ui() {
  
  static const std::pair<const char *, ShadingType> shadingArray[] = {
      std::pair("Common", ShadingType::None),
      std::pair("Lambert", ShadingType::Lambert),
      std::pair("Glass", ShadingType::Glass),
      std::pair("PathTracing", ShadingType::PathTracing),
      std::pair("Mirror", ShadingType::Mirror),
      std::pair("Phong", ShadingType::Phong),
      std::pair("Normals", ShadingType::Normals),
      std::pair("Textuture coords", ShadingType::TexCoords)
  };
  shader_->useShader = shadingArray[currentShadingIdx_].second;
  
  static const char *samplingTypeLabels[static_cast<int>(SuperSamplingType::SuperSamplingCount)] = {
      "None",
      "Uniform",
      "Cumulative random finite",
      "Cumulative random infinite"
  };
  
  ImGui::Begin("Ray Tracer Params", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
  
  if (ImGui::CollapsingHeader("Engine", 0)) {
    ImGui::Text("Surfaces = %zu", surfaces_.size());
    ImGui::Text("Materials = %zu", materials_.size());
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                ImGui::GetIO().Framerate);
    ImGui::Text("Raytracer time %.3f ms/frame (%.1f FPS)", SimpleGuiDX11::producerTime * 1000.f,
                1000.f / (SimpleGuiDX11::producerTime * 1000.f));
    ImGui::Separator();
    ImGui::Checkbox("Vsync", &vsync_);
    ImGui::DragFloat("Gamma correction", &SimpleGuiDX11::gamma_, 0.1f, 0.f);
    
  }

//  ImGui::SetNextItemOpen(true);
  if (ImGui::CollapsingHeader("Shader", 0)) {
    
    struct FuncHolder {
      static bool ItemGetter(void *data, int idx, const char **out_str) {
        *out_str = reinterpret_cast<std::pair<const char *, ShadingType> *>(data)[idx].first;
        return true;
      }
    };
    
    ImGui::Combo("Selected Shader", &currentShadingIdx_, &FuncHolder::ItemGetter, (void *) shadingArray,
                 IM_ARRAYSIZE(shadingArray));
    
    switch (shadingArray[currentShadingIdx_].second) {
      
      case ShadingType::None: {
        break;
      }
      case ShadingType::Lambert: {
        break;
      }
      case ShadingType::Glass: {
        ImGui::SliderFloat("Ior", &CommonShader::ior,
                           0.5,
                           2.5);
        break;
      }
      case ShadingType::PathTracing: {
        break;
      }
      case ShadingType::Mirror: {
        break;
      }
      case ShadingType::Phong: {
        
        
        break;
      }
      case ShadingType::Normals: {
        break;
      }
      case ShadingType::TexCoords: {
        break;
      }
    }
    
    
    ImGui::DragInt("Max recursion", &Shader::recursionDepth_);
    
    ImGui::Checkbox("Sphere map", &Shader::sphereMap_);
    
    ImGui::Separator();
    
    const SuperSamplingType currentSamplingType = Shader::superSamplingType_;
    
    ImGui::Combo("Supersampling", reinterpret_cast<int *>(&Shader::superSamplingType_), samplingTypeLabels,
                 IM_ARRAYSIZE(samplingTypeLabels));
    switch (Shader::superSamplingType_) {
      
      case SuperSamplingType::Uniform: {
        ImGui::SliderInt("SamplesX", &Shader::samplingSizeX_, 1, 50);
        ImGui::SliderInt("SamplesY", &Shader::samplingSizeY_, 1, 50);
        ImGui::Text("Samples total = %zu", Shader::samplingSizeX_ * Shader::samplingSizeY_);
        break;
      }
      case SuperSamplingType::RandomFinite: {
        ImGui::SliderInt("Samples", &Shader::samplingSize_, 1, 2500);
        break;
      }
      case SuperSamplingType::RandomInfinite: {
        static int lastSaved = 0;
        int traces = shader_->pathTracerHelper->getTracesCount();
        ImGui::Text("Samples total = %zu", traces);
        
        if (ImGui::Button("Reset")) {
          shader_->pathTracerHelper->resetTraces();
        }
        
        static bool saveToFile = false;
        
        ImGui::Checkbox("Save to file", &saveToFile);
        
        if (saveToFile) {
          std::string filename = std::string("out/path tracing - samples ");
          filename.append(std::to_string(traces));
          filename.append(".png");
          if (traces > 0) {
            if (traces < 100) {
              if (traces % 10 == 0 && lastSaved < traces) {
                saveImage(filename);
              }
            } else if (traces < 1000) {
              if (traces % 100 == 0 && lastSaved < traces) {
                saveImage(filename);
              }
            } else {
              if (traces % 1000 == 0 && lastSaved < traces) {
                saveImage(filename);
              }
            }
          }
        }
        break;
      }
    }
    
    if (currentSamplingType != Shader::superSamplingType_) {
      shader_->pathTracerHelper->resetTraces();
    }
    ImGui::Separator();
    
    ImGui::SliderFloat("Ray near", &Shader::tNear_, 0, 0.5f);
    
    ImGui::Checkbox("Correct normals", &Shader::correctNormals_);
    
    if (!Shader::sphereMap_) {
      ImGui::Separator();
      ImGui::ColorPicker4("Background color", &defaultBgColor_.x);
      ImGui::Separator();
    }
  }
  
  if (ImGui::CollapsingHeader("Camera", true)) {
    ImGui::Text("Camera position");
    
    ImGui::DragFloat("X##1", &camera_.view_from_.x);
    ImGui::DragFloat("Y##1", &camera_.view_from_.y);
    ImGui::DragFloat("Z##1", &camera_.view_from_.z);
    
    ImGui::Text("View at");
    ImGui::DragFloat("X##2", &camera_.view_at_.x);
    ImGui::DragFloat("Y##2", &camera_.view_at_.y);
    ImGui::DragFloat("Z##2", &camera_.view_at_.z);
  }
  
  if (ImGui::CollapsingHeader("Light", true)) {
    ImGui::Text("Light position");
    
    ImGui::DragFloat("X##3", &light_->position_.x);
    ImGui::DragFloat("Y##3", &light_->position_.y);
    ImGui::DragFloat("Z##3", &light_->position_.z);
    
    ImGui::ColorPicker3("Light color", &light_->color_.x);
  }
  
  static bool show_demo = false;
  ImGui::Separator();
  ImGui::Checkbox("Show demo window", &show_demo);
  if (show_demo)
    ImGui::ShowDemoWindow(nullptr);
  
  static char buf[128] = "out/render.png";
  ImGui::InputText("Filename", buf, IM_ARRAYSIZE(buf));
  
  bool save = ImGui::Button("Save current render");
  
  if (save) {
    saveImage(buf);
  }
  
  ImGui::End();
  return 0;
}

void Raytracer::saveImage(const std::string &filename) {
  uint8_t *writeArray = new uint8_t[width_ * height_ * 4]();
  
  {
    std::lock_guard<std::mutex> lock(tex_data_lock_);
    
    for (int y = 0; y < height_; ++y) {
      for (int x = 0; x < width_; ++x) {
        const int offset = (y * width_ + x) * 4;
        writeArray[offset + 0] = tex_data_[offset + 0] * 255;
        writeArray[offset + 1] = tex_data_[offset + 1] * 255;
        writeArray[offset + 2] = tex_data_[offset + 2] * 255;
        writeArray[offset + 3] = tex_data_[offset + 3] * 255;
      }
    }
    
  } // lock release
  
  stbi_write_png(filename.c_str(), width_, height_, 4, writeArray, width_ * 4);
  delete[] writeArray;
}
