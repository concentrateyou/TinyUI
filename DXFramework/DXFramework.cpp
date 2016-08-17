#include "stdafx.h"
#include "DXFramework.h"
#include "Control/TinyControl.h"

namespace DXFramework
{
	D3DCamera::D3DCamera()
	{
		m_positionX = 0.0F;
		m_positionY = 0.0F;
		m_positionZ = 0.0F;
		m_rotationX = 0.0F;
		m_rotationY = 0.0F;
		m_rotationZ = 0.0F;
	}

	D3DCamera::~D3DCamera()
	{
	}


	void D3DCamera::SetPosition(FLOAT x, FLOAT y, FLOAT z)
	{
		m_positionX = x;
		m_positionY = y;
		m_positionZ = z;
	}


	void D3DCamera::SetRotation(FLOAT x, FLOAT y, FLOAT z)
	{
		m_rotationX = x;
		m_rotationY = y;
		m_rotationZ = z;
	}


	D3DXVECTOR3 D3DCamera::GetPosition()
	{
		return D3DXVECTOR3(m_positionX, m_positionY, m_positionZ);
	}


	D3DXVECTOR3 D3DCamera::GetRotation()
	{
		return D3DXVECTOR3(m_rotationX, m_rotationY, m_rotationZ);
	}


	void D3DCamera::Render()
	{
		D3DXVECTOR3 up, position, lookAt;
		float yaw, pitch, roll;
		D3DXMATRIX rotationMatrix;
		up.x = 0.0F;
		up.y = 1.0F;
		up.z = 0.0F;
		position.x = m_positionX;
		position.y = m_positionY;
		position.z = m_positionZ;
		lookAt.x = 0.0F;
		lookAt.y = 0.0F;
		lookAt.z = 1.0F;
		pitch = m_rotationX * 0.0174532925F;
		yaw = m_rotationY * 0.0174532925F;
		roll = m_rotationZ * 0.0174532925F;
		D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, pitch, roll);
		D3DXVec3TransformCoord(&lookAt, &lookAt, &rotationMatrix);
		D3DXVec3TransformCoord(&up, &up, &rotationMatrix);
		lookAt = position + lookAt;
		D3DXMatrixLookAtLH(&m_viewMatrix, &position, &lookAt, &up);
	}


	void D3DCamera::GetViewMatrix(D3DXMATRIX& viewMatrix)
	{
		viewMatrix = m_viewMatrix;
	}
}