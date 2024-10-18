//
// Created by captain on 2021/3/25.
//

#ifndef MESHVIEWER_SKYBOXSHADER_H
#define MESHVIEWER_SKYBOXSHADER_H

// 天空盒顶点着色器代码
static const char* skybox_vertex_shader_text =
"#version 330\n"
"uniform mat4 projection;\n"
"uniform mat4 view;\n"

"in vec3 a_pos;\n"

"out vec3 TexCoords;\n"

"void main()\n"
"{\n"
"    TexCoords = a_pos;\n"
"    gl_Position = projection * view * vec4(a_pos, 1.0);\n"
"    gl_Position = gl_Position.xyww; // 保持深度为1.0\n"
"}\n";

// 天空盒片段着色器代码
static const char* skybox_fragment_shader_text =
"#version 330\n"

"uniform samplerCube skybox;\n"

"in vec3 TexCoords;\n"
"out vec4 FragColor;\n"

"void main()\n"
"{\n"
"    FragColor = texture(skybox, TexCoords);\n"
"}\n";

// 球体反射顶点着色器代码
static const char* ball_reflection_vertex_shader_text =
"#version 330\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"

"in vec3 a_pos;\n"
"in vec3 a_normal;\n"

"out vec3 WorldPos;\n"
"out vec3 Normal;\n"

"void main()\n"
"{\n"
"    WorldPos = vec3(model * vec4(a_pos, 1.0));\n"
"    Normal = mat3(transpose(inverse(model))) * a_normal;\n"
"    gl_Position = projection * view * vec4(WorldPos, 1.0);\n"
"}\n";

// 球体反射片段着色器代码
static const char* ball_reflection_fragment_shader_text =
"#version 330\n"
"uniform samplerCube skybox;\n"
"uniform vec3 cameraPos;\n"

"in vec3 WorldPos;\n"
"in vec3 Normal;\n"

"out vec4 FragColor;\n"

"void main()\n"
"{\n"
"    vec3 I = normalize(WorldPos - cameraPos);\n"
"    vec3 R = reflect(I, normalize(Normal));\n"
"    FragColor = texture(skybox, R);\n"
"}\n";

#endif //MESHVIEWER_SKYBOXSHADER_H
