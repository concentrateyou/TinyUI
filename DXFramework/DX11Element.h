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
	enum TrackerHit
	{
		hitNothing = -1,
		hitTopLeft = 0,
		hitTopRight = 1,
		hitBottomRight = 2,
		hitBottomLeft = 3,
		hitTop = 4,
		hitRight = 5,
		hitBottom = 6,
		hitLeft = 7,
		hitMiddle = 8
	};
	/// <summary>
	/// DX11元素
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
		void SetSize(const TinySize& size);
		virtual INT	GetIndexCount() const = 0;
		virtual LPCSTR GetClassName() = 0;
		virtual ElementType	 GetElementType() const = 0;
		virtual BOOL BeginScene() = 0;
		virtual void EndScene() = 0;
		virtual BOOL Render(const DX11& dx11) = 0;
	protected:
		TinySize		m_size;//原始大小
		TinyRectangle	m_rectangle;//显示大小
	};
}



