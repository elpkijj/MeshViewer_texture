#ifndef NORMAL_TEXTURE_SHADER_H
#define NORMAL_TEXTURE_SHADER_H

// 顶点着色器代码
static const char* normal_texture_vertex_shader_text =
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aNormal;\n"
"layout (location = 2) in vec2 aTexCoords;\n"
"layout (location = 3) in vec3 aTangent;\n"
"layout (location = 4) in vec3 aBitangent;\n"

"out VS_OUT {\n"
"    vec3 FragPos;\n"
"    vec2 TexCoords;\n"
"    vec3 TangentLightPos;\n"
"    vec3 TangentViewPos;\n"
"    vec3 TangentFragPos;\n"
"} vs_out;\n"

"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"uniform vec3 lightPos;\n"
"uniform vec3 viewPos;\n"

"void main()\n"
"{\n"
"    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));\n"
"    vs_out.TexCoords = aTexCoords;\n"

"    // 计算 TBN 矩阵，用于从切线空间转换到世界空间\n"
"    mat3 normalMatrix = transpose(inverse(mat3(model)));\n"
"    vec3 T = normalize(normalMatrix * aTangent);\n"
"    vec3 N = normalize(normalMatrix * aNormal);\n"
"    T = normalize(T - dot(T, N) * N);\n"
"    vec3 B = cross(N, T);\n"
"    mat3 TBN = transpose(mat3(T, B, N));\n"

"    vs_out.TangentLightPos = TBN * lightPos;\n"
"    vs_out.TangentViewPos = TBN * viewPos;\n"
"    vs_out.TangentFragPos = TBN * vs_out.FragPos;\n"

"    gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
"}\n";

// 片段着色器代码
// 片段着色器代码修改：
static const char* normal_texture_fragment_shader_text =
"#version 330 core\n"
"out vec4 FragColor;\n"

"in VS_OUT {\n"
"    vec3 FragPos;\n"
"    vec2 TexCoords;\n"
"    vec3 TangentLightPos;\n"
"    vec3 TangentViewPos;\n"
"    vec3 TangentFragPos;\n"
"} fs_in;\n"

"uniform sampler2D diffuseMap;\n"
"uniform sampler2D normalMap;\n"
"uniform vec3 lightColor;\n"

"void main()\n"
"{\n"
"    // 从法线贴图中获取法线并转换为 [-1, 1] 范围\n"
"    vec3 normal = texture(normalMap, fs_in.TexCoords).rgb;\n"
"    normal = normalize(normal * 2.0 - 1.0);\n"

"    // 计算光照方向\n"
"    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);\n"
"    float diff = max(dot(lightDir, normal), 0.0);\n"
"    vec3 diffuse = diff * lightColor * 0.6;\n"

"    // 计算镜面反射（Blinn-Phong 模型）\n"
"    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);\n"
"    vec3 halfwayDir = normalize(lightDir + viewDir);\n"
"    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);\n"
"    vec3 specular = vec3(0.2) * spec * 0.5;\n"

"    // 获取漫反射颜色并组合光照\n"
"    vec3 color = texture(diffuseMap, fs_in.TexCoords).rgb;\n"
"    vec3 result = (0.2 * color) + diffuse + specular;\n"  // 调整环境光强度
"    FragColor = vec4(result, 1.0);\n"
"}\n";


#endif // NORMAL_TEXTURE_SHADER_H
