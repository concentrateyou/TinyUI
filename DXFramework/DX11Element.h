#pragma once
#include "DXFramework.h"
#include "DX11Texture.h"
#include "DX11TextureShader.h"
#include "Render/TinyRectTracker.h"
using namespace TinyUI;

namespace DXFramework
{
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
	class DX11Element :public TinyUI::TinyObject, public TinyRectTracker
	{
		DECLARE_DYNAMIC(DX11Element)
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
	public:
		virtual BOOL Allocate(DX11& dx11) = 0;
		virtual BOOL Render(DX11& dx11) = 0;
		virtual void Deallocate(DX11& dx11) = 0;
	public:
		TinySize m_size;//原始大小
	};
}



