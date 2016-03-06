#pragma once
#include "../Common/TinyString.h"
#include "../Common/TinyEvent.h"
#include "../Render/TinyGDI.h"
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
			DECLARE_DYNAMIC(TinyVisual)
		public:
			TinyVisual();
			virtual ~TinyVisual();
			void			SetMaximumSize(const TinySize& size);
			void			SetMinimumSize(const TinySize& size);
			TinySize		GetMaximumSize() const;
			TinySize		GetMinimumSize() const;
			virtual HRESULT	OnDraw(TinyDC& dc, TinyRectangle& drawRect);
		private:
			TinyVisual*		m_spvisPrev;//同级上一个兄弟节点
			TinyVisual*		m_spvisNext;//同级下一个兄弟节点
			TinyVisual*		m_spvisParent;//父节点
			TinyVisual*		m_spvisChild;//孩子节点
			TinyVisual*		m_spvisOwner;//对于Popup窗口使用
			TinyPoint		m_pos;//元素相对窗口的坐标
			TinySize		m_size;//元素像素大小
			TinySize		m_maximumSize;//元素的最大像素大小
			TinySize		m_minimumSize;//元素的最小像素大小
		};
	}
}



