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
	XMFLOAT3 DXCamera::GetPosition()
	{
		return XMFLOAT3(m_positionX, m_positionY, m_positionZ);
	}
	XMFLOAT3 DXCamera::GetRotation()
	{
		return XMFLOAT3(m_rotationX, m_rotationY, m_rotationZ);
	}
	void DXCamera::UpdatePosition()
	{
		XMFLOAT3 up;
		XMFLOAT3 position;
		XMFLOAT3 lookAt;
		FLOAT yaw;
		FLOAT pitch;
		FLOAT roll;
		XMVECTOR upVector;
		XMVECTOR positionVector;
		XMVECTOR lookAtVector;
		XMMATRIX rotationMatrix;
		up.x = 0.0F;
		up.y = 1.0F;
		up.z = 0.0F;
		upVector = XMLoadFloat3(&up);
		position.x = m_positionX;
		position.y = m_positionY;
		position.z = m_positionZ;
		positionVector = XMLoadFloat3(&position);
		lookAt.x = 0.0F;
		lookAt.y = 0.0F;
		lookAt.z = 1.0F;
		lookAtVector = XMLoadFloat3(&lookAt);
		pitch = m_rotationX * 0.0174532925F;
		yaw = m_rotationY * 0.0174532925F;
		roll = m_rotationZ * 0.0174532925F;
		rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
		lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
		upVector = XMVector3TransformCoord(upVector, rotationMatrix);
		lookAtVector = XMVectorAdd(positionVector, lookAtVector);
		m_viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
	}
	XMMATRIX DXCamera::GetViewMatrix()
	{
		return m_viewMatrix;
	}
}