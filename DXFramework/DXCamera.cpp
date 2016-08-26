#include "stdafx.h"
#include "DXCamera.h"

namespace DXFramework
{
	DXCamera::DXCamera()
	{
		m_positionX = 0.0F;
		m_positionY = 0.0F;
		m_positionZ = 0.0F;
		m_rotationX = 0.0F;
		m_rotationY = 0.0F;
		m_rotationZ = 0.0F;
	}
	DXCamera::~DXCamera()
	{
	}
	void DXCamera::SetPosition(FLOAT x, FLOAT y, FLOAT z)
	{
		m_positionX = x;
		m_positionY = y;
		m_positionZ = z;
	}
	void DXCamera::SetRotation(FLOAT x, FLOAT y, FLOAT z)
	{
		m_rotationX = x;
		m_rotationY = y;
		m_rotationZ = z;
	}
	D3DXVECTOR3 DXCamera::GetPosition()
	{
		return D3DXVECTOR3(m_positionX, m_positionY, m_positionZ);
	}
	D3DXVECTOR3 DXCamera::GetRotation()
	{
		return D3DXVECTOR3(m_rotationX, m_rotationY, m_rotationZ);
	}
	void DXCamera::UpdatePosition()
	{
		D3DXVECTOR3 up;
		D3DXVECTOR3 position;
		D3DXVECTOR3 lookAt;
		FLOAT yaw;
		FLOAT pitch;
		FLOAT roll;
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
	D3DXMATRIX DXCamera::GetViewMatrix()
	{
		return m_viewMatrix;
	}
}