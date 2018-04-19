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
		TinyString GetElementName() const;
		TinyPoint GetTrackingPosition() const;
		TinySize GetTrackingSize() const;
		void	SetTrackingPosition(const TinyPoint& pos);
		void	SetTrackingSize(const TinySize& size);
		void	SetElementName(const TinyString& name);
		BOOL	PtInRect(const TinyPoint& pos);
	public:
		virtual BOOL Allocate(DX9& dx9) = 0;
		virtual BOOL Process(DX9& dx9) = 0;
		virtual void Deallocate(DX9& dx9) = 0;
	public:
		TinyString		m_szElement;
	};
}



