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
  
  defaultBgColor_ = Color4f(1.0f, 0.0f, 1.0f, 1.0f);
  
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
  return (shaders_[static_cast<int>(activeShader_)]->getPixel(x, y));
}

std::array<Color4f, 4> Raytracer::get_pixel4(int x, int y, float t) {
  return shaders_[static_cast<int>(activeShader_)]->getPixel4(x, y);
}

std::array<Color4f, 8> Raytracer::get_pixel8(int x, int y, float) {
  return shaders_[static_cast<int>(activeShader_)]->getPixel8(x, y);
}

std::array<Color4f, 16> Raytracer::get_pixel16(int x, int y, float) {
  return shaders_[static_cast<int>(activeShader_)]->getPixel16(x, y);
}

int Raytracer::Ui() {
  static const char *shaderNames[static_cast<int>(ShaderEnum::ShadersCount)] = {
      "None",
      "Diffuse",
      "Phong",
      "Normals",
      "Recursive Phong",
      "Glass",
      "Common",
      "Path tracer"};
  
  static const char *multiRayNames[4] = {
      "1x1",
      "2x2",
      "2x4",
      "4x4"};
  
  
  ImGui::Begin("Ray Tracer Params", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
  ImGui::SetNextItemOpen(true);
  if (ImGui::CollapsingHeader("Engine", 0)) {
    ImGui::Text("Surfaces = %zu", surfaces_.size());
    ImGui::Text("Materials = %zu", materials_.size());
    ImGui::Separator();
    ImGui::Checkbox("Vsync", &vsync_);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                ImGui::GetIO().Framerate);
    ImGui::Text("Raytracer time %.3f ms/frame (%.1f FPS)", SimpleGuiDX11::producerTime * 1000.f,
                1000.f / (SimpleGuiDX11::producerTime * 1000.f));
//    ImGui::Separator();
  }
  
  ImGui::SetNextItemOpen(true);
  if (ImGui::CollapsingHeader("Shader", 0)) {
    
    ImGui::Combo("Selected Shader", reinterpret_cast<int *>(&activeShader_), shaderNames,
                 static_cast<int>(ShaderEnum::ShadersCount));

//    ImGui::Combo("Selected Multiray", reinterpret_cast<int *>(&multiRay_), multiRayNames,
//                 4);

//    ImGui::Checkbox("Flip texture U", &Shader::flipTextureU_);
//    ImGui::Checkbox("Flip texture V", &Shader::flipTextureV_);
    ImGui::Checkbox("Sphere map", &Shader::sphereMap_);
    
    ImGui::Checkbox("Supersampling", &Shader::supersampling_);
    
    if (Shader::supersampling_) {
      ImGui::Checkbox("Rand supersampling", &Shader::supersamplingRandom_);
      ImGui::SliderInt("Samples", &Shader::samplingSize_, 1, 10);
    }
    
    ImGui::SliderFloat("Ray near", &Shader::tNear_, 0, 0.5f);
    
    
    ImGui::Checkbox("Correct normals", &Shader::correctNormals_);
    switch (activeShader_) {
      
      case ShaderEnum::None: {
        break;
      }
      case ShaderEnum::Diffuse: {
        break;
      }
      case ShaderEnum::Phong: {
        ImGui::Checkbox("Ambient", &PhongShader::phongAmbient_);
        ImGui::Checkbox("Diffuse", &PhongShader::phongDiffuse_);
        ImGui::Checkbox("Specular", &PhongShader::phongSpecular_);
        
        ImGui::Separator();
        
        ImGui::SliderFloat("Ambient Slider", &PhongShader::ambientValue_, 0.0f, 30.0f);
        ImGui::SliderFloat("Specular strength Slider", &PhongShader::specularStrength_, 0.0f, 30.0f);
        
        break;
      }
      case ShaderEnum::Normals: {
        break;
      }
      case ShaderEnum::RecursivePhong: {
        ImGui::Separator();
        ImGui::SliderInt("Recursion", &Shader::recursionDepth_, 1, 10);
        ImGui::SliderFloat("Reflectivity", &RecursivePhongShader::reflectivityCoef, 0, 1);
        break;
      }
      case ShaderEnum::Glass: {
        ImGui::Separator();
        ImGui::SliderInt("Recursion", &Shader::recursionDepth_, 0, 10);
        ImGui::SliderFloat("Ior", &GlassShader::ior_, 0.01f, 2.f);
        ImGui::Separator();
        
        ImGui::Checkbox("Add reflect", &GlassShader::addReflect_);
        if (GlassShader::addReflect_) {
          ImGui::Checkbox("Add diffuse to reflect", &GlassShader::addDiffuseToReflect_);
        }
        
        ImGui::Checkbox("Add refract", &GlassShader::addRefract_);
        if (GlassShader::addRefract_) {
          ImGui::Checkbox("Add diffuse to refract", &GlassShader::addDiffuseToRefract_);
        }
        
        ImGui::Checkbox("Add attenuation", &GlassShader::addAttenuation);
        if (GlassShader::addAttenuation) {
          ImGui::SliderFloat("Material attenuation", &GlassShader::attenuation_, 0.00f, 10.f);
        }
        break;
      }
      case ShaderEnum::Common: {
        ImGui::Separator();
        ImGui::SliderInt("Recursion", &Shader::recursionDepth_, 0, 10);
        ImGui::SliderFloat("Ior", &GlassShader::ior_, 0.01f, 2.f);
        ImGui::Separator();
        break;
      }
      case ShaderEnum::PathTracing: {
        ImGui::Separator();
        ImGui::SliderInt("Recursion", &Shader::recursionDepth_, 0, 10);
        ImGui::SliderFloat("Ior", &GlassShader::ior_, 0.01f, 2.f);
        ImGui::Separator();
        ImGui::Checkbox("Use parent GetPixel", &PathTracingShader::useParentGetPixel_);
        if (!PathTracingShader::useParentGetPixel_) {
          ImGui::SliderInt("Samples per pixel", &PathTracingShader::samples_, 1, 2000);
        }
        break;
      }
      case ShaderEnum::ShadersCount: {
        break;
      }
    }
    if (!Shader::sphereMap_) {
      ImGui::Separator();
      ImGui::ColorPicker4("Background color", &defaultBgColor_.x);
      ImGui::Separator();
    }
  }
  
  ImGui::SetNextItemOpen(true);
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
