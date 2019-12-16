//
// Created by zvone on 24-Oct-19.
//

#ifndef PG1_2019_COMMONSHADER_H
#define PG1_2019_COMMONSHADER_H


#include <shaders/shader.h>

class CommonShader : public Shader {
public:
  CommonShader(Camera *camera, Light *light, RTCScene *rtcScene, std::vector<Surface *> *surfaces,
               std::vector<Material *> *materials);
  
  glm::vec4 traceRay(const RtcRayHitIor &rayHit, int depth) override;
  
  
  
  static float ior;
  static ShadingType useShader;
  static bool softShadows;
  static int lightShadowsSamples;
private:
  
  template<ShadingType T>
  glm::vec4 traceMaterial(const RtcRayHitIor &rayHit,
                          const Material *material,
                          const glm::vec2 &tex_coord,
                          const glm::vec3 &origin,
                          const glm::vec3 &direction,
                          const glm::vec3 &worldPos,
                          const glm::vec3 &directionToCamera,
                          const glm::vec3 &lightPos,
                          const glm::vec3 &lightDir,
                          const glm::vec3 &shaderNormal,
                          const float dotNormalCamera,
                          const int depth);
};

template<>
glm::vec4 CommonShader::traceMaterial<ShadingType::None>(const RtcRayHitIor &rayHit,
                                                         const Material *material,
                                                         const glm::vec2 &tex_coord,
                                                         const glm::vec3 &origin,
                                                         const glm::vec3 &direction,
                                                         const glm::vec3 &worldPos,
                                                         const glm::vec3 &directionToCamera,
                                                         const glm::vec3 &lightPos,
                                                         const glm::vec3 &lightDir,
                                                         const glm::vec3 &shaderNormal,
                                                         const float dotNormalCamera,
                                                         const int depth);

template<>
glm::vec4 CommonShader::traceMaterial<ShadingType::Glass>(const RtcRayHitIor &rayHit,
                                                          const Material *material,
                                                          const glm::vec2 &tex_coord,
                                                          const glm::vec3 &origin,
                                                          const glm::vec3 &direction,
                                                          const glm::vec3 &worldPos,
                                                          const glm::vec3 &directionToCamera,
                                                          const glm::vec3 &lightPos,
                                                          const glm::vec3 &lightDir,
                                                          const glm::vec3 &shaderNormal,
                                                          const float dotNormalCamera,
                                                          const int depth);

template<>
glm::vec4 CommonShader::traceMaterial<ShadingType::Lambert>(const RtcRayHitIor &rayHit,
                                                            const Material *material,
                                                            const glm::vec2 &tex_coord,
                                                            const glm::vec3 &origin,
                                                            const glm::vec3 &direction,
                                                            const glm::vec3 &worldPos,
                                                            const glm::vec3 &directionToCamera,
                                                            const glm::vec3 &lightPos,
                                                            const glm::vec3 &lightDir,
                                                            const glm::vec3 &shaderNormal,
                                                            const float dotNormalCamera,
                                                            const int depth);

template<>
glm::vec4 CommonShader::traceMaterial<ShadingType::Mirror>(const RtcRayHitIor &rayHit,
                                                           const Material *material,
                                                           const glm::vec2 &tex_coord,
                                                           const glm::vec3 &origin,
                                                           const glm::vec3 &direction,
                                                           const glm::vec3 &worldPos,
                                                           const glm::vec3 &directionToCamera,
                                                           const glm::vec3 &lightPos,
                                                           const glm::vec3 &lightDir,
                                                           const glm::vec3 &shaderNormal,
                                                           const float dotNormalCamera,
                                                           const int depth);

template<>
glm::vec4 CommonShader::traceMaterial<ShadingType::Phong>(const RtcRayHitIor &rayHit,
                                                          const Material *material,
                                                          const glm::vec2 &tex_coord,
                                                          const glm::vec3 &origin,
                                                          const glm::vec3 &direction,
                                                          const glm::vec3 &worldPos,
                                                          const glm::vec3 &directionToCamera,
                                                          const glm::vec3 &lightPos,
                                                          const glm::vec3 &lightDir,
                                                          const glm::vec3 &shaderNormal,
                                                          const float dotNormalCamera,
                                                          const int depth);

template<>
glm::vec4 CommonShader::traceMaterial<ShadingType::PathTracing>(const RtcRayHitIor &rayHit,
                                                                const Material *material,
                                                                const glm::vec2 &tex_coord,
                                                                const glm::vec3 &origin,
                                                                const glm::vec3 &direction,
                                                                const glm::vec3 &worldPos,
                                                                const glm::vec3 &directionToCamera,
                                                                const glm::vec3 &lightPos,
                                                                const glm::vec3 &lightDir,
                                                                const glm::vec3 &shaderNormal,
                                                                const float dotNormalCamera,
                                                                const int depth);

template<>
glm::vec4 CommonShader::traceMaterial<ShadingType::Normals>(const RtcRayHitIor &rayHit,
                                                            const Material *material,
                                                            const glm::vec2 &tex_coord,
                                                            const glm::vec3 &origin,
                                                            const glm::vec3 &direction,
                                                            const glm::vec3 &worldPos,
                                                            const glm::vec3 &directionToCamera,
                                                            const glm::vec3 &lightPos,
                                                            const glm::vec3 &lightDir,
                                                            const glm::vec3 &shaderNormal,
                                                            const float dotNormalCamera,
                                                            const int depth);

template<>
glm::vec4 CommonShader::traceMaterial<ShadingType::TexCoords>(const RtcRayHitIor &rayHit,
                                                              const Material *material,
                                                              const glm::vec2 &tex_coord,
                                                              const glm::vec3 &origin,
                                                              const glm::vec3 &direction,
                                                              const glm::vec3 &worldPos,
                                                              const glm::vec3 &directionToCamera,
                                                              const glm::vec3 &lightPos,
                                                              const glm::vec3 &lightDir,
                                                              const glm::vec3 &shaderNormal,
                                                              const float dotNormalCamera,
                                                              const int depth);

#endif //PG1_2019_COMMONSHADER_H
