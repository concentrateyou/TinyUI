#pragma once
#include "DXFramework.h"
#include "DX9Texture2D.h"
#include "Render/TinyRectTracker.h"
using namespace TinyUI;

namespace DXFramework
{
	/// <summary>
	/// DX9ÔªËØ
	/// </summary>
	class DX9Element2D : public TinyObject, public TinyRectTracker
	{
		DECLARE_DYNAMIC(DX9Element2D)
	public:
		DX9Element2D();
		virtual ~DX9Element2D();
	public:
		TinyString GetName() const;
		TinyPoint GetPosition() const;
		TinySize GetScale() const;
		TinySize GetSize() const;
		void	SetPosition(const TinyPoint& pos);
		void	SetScale(const TinySize& size);
		void	SetSize(const TinySize& size);
		void	SetName(const TinyString& name);
		BOOL	PtInRect(const TinyPoint& pos);
	public:
		virtual BOOL Allocate(DX9& dx9) = 0;
		virtual BOOL Process(DX9& dx9) = 0;
		virtual void Deallocate(DX9& dx9) = 0;
	public:
		TinyString		m_name;
		TinySize		m_size;
	};
}



