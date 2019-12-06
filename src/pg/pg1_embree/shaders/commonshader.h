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
  
  Color4f traceRay(const RtcRayHitIor &rayHit, int depth = Shader::recursionDepth_) override;
  
  glm::vec3 hemisphereSampling(glm::vec3 normal, float& pdf);
  
private:
  
  template<ShadingType T>
  Color4f traceMaterial(const RtcRayHitIor &rayHit,
                        const Material *material,
                        const Coord2f &tex_coord,
                        const glm::vec3 &normal,
                        const glm::vec3 &worldPos,
                        int depth);
};

template<>
Color4f CommonShader::traceMaterial<ShadingType::None>(const RtcRayHitIor &rayHit,
                                         const Material *material,
                                         const Coord2f &tex_coord,
                                         const glm::vec3 &normal,
                                         const glm::vec3 &worldPos,
                                         int depth);

template<>
Color4f CommonShader::traceMaterial<ShadingType::Glass>(const RtcRayHitIor &rayHit,
                                          const Material *material,
                                          const Coord2f &tex_coord,
                                          const glm::vec3 &normal,
                                          const glm::vec3 &worldPos,
                                          int depth);

template<>
Color4f CommonShader::traceMaterial<ShadingType::Lambert>(const RtcRayHitIor &rayHit,
                                            const Material *material,
                                            const Coord2f &tex_coord,
                                            const glm::vec3 &normal,
                                            const glm::vec3 &worldPos,
                                            int depth);

template<>
Color4f CommonShader::traceMaterial<ShadingType::Mirror>(const RtcRayHitIor &rayHit,
                                           const Material *material,
                                           const Coord2f &tex_coord,
                                           const glm::vec3 &normal,
                                           const glm::vec3 &worldPos,
                                           int depth);

template<>
Color4f CommonShader::traceMaterial<ShadingType::Phong>(const RtcRayHitIor &rayHit,
                                          const Material *material,
                                          const Coord2f &tex_coord,
                                          const glm::vec3 &normal,
                                          const glm::vec3 &worldPos,
                                          int depth);

template<>
Color4f CommonShader::traceMaterial<ShadingType::PathTracing>(const RtcRayHitIor &rayHit,
                                                        const Material *material,
                                                        const Coord2f &tex_coord,
                                                        const glm::vec3 &normal,
                                                        const glm::vec3 &worldPos,
                                                        int depth);

#endif //PG1_2019_COMMONSHADER_H
