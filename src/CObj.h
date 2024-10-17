#include <vector>
#include <cmath>
#include <glm/glm.hpp>

struct Point
{
	glm::vec3 pos;
	glm::vec3 normal;
};

struct Face
{
	int pts[3];
	glm::vec3 normal;
};

class CObj
{
public:
	CObj(void);
	~CObj(void);

	std::vector<Point> m_pts; //����
	std::vector<Face> m_faces;//��
	std::vector<unsigned int> indices;//����

public:
	bool ReadObjFile(const char* pcszFileName);//����ģ���ļ�

private:
	void UnifyModel();//��λ��ģ��
	void ComputeFaceNormal(Face& f);//������ķ���
	void ComputePointNormal();//���㶥�㷨��
};

