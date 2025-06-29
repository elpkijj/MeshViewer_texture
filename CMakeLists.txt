﻿cmake_minimum_required(VERSION 2.8)
project(MeshViewer)

set(CMAKE_CXX_STANDARD 17)

if(MSVC)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
endif()

add_definitions(-w)
# SET(CMAKE_BUILD_TYPE "Debug")  # 定义编译类型
# SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb") # 定义Debug编译参数
# SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall") # 定义Release编译参数


include_directories(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/src
                                    ${PROJECT_SOURCE_DIR}/thirdparty)
link_directories(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/lib)

add_executable(${PROJECT_NAME}
    src/glad.c 
    src/main.cpp 
    src/CObj.cpp
    src/image.cpp
    src/Arcball/arcball.cpp
    src/Arcball/Geometry.cpp
    src/Arcball/Quat.cpp
    thirdparty/imgui/backends/imgui_impl_glfw.cpp
    thirdparty/imgui/backends/imgui_impl_opengl3.cpp
    thirdparty/imgui/imgui.cpp
    thirdparty/imgui/imgui_draw.cpp
    thirdparty/imgui/imgui_tables.cpp
    thirdparty/imgui/imgui_widgets.cpp
  "src/Shader/NormalTextureShader.h" "src/Shader/SkyboxShader.h" "src/Shader/DisplacementShader.h")

target_link_libraries(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/lib/glfw3.lib)
#target_link_libraries(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/lib/imgui.lib)
target_link_libraries(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/lib/OpenGL32.lib)