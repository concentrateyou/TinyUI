#pragma once
#include "TinyVisual.h"

namespace TinyUI
{
	namespace Windowless
	{
#define DEFAULT_LIST_ITEM_HEIGHT		120
#define DEFAULT_LIST_ITEM_ROW_SPACE		10
#define DEFAULT_LIST_ITEM_COLUMN_SPACE	10
		class TinyVisualVScrollBar;
		class TinyVisualList;
		/// <summary>
		/// 列表项
		/// </summary>
		class TinyVisualListItem : public TinyVisual
		{
			friend class TinyVisualList;
			DECLARE_DYNCREATE(TinyVisualListItem)
			DISALLOW_COPY_AND_ASSIGN(TinyVisualListItem)
		protected:
			TinyVisualListItem();
			TinyVisualListItem(TinyVisual* spvisParent, TinyVisualDocument* vtree);
		public:
			virtual ~TinyVisualListItem();
			TinyString RetrieveTag() const OVERRIDE;
			HRESULT	OnMouseEnter() OVERRIDE;
		protected:
			BOOL OnDraw(HDC hDC, const RECT& rcPaint) OVERRIDE;
		};
		/// <summary>
		/// 列表控件
		/// </summary>
		class TinyVisualList : public TinyVisual
		{
			friend class TinyVisualDocument;
			DECLARE_DYNCREATE(TinyVisualList)
			DISALLOW_COPY_AND_ASSIGN(TinyVisualList)
		protected:
			TinyVisualList();
			TinyVisualList(TinyVisual* spvisParent, TinyVisualDocument* vtree);
			HRESULT OnInitialize() OVERRIDE;
		public:
			virtual ~TinyVisualList();
			TinyString RetrieveTag() const OVERRIDE;
			HRESULT	OnMouseEnter() OVERRIDE;
			TinyVisualVScrollBar* GetVScrollBar();
		public:
			void SetColumnCount(INT count);
			BOOL Add(const TinyString& text, const TinyString& imagrURL);
		protected:
			BOOL OnDraw(HDC hDC, const RECT& rcPaint) OVERRIDE;
		private:
			void OnPosChange(BOOL bVer, INT code, INT iOldPos, INT iNewPos);
			void AdjustLayout(INT dx, INT dy);
		private:
			INT						m_iNewPos;
			INT						m_count;
			TinyVisualVScrollBar*	m_pVScrollbar;
			TinyScopedPtr<Delegate<void(BOOL, INT, INT, INT)>> m_onPosChange;
		};
	}
}



