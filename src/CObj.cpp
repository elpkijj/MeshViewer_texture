#include "CObj.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>  // 包含常用 GLM 函数

//.obj文件的数据
/*
# 10242 vertices, 20480 faces
v 0.5510331392 0.2215948999 0.0181707479
f 8929 2485 2488

*/
CObj::CObj(void)
{
}

CObj::~CObj(void)
{
}

bool CObj::ReadObjFile(const char* pcszFileName)
{
    FILE* fpFile = fopen(pcszFileName, "r");
    if (fpFile == NULL)
    {
        return false;
    }

    m_pts.clear();
    m_faces.clear();

    //TODO：将模型文件中的点和面数据分别存入m_pts和m_faces中
    char strLine[1024];
    Point point;
    Face face;
    std::string s1;

    while (!feof(fpFile))
    {
        fgets(strLine, 1024, fpFile);
        if (strLine[0] == 'v')
        {
            std::istringstream sin(strLine);
            sin >> s1 >> point.pos.x >> point.pos.y >> point.pos.z;
            point.normal = glm::vec3(0.0f);  // 初始化顶点法线为 0
            m_pts.push_back(point);
        }
        else if (strLine[0] == 'f') // 处理面数据
        {
            std::istringstream sin(strLine);
            sin >> s1 >> face.pts[0] >> face.pts[1] >> face.pts[2];
            ComputeFaceNormal(face);  // 计算面的法线
            m_faces.push_back(face);
        }
    }

    fclose(fpFile);

    // 归一化模型和法线
    UnifyModel();  // 归一化模型坐标
    ComputePointNormal();  // 顶点法线

    return true;
}

void CObj::UnifyModel()
{//为统一显示不同尺寸的模型，将模型归一化，将模型尺寸缩放到0.0-1.0之间
//原理：找出模型的边界最大和最小值，进而找出模型的中心
//以模型的中心点为基准对模型顶点进行缩放
//TODO:添加模型归一化代码

    glm::vec3 vec_max(-1e5f, -1e5f, -1e5f), vec_min(1e5f, 1e5f, 1e5f);

    // 计算模型的最大和最小值
    for (size_t i = 0; i < m_pts.size(); i++)
    {
        vec_max.x = std::max(vec_max.x, m_pts[i].pos.x);
        vec_max.y = std::max(vec_max.y, m_pts[i].pos.y);
        vec_max.z = std::max(vec_max.z, m_pts[i].pos.z);

        vec_min.x = std::min(vec_min.x, m_pts[i].pos.x);
        vec_min.y = std::min(vec_min.y, m_pts[i].pos.y);
        vec_min.z = std::min(vec_min.z, m_pts[i].pos.z);
    }

    // 计算中心点
    glm::vec3 center = (vec_max + vec_min) * 0.5f;

    // 计算模型的最大尺寸，取 x、y、z 中的最大差值作为统一缩放因子
    float max_dim = std::max(vec_max.x - vec_min.x, std::max(vec_max.y - vec_min.y, vec_max.z - vec_min.z));

    // 对每个顶点进行归一化
    //类似于均值归一化
    for (size_t i = 0; i < m_pts.size(); i++)
    {
        m_pts[i].pos = (m_pts[i].pos - center) / max_dim;
    }
}

void CObj::ComputeFaceNormal(Face& f)
{// 只计算面f的法向量，并保存
    // 获取面的三个顶点坐标
    glm::vec3 v1 = m_pts[f.pts[0] - 1].pos;
    glm::vec3 v2 = m_pts[f.pts[1] - 1].pos;
    glm::vec3 v3 = m_pts[f.pts[2] - 1].pos;

    // 计算面的法线，求叉积
    glm::vec3 faceNormal = glm::normalize(glm::cross(v2 - v1, v3 - v2));
    f.normal = faceNormal;  // 将法线赋值给面
}

void CObj::ComputePointNormal()
{// 计算顶点法向量，并保存
    // 首先将每个顶点的法线初始化为 0
    for (size_t i = 0; i < m_pts.size(); i++) {
        m_pts[i].normal = glm::vec3(0.0f);
    }

    // 遍历每个面，将面的法线累加到相关的顶点法线上
    for (size_t i = 0; i < m_faces.size(); i++) {
        Face& f = m_faces[i];
        glm::vec3 faceNormal = f.normal;

        m_pts[f.pts[0] - 1].normal += faceNormal;
        m_pts[f.pts[1] - 1].normal += faceNormal;
        m_pts[f.pts[2] - 1].normal += faceNormal;
    }

    // 归一化每个顶点的法线 向量归一化 x/||x||
   /* for (size_t i = 0; i < m_pts.size(); i++) {
        m_pts[i].normal = glm::normalize(m_pts[i].normal);
    }*/
}
