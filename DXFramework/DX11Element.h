#pragma once
#include "DXFramework.h"
#include "DX11Texture.h"
#include "DX11TextureShader.h"

namespace DXFramework
{
	enum ElementType
	{
		IMAGE,
		FONT
	};
	/// <summary>
	/// DX11Ԫ��
	/// </summary>
	class DX11Element
	{
	public:
		DX11Element();
		~DX11Element();
	public:
		TinyPoint GetPosition() const;
		TinySize GetScale() const;
		TinySize GetSize() const;
		void SetPosition(const TinyPoint& pos);
		void SetScale(const TinySize& size);
		virtual INT	GetIndexCount() const = 0;
		virtual LPCSTR GetClassName() = 0;
		virtual ElementType	 GetElementType() const = 0;
		virtual BOOL BeginScene() = 0;
		virtual void EndScene() = 0;
		virtual BOOL Render(const DX11& dx11) = 0;
	protected:
		TinySize	m_size;//ԭʼ��С
		TinySize	m_scale;//���Ŵ�С
		TinyPoint	m_position;//λ������
	};
}



