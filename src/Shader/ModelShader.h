//
// Created by captain on 2021/3/25.
//

#ifndef MESHVIEWER_MODELSHADER_H
#define MESHVIEWER_MODELSHADER_H
//TODO:在顶点着色器中计算uv坐标，在片段着色器中使用贴图颜色
//顶点着色器代码
static const char* mvertex_shader_text =
"#version 330\n"

"uniform mat4 u_mvp;\n"
"uniform mat4 model;\n"

"in vec3 a_pos;\n"
"in vec3 a_normal;\n"
"out vec3 Normal;\n"
"out vec3 FragPos;\n"
"out vec2 TexCoord;\n"

"void main()\n"
"{\n"
"    gl_Position = u_mvp * vec4(a_pos, 1.0);\n"
"    Normal = mat3(transpose(inverse(model))) * a_normal;\n"
"    FragPos = vec3(model * vec4(a_pos, 1.0));\n"
"    float u = 0.5 + atan(a_pos.z, a_pos.x) / (2.0 * 3.14159265359);\n"
"    float v = 0.5 - asin(a_pos.y / length(a_pos)) / 3.14159265359;\n"
"    TexCoord = vec2(u, v);\n"
"}\n";

//片段着色器代码
static const char* mfragment_shader_text =
"#version 330\n"
"struct Light {\n"
"    vec3 direction;\n"
"    vec3 ambient;\n"
"    vec3 diffuse;\n"
"    vec3 specular;\n"
"};\n"

"in vec3 Normal;\n"
"in vec3 FragPos;\n"
"in vec2 TexCoord;\n"

"uniform Light light;\n"
"uniform vec3 viewPos;\n"
"uniform bool useTex;\n"
"uniform sampler2D chessTex;\n"

"out vec4 FragColor;\n"

"void main()\n"
"{\n"
"   vec3 objectColor;"
"   if(useTex)\n"
"       //TODO:从贴图中获取颜色\n"
"       objectColor = texture(chessTex, TexCoord).rgb;\n"
"   else\n"
"       objectColor = vec3(0.5, 0.5, 0.5);\n"
"	// ambient\n"
"	vec3 ambient = light.ambient * objectColor;\n"
"	// diffuse\n"
"	vec3 norm = normalize(Normal);\n"
"	vec3 lightDir = normalize(-light.direction);\n"
"	float diff = max(dot(norm, lightDir), 0.0);\n"
"	vec3 diffuse = light.diffuse * diff * objectColor;\n"
"	// specular\n"
"	vec3 viewDir = normalize(viewPos - FragPos);\n"
"	vec3 reflectDir = reflect(-lightDir, norm);\n"
"	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);\n"
"	vec3 specular = light.specular * spec * objectColor;\n"

"	vec3 result = ambient + diffuse + specular;\n"
"	FragColor = vec4(result, 1.0);\n"
"}\n";

#endif //UNTITLED_SHADERSOURCE_H
