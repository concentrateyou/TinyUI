#pragma once
#include "DXFramework.h"

namespace DXFramework
{
	class DXCamera
	{
	public:
		DXCamera();
		~DXCamera();
		void SetPosition(FLOAT, FLOAT, FLOAT);
		void SetRotation(FLOAT, FLOAT, FLOAT);
		D3DXVECTOR3 GetPosition();
		D3DXVECTOR3 GetRotation();
		void Update();
		D3DXMATRIX GetViewMatrix();
	private:
		FLOAT m_positionX;
		FLOAT m_positionY;
		FLOAT m_positionZ;
		FLOAT m_rotationX;
		FLOAT m_rotationY;
		FLOAT m_rotationZ;
		D3DXMATRIX m_viewMatrix;
	};
}