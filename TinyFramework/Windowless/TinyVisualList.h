#pragma once
#include "TinyVisual.h"

namespace TinyFramework
{
	namespace Windowless
	{
#define DEFAULT_LIST_ITEM_HEIGHT		100
#define DEFAULT_LIST_ITEM_ROW_SPACE		12
#define DEFAULT_LIST_ITEM_COLUMN_SPACE	12
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
			TinyString	RetrieveTag() const OVERRIDE;
			HRESULT		OnMouseEnter() OVERRIDE;
		public:
			void	SetItemPtr(LPVOID ps);
			LPVOID	GetItemPtr() const;
		protected:
			BOOL	OnDraw(HDC hDC, const RECT& rcPaint) OVERRIDE;
		private:
			LPVOID	m_pointer;//自定义数据
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
			TinyString	RetrieveTag() const OVERRIDE;
			HRESULT		OnMouseEnter() OVERRIDE;
			TinyVisualVScrollBar* GetVScrollBar();
		public:
			void				SetColumnCount(INT count);
			TinyVisualListItem* Add(const TinyString& text);
			TinyVisualListItem* Add(const TinyString& text, const TinyString& imageURL, const TinyString& defaultImage);
			template<class T>
			void RemoveAll(BOOL del, DefaultDeleter<T>& deleter);
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
		template<class T>
		void TinyVisualList::RemoveAll(BOOL del, DefaultDeleter<T>& deleter)
		{
			if (m_document != NULL)
			{
				m_iNewPos = 0;
				if (m_pVScrollbar != NULL)
				{
					m_pVScrollbar->SetVisible(FALSE);
					m_pVScrollbar->SetScrollInfo(0, 0, 0, 0, FALSE);
				}
				TinyVisual* spvis = m_document->GetVisual(this, CMD_CHILD);
				spvis = m_document->GetVisual(spvis, CMD_LAST);
				while (spvis != NULL)
				{
					TinyVisual* spvisT = spvis;
					spvis = m_document->GetVisual(spvis, CMD_PREV);
					if (spvisT->IsKindOf(RUNTIME_CLASS(TinyVisualListItem)))
					{
						TinyVisualListItem* ps = static_cast<TinyVisualListItem*>(spvisT);
						if (del)
						{
							T* pointer = static_cast<T*>(ps->GetItemPtr());
							deleter(pointer);
							ps->SetItemPtr(NULL);
						}
						TinyVisualResource::GetInstance().Remove(ps->GetBackgroundImage());
						m_document->Destory(spvisT);
					}
				}
				Invalidate();
			}
		}
	}
}



