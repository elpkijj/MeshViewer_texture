#pragma once

#include <glm/glm.hpp>

class Cone
{
public:
    Cone(float radius = 1.0f, float height = 2.0f, unsigned int numSlices = 100)
    {
        //TODO:计算顶点和颜色和数量
        num = 0; // 初始化顶点数量

        float angle = 2.0f * glm::pi<float>() / numSlices;

        // 底面中心点
        glm::vec3 bottomCenter = glm::vec3(0.0f, -height / 2.0f, 0.0f);
        glm::vec4 bottomColor = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f); // 蓝色
        glm::vec4 tipColor = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f); // 顶点绿色

        // 生成底面顶点和三角形扇形
        for (unsigned int i = 0; i < numSlices; ++i)
        {
            float theta = i * angle;
            float nextTheta = (i + 1) * angle;

            // 当前和下一个点
            glm::vec3 currentVertex = glm::vec3(radius * cos(theta), -height / 2.0f, radius * sin(theta));
            glm::vec3 nextVertex = glm::vec3(radius * cos(nextTheta), -height / 2.0f, radius * sin(nextTheta));

            // 三角形扇形：中心点、当前点、下一个点
            kPositions[num] = bottomCenter;
            kColors[num] = tipColor;
            num++;

            kPositions[num] = currentVertex;
            kColors[num] = bottomColor;
            num++;

            kPositions[num] = nextVertex;
            kColors[num] = bottomColor;
            num++;
        }

        // 圆锥顶点（尖端）
        glm::vec3 tip = glm::vec3(0.0f, height / 2.0f, 0.0f);


        // 生成侧面，连接底面点与尖端
        for (unsigned int i = 0; i < numSlices; ++i)
        {
            float theta = i * angle;
            float nextTheta = (i + 1) * angle;

            // 当前和下一个底面点
            glm::vec3 currentVertex = glm::vec3(radius * cos(theta), -height / 2.0f, radius * sin(theta));
            glm::vec3 nextVertex = glm::vec3(radius * cos(nextTheta), -height / 2.0f, radius * sin(nextTheta));

            // 生成三角形：尖端、当前点、下一个点
            kPositions[num] = tip;
            kColors[num] = tipColor;
            num++;

            kPositions[num] = currentVertex;
            kColors[num] = bottomColor;
            num++;

            kPositions[num] = nextVertex;
            kColors[num] = bottomColor;
            num++;
        }
    }

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
