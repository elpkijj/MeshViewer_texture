#ifndef NORMAL_TEXTURE_SHADER_H
#define NORMAL_TEXTURE_SHADER_H

// ������ɫ������
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

"    // ���� TBN �������ڴ����߿ռ�ת��������ռ�\n"
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

// Ƭ����ɫ������
// Ƭ����ɫ�������޸ģ�
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
"    // �ӷ�����ͼ�л�ȡ���߲�ת��Ϊ [-1, 1] ��Χ\n"
"    vec3 normal = texture(normalMap, fs_in.TexCoords).rgb;\n"
"    normal = normalize(normal * 2.0 - 1.0);\n"

"    // ������շ���\n"
"    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);\n"
"    float diff = max(dot(lightDir, normal), 0.0);\n"
"    vec3 diffuse = diff * lightColor * 0.6;\n"

"    // ���㾵�淴�䣨Blinn-Phong ģ�ͣ�\n"
"    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);\n"
"    vec3 halfwayDir = normalize(lightDir + viewDir);\n"
"    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);\n"
"    vec3 specular = vec3(0.2) * spec * 0.5;\n"

"    // ��ȡ��������ɫ����Ϲ���\n"
"    vec3 color = texture(diffuseMap, fs_in.TexCoords).rgb;\n"
"    vec3 result = (0.2 * color) + diffuse + specular;\n"  // ����������ǿ��
"    FragColor = vec4(result, 1.0);\n"
"}\n";


#endif // NORMAL_TEXTURE_SHADER_H
