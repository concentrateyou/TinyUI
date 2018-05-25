#pragma once
#include "GLFramework.h"

namespace GLFramework
{
	class GLCamera
	{
		DISALLOW_COPY_AND_ASSIGN(GLCamera)
	public:
		GLCamera();
		~GLCamera();
		void		SetPosition(FLOAT, FLOAT, FLOAT);
		void		SetRotation(FLOAT, FLOAT, FLOAT);
		void		UpdateView();
		glm::vec3	GetPosition() const;
		glm::vec3	GetRotation() const;
		glm::mat4	GetView() const;
	private:
		glm::vec3	m_position;
		glm::vec3	m_rotation;
		glm::mat4	m_view;
	};
}