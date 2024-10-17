#pragma once

#include <glm/glm.hpp>

class Cylinder
{
public:
    Cylinder(float radius = 1.0f, float height = 2.0f, unsigned int numSlices = 500) {
        //TODO:���㶥�����ɫ������
        num = 0;

        float angleStep = 2.0f * glm::pi<float>() / numSlices;

        glm::vec3 topCenter = glm::vec3(0.0f, height / 2.0f, 0.0f);
        glm::vec3 bottomCenter = glm::vec3(0.0f, -height / 2.0f, 0.0f);

        // ���ɶ����͵ײ���Բ��
        for (unsigned int i = 0; i < numSlices; ++i)
        {
            float theta = i * angleStep;
            float nextTheta = (i + 1) * angleStep;

            // ��Բ��������Ե����
            glm::vec3 topVertex1 = glm::vec3(radius * cos(theta), height / 2.0f, radius * sin(theta));
            glm::vec3 topVertex2 = glm::vec3(radius * cos(nextTheta), height / 2.0f, radius * sin(nextTheta));

            // ��Բ��������Ե����
            glm::vec3 bottomVertex1 = glm::vec3(radius * cos(theta), -height / 2.0f, radius * sin(theta));
            glm::vec3 bottomVertex2 = glm::vec3(radius * cos(nextTheta), -height / 2.0f, radius * sin(nextTheta));


            glm::vec4 redColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);      // ��ɫ
            glm::vec4 greenColor = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);   // ��ɫ
            //glm::vec4 blueColor = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);     // ��ɫ

            // ��Ӷ�Բ��������
            kPositions[num] = topCenter;
            kColors[num] = redColor;
            num++;
            kPositions[num] = topVertex1;
            kColors[num] = greenColor;
            num++;
            kPositions[num] = topVertex2;
            kColors[num] = greenColor;
            num++;

            // ��ӵ�Բ��������
            kPositions[num] = bottomCenter;
            kColors[num] = redColor;
            num++;
            kPositions[num] = bottomVertex1;
            kColors[num] = greenColor;
            num++;
            kPositions[num] = bottomVertex2;
            kColors[num] = greenColor;
            num++;

            // ���ɲ����ı��Σ����������Σ�
            // ��һ��������
            kPositions[num] = bottomVertex1;
            kColors[num] = greenColor;
            num++;
            kPositions[num] = bottomVertex2;
            kColors[num] = greenColor;
            num++;
            kPositions[num] = topVertex2;
            kColors[num] = greenColor;
            num++;

            // �ڶ���������
            kPositions[num] = topVertex2;
            kColors[num] = greenColor;
            num++;
            kPositions[num] = topVertex1;
            kColors[num] = greenColor;
            num++;
            kPositions[num] = bottomVertex1;
            kColors[num] = greenColor;
            num++;

        }
    };

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
    glm::vec3 kPositions[6000];
    glm::vec4 kColors[6000];
    unsigned int num;
};
