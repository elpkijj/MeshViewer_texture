#include "CObj.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>  // �������� GLM ����

//.obj�ļ�������
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

    //TODO����ģ���ļ��еĵ�������ݷֱ����m_pts��m_faces��
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
            point.normal = glm::vec3(0.0f);  // ��ʼ�����㷨��Ϊ 0
            m_pts.push_back(point);
        }
        else if (strLine[0] == 'f') // ����������
        {
            std::istringstream sin(strLine);
            sin >> s1 >> face.pts[0] >> face.pts[1] >> face.pts[2];
            ComputeFaceNormal(face);  // ������ķ���
            m_faces.push_back(face);
        }
    }

    fclose(fpFile);

    // ��һ��ģ�ͺͷ���
    UnifyModel();  // ��һ��ģ������
    ComputePointNormal();  // ���㷨��

    return true;
}

void CObj::UnifyModel()
{//Ϊͳһ��ʾ��ͬ�ߴ��ģ�ͣ���ģ�͹�һ������ģ�ͳߴ����ŵ�0.0-1.0֮��
//ԭ���ҳ�ģ�͵ı߽�������Сֵ�������ҳ�ģ�͵�����
//��ģ�͵����ĵ�Ϊ��׼��ģ�Ͷ����������
//TODO:���ģ�͹�һ������

    glm::vec3 vec_max(-1e5f, -1e5f, -1e5f), vec_min(1e5f, 1e5f, 1e5f);

    // ����ģ�͵�������Сֵ
    for (size_t i = 0; i < m_pts.size(); i++)
    {
        vec_max.x = std::max(vec_max.x, m_pts[i].pos.x);
        vec_max.y = std::max(vec_max.y, m_pts[i].pos.y);
        vec_max.z = std::max(vec_max.z, m_pts[i].pos.z);

        vec_min.x = std::min(vec_min.x, m_pts[i].pos.x);
        vec_min.y = std::min(vec_min.y, m_pts[i].pos.y);
        vec_min.z = std::min(vec_min.z, m_pts[i].pos.z);
    }

    // �������ĵ�
    glm::vec3 center = (vec_max + vec_min) * 0.5f;

    // ����ģ�͵����ߴ磬ȡ x��y��z �е�����ֵ��Ϊͳһ��������
    float max_dim = std::max(vec_max.x - vec_min.x, std::max(vec_max.y - vec_min.y, vec_max.z - vec_min.z));

    // ��ÿ��������й�һ��
    //�����ھ�ֵ��һ��
    for (size_t i = 0; i < m_pts.size(); i++)
    {
        m_pts[i].pos = (m_pts[i].pos - center) / max_dim;
    }
}

void CObj::ComputeFaceNormal(Face& f)
{// ֻ������f�ķ�������������
    // ��ȡ���������������
    glm::vec3 v1 = m_pts[f.pts[0] - 1].pos;
    glm::vec3 v2 = m_pts[f.pts[1] - 1].pos;
    glm::vec3 v3 = m_pts[f.pts[2] - 1].pos;

    // ������ķ��ߣ�����
    glm::vec3 faceNormal = glm::normalize(glm::cross(v2 - v1, v3 - v2));
    f.normal = faceNormal;  // �����߸�ֵ����
}

void CObj::ComputePointNormal()
{// ���㶥�㷨������������
    // ���Ƚ�ÿ������ķ��߳�ʼ��Ϊ 0
    for (size_t i = 0; i < m_pts.size(); i++) {
        m_pts[i].normal = glm::vec3(0.0f);
    }

    // ����ÿ���棬����ķ����ۼӵ���صĶ��㷨����
    for (size_t i = 0; i < m_faces.size(); i++) {
        Face& f = m_faces[i];
        glm::vec3 faceNormal = f.normal;

        m_pts[f.pts[0] - 1].normal += faceNormal;
        m_pts[f.pts[1] - 1].normal += faceNormal;
        m_pts[f.pts[2] - 1].normal += faceNormal;
    }

    // ��һ��ÿ������ķ��� ������һ�� x/||x||
   /* for (size_t i = 0; i < m_pts.size(); i++) {
        m_pts[i].normal = glm::normalize(m_pts[i].normal);
    }*/
}
