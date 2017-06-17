#pragma once
#include "MShowCommon.h"
#include "Render/TinyRectTracker.h"
#include "DX2D.h"

namespace MShow
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
	/// 可视化组件基类
	/// </summary>
	class MElement : public TinyObject, public TinyRectTracker
	{
		DECLARE_DYNAMIC(MElement)
	public:
		MElement();
		virtual ~MElement();
	public:
		virtual BOOL Draw(DX2D& d2d) = 0;
		virtual BOOL Release() = 0;
	public:
		TinyString	GetName() const;
		TinyPoint	GetPosition() const;
		TinySize	GetScale() const;
		TinySize	GetSize() const;
		void	SetPosition(const TinyPoint& pos);
		void	SetScale(const TinySize& size);
		void	SetSize(const TinySize& size);
		void	SetName(const TinyString& name);
		BOOL	PtInRect(const TinyPoint& pos);
	protected:
		TinySize	m_size;
		TinyString	m_name;
	};
}