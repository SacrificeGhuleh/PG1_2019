#ifndef OBJ_LOADER_H_
#define OBJ_LOADER_H_

#include <glm/vec3.hpp>

#include <geometry/surface.h>

/*! \fn int LoadOBJ( const char * file_name, glm::vec3 & default_color, std::vector<Surface *> & surfaces, std::vector<Material *> & materials )
\brief Načte geometrii z OBJ souboru \a file_name.
\note Při exportu z 3ds max je nutné nastavit systémové jednotky na metry:
Customize -> Units Setup Metric (Meters)
System Unit Setup -> 1 Unit = 1,0 m a zaškrtnout Respect System Units in File
\see 
\param file_name úplná cesta k OBJ souboru včetně přípony.
\param surfaces pole ploch, do kterého se budou ukládat načtené plochy.
\param materials pole materiálů, do kterého se budou ukládat načtené materiály.
\param flip_yz rotace kolem osy x o + 90st.
\param default_color výchozí barva vertexu.
*/
int LoadOBJ(const char *file_name, std::vector<Surface *> &surfaces, std::vector<Material *> &materials,
            bool flip_yz = false, glm::vec3 default_color = glm::vec3(0.5f, 0.5f, 0.5f));

#endif
