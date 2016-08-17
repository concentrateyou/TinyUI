#pragma once
#include <D3D10_1.h>
#include <D3DX10.h>
#include <DXGI.h>
#include <string>
#include "Common/TinyCommon.h"
#include "Control/TinyControl.h"
#include "Render/TinyGDI.h"
using namespace TinyUI;

namespace DXFramework
{
	class D3DCamera
	{
		DISALLOW_COPY_AND_ASSIGN(D3DCamera);
	public:
		D3DCamera();
		~D3DCamera();
		void SetPosition(FLOAT, FLOAT, FLOAT);
		void SetRotation(FLOAT, FLOAT, FLOAT);
		D3DXVECTOR3 GetPosition();
		D3DXVECTOR3 GetRotation();
		void Render();
		void GetViewMatrix(D3DXMATRIX&);
	private:
		FLOAT		m_positionX;
		FLOAT		m_positionY;
		FLOAT		m_positionZ;
		FLOAT		m_rotationX;
		FLOAT		m_rotationY;
		FLOAT		m_rotationZ;
		D3DXMATRIX	m_viewMatrix;
	};
}
