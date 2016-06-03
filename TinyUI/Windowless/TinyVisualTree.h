#pragma once
#include "TinyVisualManage.h"

namespace TinyUI
{
	namespace Windowless
	{
#define PVISUAL_TOP        (TinyVisual*)0
#define PVISUAL_BOTTOM     (TinyVisual*)1	
#define CMD_FIRST        0
#define CMD_LAST         1
#define CMD_NEXT         2
#define CMD_PREV         3
#define CMD_OWNER        4
#define CMD_CHILD        5

#define NOSIZE          0x0001
#define NOMOVE          0x0002
#define NOZORDER        0x0004
#define NOREDRAW        
		class TinyVisual;
		class TinyVisualHWND;
		/// <summary>
		/// 可视化树
		/// </summary>
		class TinyVisualTree
		{
			friend class TinyVisualParse;
			DISALLOW_COPY_AND_ASSIGN(TinyVisualTree);
		public:
			TinyVisualTree(TinyVisualHWND* pv);
			~TinyVisualTree();
		public:
			BOOL		Initialize();
			void		Uninitialize();
			TinyVisualHWND*	GetVisualHWND();
			TinyVisual*	GetVisual(TinyVisual* spvis, UINT cmd) const;
			TinyVisual*	SetParent(TinyVisual* spvis, TinyVisual* spvisParent);
			TinyVisual*	GetParent(TinyVisual* spvis) const;
			BOOL		IsChild(TinyVisual* spvisParent, TinyVisual* spvis) const;
			BOOL		IsVisible(TinyVisual* spvis) const;
			BOOL		SetVisualPos(TinyVisual* spvis, INT  x, INT  y, INT  cx, INT  cy, DWORD dwFlags);
			TinyVisual*	GetVisualByPos(INT x, INT y);
			TinyVisual*	GetCapture() const;
			TinyVisual*	SetCapture(TinyVisual* pNew);
			TinyVisual*	GetFocus() const;
			TinyVisual*	SetFocus(TinyVisual* pNew);
			void		LinkVisual(TinyVisual* spvis, TinyVisual* spvisInsert, TinyVisual**pspvisFirst);
			void		UnlinkVisual(TinyVisual* spvisUnlink, TinyVisual** pspvisFirst);
#ifdef _DEBUG
			void		Dump();
			void		Dump(TinyVisual* spvis);
#endif // _DEBUG
		public:
			HRESULT		OnMouseMove(POINT pos);
			HRESULT     OnLButtonDown(POINT pos);
			HRESULT     OnLButtonUp(POINT pos);
			HRESULT     OnRButtonDown(POINT pos);
			HRESULT     OnRButtonUp(POINT pos);
		private:
			TinyVisual*	GetVisualByPos1(TinyVisual* spvis, INT x, INT y);
			TinyVisual*	GetVisualByPos2(TinyVisual* spvis, INT x, INT y);
			TinyVisual* GetPrevVisual(TinyVisual* spvisList, TinyVisual* spvisFind) const;
		private:
			TinyVisualHWND*					m_pWindow;
			TinyVisual*						m_spvisWindow;//根节点
			TinyVisual*						m_spvisCapture;
			TinyVisual*						m_spvisFocus;
			TinyScopedPtr<TinyVisualParse>	m_parse;
		};
	};
}



