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
		void		UpdatePosition();
		XMFLOAT3	GetPosition();
		XMFLOAT3	GetRotation();
		XMMATRIX	GetView();
	private:
		FLOAT		m_positionX;
		FLOAT		m_positionY;
		FLOAT		m_positionZ;
		FLOAT		m_rotationX;
		FLOAT		m_rotationY;
		FLOAT		m_rotationZ;
		XMMATRIX	m_viewMatrix;
	};
}


