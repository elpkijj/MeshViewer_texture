#ifndef DISPLACEMENT_SHADER_H
#define DISPLACEMENT_SHADER_H

// 顶点着色器代码 - 置换贴图
static const char* displacement_vertex_shader_text =
"#version 330 core\n"
"layout(location = 0) in vec3 aPos;\n"
"layout(location = 1) in vec2 aTexCoords;\n"

"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"uniform sampler2D displacementMap;\n"
"uniform float heightScale;\n"

"out vec2 TexCoords;\n"

"void main()\n"
"{\n"
"    float displacement = texture(displacementMap, aTexCoords).r * heightScale;\n"
"    vec3 newPos = aPos + displacement * normalize(aPos);\n"
"    gl_Position = projection * view * model * vec4(newPos, 1.0);\n"
"    TexCoords = aTexCoords;\n"
"}\n";

// 片段着色器代码 - 置换贴图
static const char* displacement_fragment_shader_text =
"#version 330 core\n"
"in vec2 TexCoords;\n"

"uniform sampler2D textureMap;\n"
"uniform sampler2D ambientMap;\n" // 环境光遮蔽贴图
"uniform sampler2D roughnessMap;\n" // 粗糙度贴图

"out vec4 FragColor;\n"

"void main()\n"
"{\n"
"    vec3 color = texture(textureMap, TexCoords).rgb;\n"
"    float ambientOcclusion = texture(ambientMap, TexCoords).r;\n"
"    float roughness = texture(roughnessMap, TexCoords).r;\n"

"    // 应用环境光遮蔽对颜色进行调整\n"
"    color *= ambientOcclusion;\n"

"    // 基于粗糙度对光照效果进行调整（假设使用简单的粗糙度调整）\n"
"    vec3 finalColor = mix(color * 0.5, color, roughness);\n"

"    FragColor = vec4(finalColor, 1.0);\n"
"}\n";

#endif // DISPLACEMENT_SHADER_H
