#pragma once
#include <glm/glm.hpp>

class Camera
{
public:
	void Init(float aspectRatio);
private:
	glm::mat4 m_view;
	glm::mat4 m_proj;
};

