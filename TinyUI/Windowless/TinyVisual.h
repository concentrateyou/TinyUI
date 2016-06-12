#pragma once
#include "../Common/TinyString.h"
#include "../Common/TinyEvent.h"
#include "../Render/TinyCanvas.h"
#include "../XML//tinyxml.h"
#include "TinyVisualLayout.h"
#include "TinyUtility.h"
namespace TinyUI
{
	namespace Windowless
	{
		enum StyleImage
		{
			NORMAL = 0,
			HIGHLIGHT = 1,
			DOWN = 2
		};
		/// <summary>
		/// 可视化元素基类
		/// </summary>
		class TinyVisual : public TinyObject
		{
			friend class TinyVisualTree;
			friend class TinyVisualParse;
			friend class TinyVisualFactory;
			DECLARE_DYNAMIC(TinyVisual);
			DISALLOW_COPY_AND_ASSIGN(TinyVisual);
		protected:
			TinyVisual(TinyVisual* spvisParent, TinyVisualTree* vtree);
		public:
			void				SetText(LPCSTR pzText);
			TinyString			GetText() const;
			void				SetName(LPCSTR pzName);
			TinyString			GetName() const;
			void				SetToolTip(LPCSTR pzTitle);
			TinyString			GetToolTip() const;
			TinyPoint			GetPosition() const;
			void				SetPosition(const TinyPoint& pos);
			TinySize			GetSize() const;
			void				SetSize(const TinySize& size);
			void				SetMaximumSize(const TinySize& size);
			void				SetMinimumSize(const TinySize& size);
			TinySize			GetMaximumSize() const;
			TinySize			GetMinimumSize() const;
			BOOL				IsVisible() const;
			BOOL				IsEnable() const;
			void				SetVisible(BOOL visible);
			void				SetEnable(BOOL enable);
			void				SetClip(HRGN hrgnClip);
			BOOL				SetStyleImage(StyleImage type, LPCSTR pzFile);
			BOOL				SetStyleImage(StyleImage type, BYTE*	ps, DWORD dwSize);
		public:
			virtual void		Resize();
			virtual void		OnSizeChange(const TinySize&, const TinySize&);
		public:
			virtual ~TinyVisual();
			virtual TinyString	RetrieveTag() const = 0;
			virtual HRESULT		OnDraw(TinyCanvas& canvas) = 0;
			virtual HRESULT		OnMouseMove(POINT pos) = 0;
			virtual HRESULT		OnLButtonDown(POINT pos) = 0;
			virtual HRESULT		OnLButtonUp(POINT pos) = 0;
			virtual HRESULT		OnRButtonDown(POINT pos) = 0;
			virtual HRESULT		OnRButtonUp(POINT pos) = 0;
		protected:
			TinyImage			m_images[3];
			TinyVisual*			m_spvisNext;//同级下一个兄弟节点
			TinyVisual*			m_spvisParent;//父节点
			TinyVisual*			m_spvisChild;//第一个孩子节点
			TinyVisual*			m_spvisOwner;//对于Popup窗口使用
			TinyVisualTree*		m_vtree;
			TinyRectangle		m_windowRect;//屏幕区域(屏幕坐标)
			TinyString			m_strName;
			TinyString			m_strText;
			TinyString			m_strToolTip;
			TinySize			m_maximumSize;//元素的最大像素大小
			TinySize			m_minimumSize;//元素的最小像素大小
			HRGN				m_hrgnClip;
			BOOL				m_visible;
			BOOL				m_enable;
		};
	}
}



