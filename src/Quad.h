#pragma once

#include <glm/glm.hpp>

class Quad
{
public:
    Quad() {};
    glm::vec3* getPos()
    {
        return kPositions;
    }
    glm::vec4* getColor()
    {
        return kColors;
    }
    unsigned int getNum()
    {
        return num;
    }
private:
    glm::vec3 kPositions[6] = {
        //��һ��������
        { -1.0f, -1.0f,0.0f},//����
        {  1.0f, -1.0f,0.0f},//����
        {  1.0f,  1.0f,0.0f},//����

        //�ڶ���������
        {  1.0f,  1.0f,0.0f},//����
        { -1.0f,  1.0f,0.0f},//����
        { -1.0f, -1.0f,0.0f} //����
    };
    glm::vec4 kColors[6] = {
        //��һ����������ɫ
        { 1.f, 0.f, 0.f ,1.f},//����
        { 0.f, 1.f, 0.f ,1.f},//����
        { 0.f, 0.f, 1.f ,1.f},//����

        //�ڶ�����������ɫ
        { 0.f, 0.f, 1.f ,1.f},//����
        { 1.f, 0.f, 0.f ,1.f},//����
        { 1.f, 0.f, 0.f ,1.f} //����
    };
    unsigned int num = 6;
};

