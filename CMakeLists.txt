CMAKE_MINIMUM_REQUIRED(VERSION 3.14)
PROJECT(PG1_2019)
#INCLUDE(cmake-precompiled-header/PrecompiledHeader.cmake)

SET(CMAKE_CXX_STANDARD 11)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-attributes")

#set(CMAKE_CXX_FLAGS "-Wall -Wextra")
SET(CMAKE_CXX_FLAGS_DEBUG "-g")
SET(CMAKE_CXX_FLAGS_RELEASE "-O3")

SET(SRC_ROOT src/pg/pg1_embree)
SET(SRC_ENGINE ${SRC_ROOT}/engine)
SET(SRC_GEOMETRY ${SRC_ROOT}/geometry)
SET(SRC_MATH ${SRC_ROOT}/math)
SET(SRC_SHADERS ${SRC_ROOT}/shaders)
SET(SRC_UTILS ${SRC_ROOT}/utils)

SET(LIB_ROOT src/libs)
SET(IMGUI_ROOT ${LIB_ROOT}/imgui)

INCLUDE_DIRECTORIES(${SRC_ROOT})
INCLUDE_DIRECTORIES(${IMGUI_ROOT})
INCLUDE_DIRECTORIES(${IMGUI_ROOT}/examples)

FIND_PACKAGE(Embree 3.6.1 REQUIRED)
INCLUDE_DIRECTORIES(${EMBREE_INCLUDE_DIRS})

FIND_PACKAGE(Doxygen)

IF (DOXYGEN_FOUND)
  SET(DOXYGEN_INPUT ${SOURCE_FILES})
  SET(DOXYGEN_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
  
  ADD_CUSTOM_COMMAND(
    OUTPUT ${DOXYGEN_OUTPUT}
    COMMAND ${CMAKE_COMMAND} -E echo_append "Building API Documentation..."
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_INPUT}
    COMMAND ${CMAKE_COMMAND} -E echo "Done."
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS ${DOXYGEN_INPUT}
  )
  
  ADD_CUSTOM_TARGET(apidoc ALL DEPENDS ${DOXYGEN_OUTPUT})
  
  ADD_CUSTOM_TARGET(apidoc_forced
                    COMMAND ${CMAKE_COMMAND} -E echo_append "Building API Documentation..."
                    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_INPUT}
                    COMMAND ${CMAKE_COMMAND} -E echo "Done."
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    )
ENDIF (DOXYGEN_FOUND)

SET(IMGUI
    ${IMGUI_ROOT}/imgui.cpp
    ${IMGUI_ROOT}/imgui.h
    ${IMGUI_ROOT}/imgui_demo.cpp
    ${IMGUI_ROOT}/imgui_draw.cpp
    ${IMGUI_ROOT}/imgui_widgets.cpp
    ${IMGUI_ROOT}/imgui_internal.h
    ${IMGUI_ROOT}/imconfig.h
    ${IMGUI_ROOT}/imstb_rectpack.h
    ${IMGUI_ROOT}/imstb_textedit.h
    ${IMGUI_ROOT}/imstb_truetype.h
    ${IMGUI_ROOT}/examples/imgui_impl_win32.cpp
    ${IMGUI_ROOT}/examples/imgui_impl_win32.h
    ${IMGUI_ROOT}/examples/imgui_impl_dx11.cpp
    ${IMGUI_ROOT}/examples/imgui_impl_dx11.h
    )

SET(PCH
    ${SRC_ROOT}/stdafx.cpp
    ${SRC_ROOT}/stdafx.h
    )

SET(ENGINE
    src/pg/pg1_embree/engine/camera.cpp
    src/pg/pg1_embree/engine/camera.h
    src/pg/pg1_embree/engine/raytracer.cpp
    src/pg/pg1_embree/engine/raytracer.h
    src/pg/pg1_embree/engine/simpleguidx11.cpp
    src/pg/pg1_embree/engine/simpleguidx11.h
    src/pg/pg1_embree/engine/light.cpp
    src/pg/pg1_embree/engine/light.h src/pg/pg1_embree/engine/sphericalmap.cpp src/pg/pg1_embree/engine/sphericalmap.h)

SET(GEOMETRY
    src/pg/pg1_embree/geometry/material.cpp
    src/pg/pg1_embree/geometry/material.h
    src/pg/pg1_embree/geometry/objloader.cpp
    src/pg/pg1_embree/geometry/objloader.h
    src/pg/pg1_embree/geometry/surface.cpp
    src/pg/pg1_embree/geometry/surface.h
    src/pg/pg1_embree/geometry/texture.cpp
    src/pg/pg1_embree/geometry/texture.h
    src/pg/pg1_embree/geometry/triangle.cpp
    src/pg/pg1_embree/geometry/triangle.h
    src/pg/pg1_embree/geometry/vertex.cpp
    src/pg/pg1_embree/geometry/vertex.h
    )

SET(MATH
    src/pg/pg1_embree/math/matrix3x3.cpp
    src/pg/pg1_embree/math/matrix3x3.h
    src/pg/pg1_embree/math/mymath.cpp
    src/pg/pg1_embree/math/mymath.h
    src/pg/pg1_embree/math/vector.cpp
    src/pg/pg1_embree/math/vector.h)

SET(SHADERS
    src/pg/pg1_embree/shaders/phongshader.cpp
    src/pg/pg1_embree/shaders/phongshader.h
    src/pg/pg1_embree/shaders/shader.cpp
    src/pg/pg1_embree/shaders/shader.h
    src/pg/pg1_embree/shaders/shaderenum.h src/pg/pg1_embree/shaders/diffuseshader.cpp src/pg/pg1_embree/shaders/diffuseshader.h src/pg/pg1_embree/shaders/normalsshader.cpp src/pg/pg1_embree/shaders/normalsshader.h src/pg/pg1_embree/shaders/recursivephongshader.cpp src/pg/pg1_embree/shaders/recursivephongshader.h)

SET(UTILS
    src/pg/pg1_embree/utils/structs.cpp
    src/pg/pg1_embree/utils/structs.h
    src/pg/pg1_embree/utils/utils.cpp
    src/pg/pg1_embree/utils/utils.h
    )

SET(SRC
    src/pg/pg1_embree/pg1_embree.cpp
    src/pg/pg1_embree/targetver.h
    src/pg/pg1_embree/tutorials.cpp
    src/pg/pg1_embree/tutorials.h
    src/pg/pg1_embree/stdafx.cpp src/pg/pg1_embree/shaders/glassshader.cpp src/pg/pg1_embree/shaders/glassshader.h)

ADD_EXECUTABLE(${PROJECT_NAME} ${SRC} ${UTILS} ${SHADERS} ${MATH} ${GEOMETRY} ${ENGINE} ${IMGUI} ${PCH})


TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${EMBREE_LIBRARY} D3DCompiler d3d11 libfreeimage xinput)

FILE(COPY data DESTINATION ${CMAKE_BINARY_DIR})