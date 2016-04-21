#pragma once
#include "../Common/TinyString.h"
#include "../Common/TinyEvent.h"
#include "../Render/TinyGDI.h"
#include "../XML//tinyxml.h"
#include "TinyUtility.h"
namespace TinyUI
{
	namespace Windowless
	{
		/// <summary>
		/// 可视化元素基类
		/// </summary>
		class TinyVisual : public TinyObject
		{
			friend class TinyVisualTree;
			friend class TinyVisualParse;
			DECLARE_DYNAMIC(TinyVisual)
			DISALLOW_COPY_AND_ASSIGN(TinyVisual)
		public:
			TinyVisual(TinyVisual* spvisParent);
		public:
			void		SetText(LPCSTR pzText);
			TinyString	GetText() const;
			void		SetName(LPCSTR pzName);
			TinyString	GetName() const;
			void		SetToolTip(LPCSTR pzTitle);
			TinyString	GetToolTip() const;
			TinyPoint	GetPosition() const;
			TinySize	GetSize() const;
			void		SetSize(const TinySize& size);
			void		SetMaximumSize(const TinySize& size);
			void		SetMinimumSize(const TinySize& size);
			TinySize	GetMaximumSize() const;
			TinySize	GetMinimumSize() const;
			BOOL		IsVisible() const;
			BOOL		IsEnable() const;
			void		SetVisible(BOOL visible);
			void		SetEnable(BOOL enable);
		public:
			virtual void OnSizeChange(const TinySize&, const TinySize&);
		public:
			virtual ~TinyVisual();
			virtual LPCSTR	RetrieveTag() = 0;
			virtual HRESULT	OnDraw(TinyDC& dc, TinyRectangle& drawRect) = 0;
			virtual HRESULT OnMouseMove(POINT pos) = 0;
			virtual HRESULT OnLButtonDown(POINT pos) = 0;
			virtual HRESULT OnLButtonUp(POINT pos) = 0;
			virtual HRESULT OnRButtonDown(POINT pos) = 0;
			virtual HRESULT OnRButtonUp(POINT pos) = 0;
		protected:
			TinyVisual*		m_spvisNext;//同级下一个兄弟节点
			TinyVisual*		m_spvisParent;//父节点
			TinyVisual*		m_spvisChild;//第一个孩子节点
			TinyVisual*		m_spvisOwner;//对于Popup窗口使用
			TinyRectangle   m_windowRect;//屏幕区域(屏幕坐标)
			TinySize		m_maximumSize;//元素的最大像素大小
			TinySize		m_minimumSize;//元素的最小像素大小
			TinyString		m_strName;
			TinyString		m_strText;
			TinyString		m_strToolTip;
			HRGN			m_hrgnClip;
			BOOL			m_visible;
			BOOL			m_enable;
		};
	}
}



