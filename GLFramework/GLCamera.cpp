#include "stdafx.h"
#include "GLCamera.h"

namespace GLFramework
{
	GLCamera::GLCamera()
	{
		m_position.x = m_position.y = m_position.z = 0.0F;
		m_rotation.x = m_rotation.y = m_rotation.z = 0.0F;
	}

	GLCamera::~GLCamera()
	{

	}


	void GLCamera::SetPosition(FLOAT x, FLOAT y, FLOAT z)
	{
		m_position.x = x;
		m_position.y = y;
		m_position.z = z;
	}

	void GLCamera::SetRotation(FLOAT x, FLOAT y, FLOAT z)
	{
		m_rotation.x = x;
		m_rotation.y = y;
		m_rotation.z = z;
	}


	void GLCamera::UpdateView()
	{
		glm::vec3 eye;
		glm::vec3 center;
		glm::vec3 up(0.0F, 1.0F, 0.0F);
		m_view = glm::lookAt(eye, center, up);
	}
	glm::mat4 GLCamera::GetView() const
	{
		return m_view;
	}

	glm::vec3 GLCamera::GetPosition() const
	{
		return m_position;
	}
	glm::vec3 GLCamera::GetRotation() const
	{
		return m_rotation;
	}
}