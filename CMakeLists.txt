cmake_minimum_required(VERSION 3.14)
project(PG1_2019)
#INCLUDE(cmake-precompiled-header/PrecompiledHeader.cmake)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-attributes")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

#set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(SRC_ROOT src/pg/pg1_embree)
set(SRC_ENGINE ${SRC_ROOT}/engine)
set(SRC_GEOMETRY ${SRC_ROOT}/geometry)
set(SRC_MATH ${SRC_ROOT}/math)
set(SRC_SHADERS ${SRC_ROOT}/shaders)
set(SRC_UTILS ${SRC_ROOT}/utils)

set(LIB_ROOT src/libs)
set(IMGUI_ROOT ${LIB_ROOT}/imgui)
set(GLM_ROOT ${LIB_ROOT}/glm)

include_directories(${SRC_ROOT})
include_directories(${IMGUI_ROOT})
include_directories(${IMGUI_ROOT}/examples)
include_directories(${GLM_ROOT})

find_package(Embree 3.6.1 REQUIRED)
include_directories(${EMBREE_INCLUDE_DIRS})

find_package(Doxygen)

if (DOXYGEN_FOUND)
  set(DOXYGEN_INPUT ${SOURCE_FILES})
  set(DOXYGEN_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
  
  add_custom_command(
    OUTPUT ${DOXYGEN_OUTPUT}
    COMMAND ${CMAKE_COMMAND} -E echo_append "Building API Documentation..."
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_INPUT}
    COMMAND ${CMAKE_COMMAND} -E echo "Done."
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS ${DOXYGEN_INPUT}
  )
  
  add_custom_target(apidoc ALL DEPENDS ${DOXYGEN_OUTPUT})
  
  add_custom_target(apidoc_forced
                    COMMAND ${CMAKE_COMMAND} -E echo_append "Building API Documentation..."
                    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_INPUT}
                    COMMAND ${CMAKE_COMMAND} -E echo "Done."
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    )
endif (DOXYGEN_FOUND)

set(IMGUI
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

set(PCH
    ${SRC_ROOT}/stdafx.cpp
    ${SRC_ROOT}/stdafx.h
    )

set(ENGINE
    src/pg/pg1_embree/engine/camera.cpp
    src/pg/pg1_embree/engine/camera.h
    src/pg/pg1_embree/engine/raytracer.cpp
    src/pg/pg1_embree/engine/raytracer.h
    src/pg/pg1_embree/engine/simpleguidx11.cpp
    src/pg/pg1_embree/engine/simpleguidx11.h
    src/pg/pg1_embree/engine/light.cpp
    src/pg/pg1_embree/engine/light.h
    src/pg/pg1_embree/engine/sphericalmap.cpp
    src/pg/pg1_embree/engine/sphericalmap.h src/pg/pg1_embree/engine/rtcrayhitior.h)

set(GEOMETRY
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

set(MATH
    src/pg/pg1_embree/math/mymath.h
    src/pg/pg1_embree/math/vector.h)

set(SHADERS
    src/pg/pg1_embree/shaders/phongshader.cpp
    src/pg/pg1_embree/shaders/phongshader.h
    src/pg/pg1_embree/shaders/shader.cpp
    src/pg/pg1_embree/shaders/shader.h
    src/pg/pg1_embree/shaders/shaderenum.h
    src/pg/pg1_embree/shaders/diffuseshader.cpp
    src/pg/pg1_embree/shaders/diffuseshader.h
    src/pg/pg1_embree/shaders/normalsshader.cpp
    src/pg/pg1_embree/shaders/normalsshader.h
    src/pg/pg1_embree/shaders/recursivephongshader.cpp
    src/pg/pg1_embree/shaders/recursivephongshader.h
    src/pg/pg1_embree/shaders/glassshader.cpp
    src/pg/pg1_embree/shaders/glassshader.h 
	src/pg/pg1_embree/shaders/commonshader.cpp 
	src/pg/pg1_embree/shaders/commonshader.h)

set(UTILS
    src/pg/pg1_embree/utils/structs.h
    src/pg/pg1_embree/utils/utils.cpp
    src/pg/pg1_embree/utils/utils.h
    )

set(SRC
    src/pg/pg1_embree/pg1_embree.cpp
    src/pg/pg1_embree/targetver.h
    src/pg/pg1_embree/tutorials.cpp
    src/pg/pg1_embree/tutorials.h
    src/pg/pg1_embree/stdafx.cpp)

set (TESTS
     src/pg/pg1_embree/tests/test.h 
	 src/pg/pg1_embree/shaders/shadingtype.h 
	 src/pg/pg1_embree/shaders/pathtracingshader.cpp 
	 src/pg/pg1_embree/shaders/pathtracingshader.h)

add_executable(${PROJECT_NAME} ${SRC} ${UTILS} ${SHADERS} ${MATH} ${GEOMETRY} ${ENGINE} ${TESTS} ${IMGUI} ${PCH})


find_package(OpenMP REQUIRED)
if (OpenMP_CXX_FOUND)
  target_link_libraries(${PROJECT_NAME} PUBLIC ${EMBREE_LIBRARY} D3DCompiler d3d11 libfreeimage xinput OpenMP::OpenMP_CXX)
else ()
  target_link_libraries(${PROJECT_NAME} ${EMBREE_LIBRARY} D3DCompiler d3d11 libfreeimage xinput)
endif ()

file(COPY data DESTINATION ${CMAKE_BINARY_DIR})