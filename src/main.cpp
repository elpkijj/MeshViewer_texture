#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtx/transform2.hpp>
#include<glm/gtx/euler_angles.hpp>

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <windows.h>

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"

#include "Geometry.h"
#include "CObj.h"

#include "Shader/ShaderSource.h"
#include "Shader/ModelShader.h"
#include "Arcball/arcball.h"

#include <cmath>  // 需要包含 cmath
#ifndef M_PI
#define M_PI 3.14159265358979323846  // 定义 M_PI 为圆周率
#endif

using namespace std;

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xposIn, double yposIn);



int Width = 700, Height = 700;
GLFWwindow* window;
GLuint vertex_shader, fragment_shader, shader, mshader;
//mvp位置，位置，颜色
GLint mvp_location, vpos_location, vcol_location;
GLint m_mvp_location, m_vpos_location, m_vcol_location, m_vnor_location;
//环境光，镜面光，漫反射光，光源方向，摄像机位置，模型矩阵位置
GLint light_ambient,light_specular,light_diffuse, light_dir, viewPos, m_model_location;
GLuint diffuseMap, normalMap, displacementMap, cubemapTexture;

glm::vec3* pos;
glm::vec4* color;
glm::vec3 cameraPos = glm::vec3(0, 0, 10);
unsigned int num = 0;
int id = 0;

bool lineMode = false;
bool modelTex = false;

//鼠标
float lastX = 400, lastY = 300, lastZ = 0;
float dx = 0.0f, dy = 0.0f;
//摄像机绕x y z的旋转角度
float pitch = 0.0f, yaw = 0.0f,roll = 0.0f;
bool firstMouse = true;


//坐标系变换
glm::mat4 trans = glm::translate(glm::vec3(0, 0, 0)); //不移动顶点坐标;
glm::mat4 rotation = glm::rotate(glm::radians(0.0f), glm::vec3(0, 0, 1.0f)); //不旋转;
glm::quat q_rot = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
float scaleNum = 2.0f;
glm::mat4 scale = glm::scale(glm::vec3(scaleNum, scaleNum, scaleNum)); //缩放;
glm::mat4 model, view, projection;
glm::mat4 mvp;
CArcball arcball = CArcball(Width, Height, Width/2, Height/2);

//加载模型
CObj obj;
bool loaded = false;

//基础几何图形
Quad* quad = new Quad();
Triangle* triangle = new Triangle();
Cube* cube = new Cube();
Cone* cone = new Cone();
Cylinder* cylinder = new Cylinder();
Wall* wall = new Wall();
SkyBox* skybox = new SkyBox();
Ball* ball = new Ball();


//初始化OpenGL
void init_opengl()
{
    cout << "init opengl" << endl;

    //设置错误回调
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    //创建窗口
    window = glfwCreateWindow(Width, Height, "MeshViewer", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    gladLoadGL();
    glfwSwapInterval(1);


}

//编译、链接Shader
GLuint compile_shader(const char* vertex,const char* fragment)
{
    //创建顶点Shader
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    //指定Shader源码
    glShaderSource(vertex_shader, 1, &vertex, NULL);
    //编译Shader
    glCompileShader(vertex_shader);
    //获取编译结果
    GLint compile_status = GL_FALSE;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compile_status);
    if (compile_status == GL_FALSE)
    {
        GLchar message[256];
        glGetShaderInfoLog(vertex_shader, sizeof(message), 0, message);
        cout << "compile vs error:" << message << endl;
    }

    //创建片段Shader
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    //指定Shader源码
    glShaderSource(fragment_shader, 1, &fragment, NULL);
    //编译Shader
    glCompileShader(fragment_shader);
    //获取编译结果
    compile_status = GL_FALSE;
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compile_status);
    if (compile_status == GL_FALSE)
    {
        GLchar message[256];
        glGetShaderInfoLog(fragment_shader, sizeof(message), 0, message);
        cout << "compile fs error:" << message << endl;
    }


    //创建GPU程序
    GLuint program = glCreateProgram();
    //附加Shader
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    //Link
    glLinkProgram(program);
    //获取编译结果
    GLint link_status = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &link_status);
    if (link_status == GL_FALSE)
    {
        GLchar message[256];
        glGetProgramInfoLog(program, sizeof(message), 0, message);
        cout << "link error:" << message << endl;
    }
    return program;
}

void Init_imgui()
{
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui::StyleColorsDark();

    const char* glsl_version = "#version 330";
    ImGui_ImplOpenGL3_Init(glsl_version);

}

unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        //根据颜色通道数确定图像格式
        //灰度图(黑白以及介于之间的灰色，如医学影像)
        if (nrComponents == 1)
            format = GL_RED;
        //标准rgb图
        else if (nrComponents == 3)
            format = GL_RGB;
        //带alpha通道的rgb图
        else if (nrComponents == 4)
            format = GL_RGBA;

        //设置纹理参数
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        //设置纹理环绕
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    //释放纹理数据返回id
    return textureID;
}

//加载立方体贴图
unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrComponents;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

bool loadObjFile()
{
    OPENFILENAME ofn;       // 文件对话框结构体
    TCHAR szFile[260];       // 存储文件名的缓冲区

    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = TEXT("OBJ Files(*.OBJ)\0");
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn) == TRUE) {
        std::cout << "Selected file: " << szFile << std::endl;
        obj.ReadObjFile(szFile);
        return true;
    }
    else {
        std::cout << "No file selected" << std::endl;
    }

    return false;
}

void drawModel()
{
    //TODO:使用索引绘制方式，创建VAO，VBO，EBO
    GLuint VAO, VBO, NBO, TBO, EBO;
    // 创建 VAO（顶点数组对象）
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // 创建 VBO 用于存储顶点数据（位置和法线）
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    std::vector<float> vertexData;
    std::vector<glm::vec2> texCoords;  // 用于存储球面映射生成的纹理坐标

    // 生成顶点和法线数据，并手动生成基于球面映射的纹理坐标
    for (size_t i = 0; i < obj.m_pts.size(); i++) {
        glm::vec3 pos = obj.m_pts[i].pos;
        glm::vec3 normal = obj.m_pts[i].normal;

        // 顶点位置
        vertexData.push_back(pos.x);
        vertexData.push_back(pos.y);
        vertexData.push_back(pos.z);

        // 顶点法线
        vertexData.push_back(normal.x);
        vertexData.push_back(normal.y);
        vertexData.push_back(normal.z);

        // 计算球面映射的纹理坐标 (u, v)
        float u = 0.5f + atan2(pos.z, pos.x) / (2.0f * 3.14159265359);  // u - 经度映射
        float v = 0.5f - asin(pos.y / glm::length(pos)) / 3.14159265359; // v - 纬度映射

        texCoords.push_back(glm::vec2(u, v));  // 将球面映射的纹理坐标存储到数组中
    }

    // 上传顶点和法线数据到 GPU
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);

    // 配置顶点属性指针 - 顶点坐标
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 配置顶点属性指针 - 顶点法线
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // 创建 TBO 用于存储纹理坐标
    glGenBuffers(1, &TBO);
    glBindBuffer(GL_ARRAY_BUFFER, TBO);

    // 上传纹理坐标数据到 GPU
    glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(glm::vec2), texCoords.data(), GL_STATIC_DRAW);

    // 配置纹理坐标属性指针
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);

    // 创建并绑定 EBO（索引缓冲对象）
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    // 提取索引数据
    std::vector<unsigned int> indices;
    for (size_t i = 0; i < obj.m_faces.size(); i++) {
        indices.push_back(obj.m_faces[i].pts[0] - 1);  // 从 1 开始，需要减去 1
        indices.push_back(obj.m_faces[i].pts[1] - 1);
        indices.push_back(obj.m_faces[i].pts[2] - 1);
    }

    // 上传索引数据到 GPU
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glm::vec3 lightDir = glm::vec3(0.0f, -1.0f, -1.0f);
    glm::vec3 lightAmbient = glm::vec3(0.2f, 0.2f, 0.2f);
    glm::vec3 lightDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
    glm::vec3 lightSpecular = glm::vec3(1.0f, 1.0f, 1.0f);



    m_mvp_location = glGetUniformLocation(mshader, "u_mvp");
    m_vpos_location = glGetAttribLocation(mshader, "a_pos");
    m_vnor_location = glGetAttribLocation(mshader, "a_normal");
    light_dir = glGetUniformLocation(mshader, "light.direction");
    light_ambient = glGetUniformLocation(mshader, "light.ambient");
    light_diffuse = glGetUniformLocation(mshader, "light.diffuse");
    light_specular = glGetUniformLocation(mshader, "light.specular");
    viewPos = glGetUniformLocation(mshader, "viewPos");
    m_model_location = glGetUniformLocation(mshader, "model");
    GLint useTex = glGetUniformLocation(mshader, "useTex");

    glUseProgram(mshader);
    glUniformMatrix4fv(m_mvp_location, 1, GL_FALSE, &mvp[0][0]);
    glUniform3fv(viewPos, 1, &cameraPos[0]);
    glUniformMatrix4fv(m_model_location, 1, GL_FALSE, &model[0][0]);
    glUniform3fv(light_dir, 1, &lightDir[0]);
    glUniform3fv(light_ambient, 1, &lightAmbient[0]);
    glUniform3fv(light_specular, 1, &lightSpecular[0]);
    glUniform3fv(light_diffuse, 1, &lightDiffuse[0]);
    glUniform1i(useTex, modelTex);

    //TODO:绑定贴图，绘制
    // 传递是否使用纹理的标志
    glUniform1i(useTex, modelTex);

    // 如果使用纹理，绑定纹理
    if (modelTex) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);  // 使用预加载的纹理
        glUniform1i(glGetUniformLocation(mshader, "chessTex"), 0);
    }

    // 绘制模型
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    // 清除绑定
    glBindVertexArray(0);

    // 删除缓冲对象
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &TBO);
    glDeleteVertexArrays(1, &VAO);
}

void drawNormalTexture()
{
    //TODO:自行创建并编写shader，实现法线贴图效果
    //TIPS:Wall.h中存储了绘制矩形的相关数据
    
}

void drawSkyBox()
{
    //TODO:自行创建并编写shader，实现环境贴图效果
    //TIPS:ball.h中存储了绘制球体的数据，skybox.h中存储了实现天空盒相关数据

}

void drawDisplacement()
{
    //TODO:自行创建并编写shader，实现置换贴图效果
    //TIPS:ball.h中存储了绘制球体的数据
    

}


int main(void)
{
    init_opengl();

    shader = compile_shader(vertex_shader_text,fragment_shader_text);
    mshader = compile_shader(mvertex_shader_text, mfragment_shader_text);

    //TODO:载入需要的贴图
    //TIPS:使用loadTexture和loadCubemap函数
    //diffuseMap = loadTexture("../../../resources/textures/wall_diffuse.png");
    diffuseMap = loadTexture("../../../resources/textures/brickwall.jpg");
    //diffuseMap = loadTexture("../../../resources/textures/chess.jpeg");

    Init_imgui();

    //获取shader属性ID
    mvp_location = glGetUniformLocation(shader, "u_mvp");
    vpos_location = glGetAttribLocation(shader, "a_pos");
    vcol_location = glGetAttribLocation(shader, "a_color");


    while (!glfwWindowShouldClose(window))
    {
        cameraPos = glm::vec3(0, 0, 10);

        if(lineMode)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        float ratio;
        //获取画面宽高
        glfwGetFramebufferSize(window, &Width, &Height);
        ratio = Width / (float)Height;

        glViewport(0, 0, Width, Height);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(49.f / 255, 77.f / 255, 121.f / 255, 1.f);

        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Setup");

            ImGui::Checkbox("Line Mode", &lineMode);
            ImGui::Checkbox("ModelTexture", &modelTex);// Edit bools storing our window open/close state

            if (ImGui::Button("Triangle"))
                id = 0;
            if (ImGui::Button("Quad"))
                id = 1;
            if (ImGui::Button("Cube"))
                id = 2;
            if (ImGui::Button("Cone"))
                id = 3;
            if (ImGui::Button("Cylinder"))
                id = 4;
            if (ImGui::Button("Model"))
            {
                id = 5;
                loaded = loadObjFile();
            }
            if (ImGui::Button("NormalTex"))
                id = 6;
            if (ImGui::Button("SkyBox"))
                id = 7;
            if (ImGui::Button("Displacement"))
                id = 8;

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        switch (id)
        {
        case 0:
            pos = triangle->getPos();
            color = triangle->getColor();
            num = triangle->getNum();
            break;
        case 1:
            pos = quad->getPos();
            color = quad->getColor();
            num = quad->getNum();
            break;
        case 2:
            pos = cube->getPos();
            color = cube->getColor();
            num = cube->getNum();
            break;
        case 3:
            pos = cone->getPos();
            color = cone->getColor();
            num = cone->getNum();
            break;
        case 4:
            pos = cylinder->getPos();
            color = cylinder->getColor();
            num = cylinder->getNum();
            break;
        case 5:
        case 6:
        case 7:
        case 8:
            break;
        default:
            cout << "wrong" << endl;
        }

        //坐标系变换
        scale = glm::scale(glm::vec3(scaleNum, scaleNum, scaleNum)); //缩放;

        model = trans * scale * rotation;

        view = glm::lookAt(cameraPos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

        projection = glm::perspective(glm::radians(60.f), ratio, 1.f, 1000.f);

        mvp = projection * view * model;
        
        switch(id)
        {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
            //指定GPU程序(就是指定顶点着色器、片段着色器)
            glUseProgram(shader);
            glEnable(GL_DEPTH_TEST);

            //启用顶点Shader属性(a_pos)，指定与顶点坐标数据进行关联
            glEnableVertexAttribArray(vpos_location);
            glVertexAttribPointer(vpos_location, 3, GL_FLOAT, false, sizeof(glm::vec3), pos);

            //启用顶点Shader属性(a_color)，指定与顶点颜色数据进行关联
            glEnableVertexAttribArray(vcol_location);
            glVertexAttribPointer(vcol_location, 3, GL_FLOAT, false, sizeof(glm::vec4), color);

            //上传mvp矩阵
            glUniformMatrix4fv(mvp_location, 1, GL_FALSE, &mvp[0][0]);
            //上传顶点数据并进行绘制
            glDrawArrays(GL_TRIANGLES, 0, num);
            break;
        case 5:
            if (loaded) drawModel();
            break;
        case 6:
            drawNormalTexture();
            break;
        case 7:
            drawSkyBox();
            break;
        case 8:
            drawDisplacement();
            break;
        default:
            cout << "wrong" << endl;
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    //TODO:通过鼠标操作实现旋转，平移
    //TIPS:修改平移，旋转矩阵
    int leftButtonState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    int rightButtonState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);

    if (firstMouse)
    {
        lastX = xposIn;
        lastY = yposIn;
        firstMouse = false;
    }

    float xoffset = xposIn - lastX;
    float yoffset = lastY - yposIn;
    lastX = xposIn;
    lastY = yposIn;

    if (leftButtonState == GLFW_PRESS)
    {
        float sensitivity = 0.01f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        trans = glm::translate(trans, glm::vec3(xoffset, yoffset, 0.0f));
    }

    if (rightButtonState == GLFW_PRESS)
    {
        float sensitivity = 0.005f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

        glm::quat pitchQuat = glm::angleAxis(-yoffset, right);
        glm::quat yawQuat = glm::angleAxis(-xoffset, up);

        q_rot = yawQuat * pitchQuat * q_rot;
        rotation = glm::mat4_cast(q_rot);
    }
}

void scroll_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    //TODO：鼠标滚轮操作，修改scaleNum变量以实现缩放
    if (yposIn > 0) {
        scaleNum += 0.1f;
    }
    else if (yposIn < 0) {
        scaleNum -= 0.1f;
        if (scaleNum < 0.1f) scaleNum = 0.1f;
    }

    scale = glm::scale(glm::vec3(scaleNum, scaleNum, scaleNum));
}