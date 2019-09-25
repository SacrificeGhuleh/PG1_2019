CMAKE_MINIMUM_REQUIRED(VERSION 3.14)
PROJECT(PG1_2019)
#INCLUDE(cmake-precompiled-header/PrecompiledHeader.cmake)

SET(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-attributes")

#set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

SET(SRC_ROOT src/pg/pg1_embree)
SET(LIB_ROOT src/libs)
SET(IMGUI_ROOT ${LIB_ROOT}/imgui)

INCLUDE_DIRECTORIES(${SRC_ROOT})
INCLUDE_DIRECTORIES(${IMGUI_ROOT})
INCLUDE_DIRECTORIES(${IMGUI_ROOT}/examples)

find_package(Embree 3.6.1 REQUIRED)
include_directories(${EMBREE_INCLUDE_DIRS})

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
    ${SRC_ROOT}/stdafx.h)

SET(SRC
    ${SRC_ROOT}/camera.cpp
    ${SRC_ROOT}/camera.h
    ${SRC_ROOT}/material.cpp
    ${SRC_ROOT}/material.h
    ${SRC_ROOT}/matrix3x3.cpp
    ${SRC_ROOT}/matrix3x3.h
    ${SRC_ROOT}/mymath.cpp
    ${SRC_ROOT}/mymath.h
    ${SRC_ROOT}/objloader.cpp
    ${SRC_ROOT}/objloader.h
    ${SRC_ROOT}/pg1_embree.cpp
    ${SRC_ROOT}/raytracer.cpp
    ${SRC_ROOT}/raytracer.h
    ${SRC_ROOT}/simpleguidx11.cpp
    ${SRC_ROOT}/simpleguidx11.h
    ${SRC_ROOT}/structs.cpp
    ${SRC_ROOT}/structs.h
    ${SRC_ROOT}/surface.cpp
    ${SRC_ROOT}/surface.h
    ${SRC_ROOT}/targetver.h
    ${SRC_ROOT}/texture.cpp
    ${SRC_ROOT}/texture.h
    ${SRC_ROOT}/triangle.cpp
    ${SRC_ROOT}/triangle.h
    ${SRC_ROOT}/tutorials.cpp
    ${SRC_ROOT}/tutorials.h
    ${SRC_ROOT}/utils.cpp
    ${SRC_ROOT}/utils.h
    ${SRC_ROOT}/vector3.cpp
    ${SRC_ROOT}/vector3.h
    ${SRC_ROOT}/vertex.cpp
    ${SRC_ROOT}/vertex.h)



ADD_EXECUTABLE(${PROJECT_NAME} ${SRC} ${IMGUI} ${PCH})

#ADD_PRECOMPILED_HEADER(${PROJECT_NAME} ${SRC_ROOT}/stdafx.h SOURCE_CXX ${SRC_ROOT}/stdafx.cpp FORCEINCLUDE)

target_link_libraries(${PROJECT_NAME} ${EMBREE_LIBRARY} D3DCompiler d3d11 libfreeimage xinput)

file(COPY data DESTINATION ${CMAKE_BINARY_DIR})

#add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#                   COMMAND ${CMAKE_COMMAND} -E copy_directory
#                   data $<TARGET_FILE_DIR:${PROJECT_NAME}>/data)