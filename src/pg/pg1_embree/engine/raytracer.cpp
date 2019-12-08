#include <stdafx.h>
#include <tutorials.h>

//Engine
#include <engine/raytracer.h>
#include <engine/light.h>
#include <engine/sphericalmap.h>

//geometry
#include <geometry/objloader.h>

//Shaders
#include <shaders/phongshader.h>
#include <shaders/normalsshader.h>
#include <shaders/diffuseshader.h>
#include <shaders/recursivephongshader.h>
#include <shaders/glassshader.h>

//Utils
#include <utils/utils.h>
#include <shaders/commonshader.h>
#include <shaders/pathtracingshader.h>


int Raytracer::currentShadingIdx = 0;

Raytracer::Raytracer(const int width,
                     const int height,
                     const float fov_y,
                     const Vector3 view_from,
                     const Vector3 view_at,
                     const Vector3 lightPos,
                     const Vector3 lightColor,
                     const char *config) :
    SimpleGuiDX11(width, height) {
  InitDeviceAndScene(config);
  
  camera_ = Camera(width, height, fov_y, view_from, view_at);
  phongAmbient_ = false;
  phongDiffuse_ = false;
  phongSpecular_ = false;
  ambientValue_ = 1.f;
  specularStrength_ = 1.f;
  
  defaultBgColor_ = Color4f(0.0f, 0.0f, 0.0f, 1.0f);
  
  light_ = new Light(lightPos, lightColor);

//  sphericalMap_ = new SphericalMap("data/venice_sunset.jpg");
  sphericalMap_ = new SphericalMap("data/field.jpg");
//  sphericalMap_ = new SphericalMap("data/outdoor_umbrellas_4k.hdr");
  
  
  activeShader_ = ShaderEnum::Common;
  
  shaders_[static_cast<int>(ShaderEnum::None)] = new Shader(
      &camera_,
      light_,
      &scene_,
      &surfaces_,
      &materials_);
  
  shaders_[static_cast<int>(ShaderEnum::Diffuse)] = new DiffuseShader(
      &camera_,
      light_,
      &scene_,
      &surfaces_,
      &materials_);
  
  shaders_[static_cast<int>(ShaderEnum::Phong)] = new PhongShader(
      &camera_,
      light_,
      &scene_,
      &surfaces_,
      &materials_);
  
  shaders_[static_cast<int>(ShaderEnum::Normals)] = new NormalsShader(
      &camera_,
      light_,
      &scene_,
      &surfaces_,
      &materials_);
  
  shaders_[static_cast<int>(ShaderEnum::RecursivePhong)] = new RecursivePhongShader(
      &camera_,
      light_,
      &scene_,
      &surfaces_,
      &materials_);
  
  shaders_[static_cast<int>(ShaderEnum::Glass)] = new GlassShader(
      &camera_,
      light_,
      &scene_,
      &surfaces_,
      &materials_);
  
  shaders_[static_cast<int>(ShaderEnum::Common)] = new CommonShader(
      &camera_,
      light_,
      &scene_,
      &surfaces_,
      &materials_);
  shaders_[static_cast<int>(ShaderEnum::PathTracing)] = new PathTracingShader(
      &camera_,
      light_,
      &scene_,
      &surfaces_,
      &materials_);
  
  for (auto shader : shaders_) {
    shader->setDefaultBgColor(&defaultBgColor_);
    shader->setSphericalMap(sphericalMap_);
  }
  
  
}

Raytracer::~Raytracer() {
  LOG("Destructor called");
  for (auto shader : shaders_) {
    SAFE_DELETE(shader);
  }
  SAFE_DELETE(light_);
  SAFE_DELETE(sphericalMap_);
  
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
    
    Vertex3f *vertices = (Vertex3f *) rtcSetNewGeometryBuffer(
        mesh, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3,
        sizeof(Vertex3f), 3 * surface->no_triangles());
    
    Triangle3ui *triangles = (Triangle3ui *) rtcSetNewGeometryBuffer(
        mesh, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3,
        sizeof(Triangle3ui), surface->no_triangles());
    
    rtcSetGeometryUserData(mesh, (void *) (surface->get_material()));
    
    rtcSetGeometryVertexAttributeCount(mesh, 2);
    
    Normal3f *normals = (Normal3f *) rtcSetNewGeometryBuffer(
        mesh, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 0, RTC_FORMAT_FLOAT3,
        sizeof(Normal3f), 3 * surface->no_triangles());
    
    Coord2f *tex_coords = (Coord2f *) rtcSetNewGeometryBuffer(
        mesh, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 1, RTC_FORMAT_FLOAT2,
        sizeof(Coord2f), 3 * surface->no_triangles());
    
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

Color4f Raytracer::get_pixel(const int x, const int y, const float t) {
  if(x == 320/2 && y == 240/3*2){
    (void)0;
  }
  return (shaders_[static_cast<int>(ShaderEnum::Common)]->getPixel(x, y));
//  return (shaders_[static_cast<int>(activeShader_)]->getPixel(x, y));
}

std::array<Color4f, 4> Raytracer::get_pixel4(int x, int y, float t) {
  return shaders_[static_cast<int>(ShaderEnum::Common)]->getPixel4(x, y);
}

std::array<Color4f, 8> Raytracer::get_pixel8(int x, int y, float) {
  return shaders_[static_cast<int>(ShaderEnum::Common)]->getPixel8(x, y);
}

std::array<Color4f, 16> Raytracer::get_pixel16(int x, int y, float) {
  return shaders_[static_cast<int>(ShaderEnum::Common)]->getPixel16(x, y);
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
  static_cast<CommonShader *>(shaders_[static_cast<int>(ShaderEnum::Common)])->useShader = shadingArray[currentShadingIdx].second;
  
  ImGui::Begin("Ray Tracer Params", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
  
  if (ImGui::CollapsingHeader("Engine", 0)) {
    ImGui::Text("Surfaces = %zu", surfaces_.size());
    ImGui::Text("Materials = %zu", materials_.size());
    ImGui::Separator();
    ImGui::Checkbox("Vsync", &vsync_);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                ImGui::GetIO().Framerate);
    ImGui::Text("Raytracer time %.3f ms/frame (%.1f FPS)", SimpleGuiDX11::producerTime * 1000.f,
                1000.f / (SimpleGuiDX11::producerTime * 1000.f));
  }

//  ImGui::SetNextItemOpen(true);
  if (ImGui::CollapsingHeader("Shader", 0)) {
    static const char *currentShaderLabel = shadingArray[currentShadingIdx].first;
    
    struct FuncHolder {
      static bool ItemGetter(void *data, int idx, const char **out_str) {
        *out_str = reinterpret_cast<std::pair<const char *, ShadingType> *>(data)[idx].first;
        return true;
      }
    };
    
    ImGui::Combo("Selected Shader", &currentShadingIdx, &FuncHolder::ItemGetter, (void *) shadingArray,
                 IM_ARRAYSIZE(shadingArray));
    
    switch (shadingArray[currentShadingIdx].second) {
      
      case ShadingType::None: {
        break;
      }
      case ShadingType::Lambert: {
        break;
      }
      case ShadingType::Glass: {
        ImGui::SliderFloat("Ior", &static_cast<CommonShader*>(shaders_[static_cast<int>(ShaderEnum::Common)])->ior, 0.5,
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
    
    ImGui::Checkbox("Sphere map", &Shader::sphereMap_);
    
    ImGui::Checkbox("Supersampling", &Shader::supersampling_);
    
    if (Shader::supersampling_) {
      ImGui::Checkbox("Rand supersampling", &Shader::supersamplingRandom_);
      ImGui::SliderInt("Samples", &Shader::samplingSize_, 1, 1000);
    }
    
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
    ImGui::SliderFloat("X", &camera_.view_from_.x, -100.0f, 100.0f);
    ImGui::SliderFloat("Y", &camera_.view_from_.y, -100.0f, 100.0f);
    ImGui::SliderFloat("Z", &camera_.view_from_.z, -100.0f, 100.0f);
  }
  
  static bool show_demo = false;
  ImGui::Separator();
  if (ImGui::Checkbox("Show demo window", &show_demo))
    ImGui::ShowDemoWindow(nullptr);
  
  ImGui::End();
  return 0;
}
