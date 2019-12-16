#ifndef VERTEX_H_
#define VERTEX_H_

#include <utils/structs.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

/*! \def NO_TEXTURE_COORDS
\brief Počet texturovacích souřadnic.
*/
#define NO_TEXTURE_COORDS 1

//class Surface;

/*! \struct Vertex
\brief Struktura popisující všechny atributy vertexu.

\author Tomáš Fabián
\version 1.0
\date 2013
*/
struct /*ALIGN*/ Vertex {
public:
  glm::vec3 position{}; /*!< Pozice vertexu. */
  glm::vec3 normal{}; /*!< Normála vertexu. */
  glm::vec3 color{}; /*!< RGB barva vertexu <0, 1>^3. */
  glm::vec2 texture_coords[NO_TEXTURE_COORDS]{}; /*!< Texturovací souřadnice. */
  glm::vec3 tangent{}; /*!< První osa souřadného systému tangenta-bitangenta-normála. */
  
  char pad[8]{}; // doplnění na 64 bytů, mělo by to mít alespoň 4 byty, aby se sem vešel 32-bitový ukazatel
  
  //! Výchozí konstruktor.
  /*!
  Inicializuje všechny složky vertexu na hodnotu nula.
  */
  Vertex() = default;
  
  //! Obecný konstruktor.
  /*!
  Inicializuje vertex podle zadaných hodnot parametrů.

  \param position pozice vertexu.
  \param normal normála vertexu.
  \param color barva vertexu.
  \param texture_coords nepovinný ukazatel na pole texturovacích souřadnic.
  */
  Vertex(glm::vec3 position, glm::vec3 normal, glm::vec3 color, glm::vec2 *texture_coords = nullptr);
  
  //void Print();
};

#endif
