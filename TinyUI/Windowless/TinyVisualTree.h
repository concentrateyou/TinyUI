#pragma once
#include "TinyVisualManage.h"

namespace TinyUI
{
	namespace Windowless
	{
#define PVISUAL_TOP        (TinyVisual*)0
#define PVISUAL_BOTTOM     (TinyVisual*)1	
#define CMD_FIRST        0  //第一个兄弟节点
#define CMD_LAST         1  //最后一个兄弟节点
#define CMD_NEXT         2  //下一个兄弟节点
#define CMD_PREV         3  //上一个兄弟节点
#define CMD_OWNER        4  //拥有者节点
#define CMD_CHILD        5  //第一个孩子节点
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
			friend class TinyVisualFactory;
			DISALLOW_COPY_AND_ASSIGN(TinyVisualTree);
		public:
			TinyVisualTree(TinyVisualHWND* pv);
			~TinyVisualTree();
		public:
			BOOL			Initialize();
			void			Uninitialize();
		public:
			TinyVisualHWND*	GetVisualHWND();
			TinyVisual*		GetVisual(TinyVisual* spvis, UINT cmd) const;
			TinyVisual*		SetParent(TinyVisual* spvis, TinyVisual* spvisNewParent);
			TinyVisual*		GetParent(TinyVisual* spvis) const;
			BOOL			IsChild(TinyVisual* spvisParent, TinyVisual* spvis) const;
			BOOL			IsVisible(TinyVisual* spvis) const;
			BOOL			IsEnable(TinyVisual* spvis) const;
			TinyVisual*		GetVisualByName(const TinyString& name);
			TinyVisual*		GetVisualByPos(INT x, INT y);
			TinyVisual*		GetCapture() const;
			TinyVisual*		SetCapture(TinyVisual* spvis);
			TinyVisual*		GetFocus() const;
			TinyVisual*		SetFocus(TinyVisual* spvis);
			TinyPoint		GetWindowPos(const TinyVisual* spvis);
			TinyRectangle	GetWindowRect(const TinyVisual* spvis);
			TinyPoint		GetScreenPos(const TinyVisual* spvis);
			void			Render(TinyVisual* spvis, HDC hDC);
		public:
			HRESULT			OnMouseMove(TinyPoint pos, DWORD dwFlags);
			HRESULT			OnLButtonDown(TinyPoint pos, DWORD dwFlags);
			HRESULT			OnLButtonUp(TinyPoint pos, DWORD dwFlags);
			HRESULT			OnRButtonDown(TinyPoint pos, DWORD dwFlags);
			HRESULT			OnRButtonUp(TinyPoint pos, DWORD dwFlags);
			HRESULT			OnMButtonDown(TinyPoint pos, DWORD dwFlags);
			HRESULT			OnMButtonUp(TinyPoint pos, DWORD dwFlags);
		private:
			void			ConvertToVisualPos(TinyVisual* spvis, TinyPoint& pos);//相对于原生窗口的坐标转换到元素坐标
			TinyVisual*		GetVisualByName1(TinyVisual* spvis, const TinyString& name);
			TinyVisual*		GetVisualByName2(TinyVisual* spvis, const TinyString& name);
			TinyVisual*		GetVisualByPos1(TinyVisual* spvis, INT x, INT y);
			TinyVisual*		GetVisualByPos2(TinyVisual* spvis, INT x, INT y);
			void			LinkVisual(TinyVisual* spvis, TinyVisual* spvisInsert, TinyVisual**pspvisFirst);
			void			UnlinkVisual(TinyVisual* spvisUnlink, TinyVisual** pspvisFirst);
			TinyVisual*		GetPrevVisual(TinyVisual* spvisList, TinyVisual* spvisFind) const;
			void			onClick();
		protected:
			TinyVisual*									m_spvisWindow;//根节点
			TinyVisual*									m_spvisCapture;
			TinyVisual*									m_spvisFocus;
			TinyVisualHWND*								m_pWindow;
			TinyScopedPtr<TinyVisualParse>				m_parse;
			TinyScopedPtr<TinyVisualFactory>			m_fs;
		private:
			TinyScopedPtr<Delegate<void(void)>>			m_click;
		public:
#ifdef _DEBUG
			void			Dump();
			void			Dump(TinyVisual* spvis, INT& deep);
#endif
		};
	};
}



