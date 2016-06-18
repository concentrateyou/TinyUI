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
			/// <summary>
			/// 原生窗口
			/// </summary>
			TinyVisualHWND*	GetVisualHWND();
			/// <summary>
			/// 获得兄弟孩子元素
			/// </summary>
			TinyVisual*		GetVisual(TinyVisual* spvis, UINT cmd) const;
			/// <summary>
			/// 设置父元素
			/// </summary>
			TinyVisual*		SetParent(TinyVisual* spvis, TinyVisual* spvisNewParent);
			/// <summary>
			/// 获得父元素
			/// </summary>
			TinyVisual*		GetParent(TinyVisual* spvis) const;
			/// <summary>
			/// 是否孩子元素
			/// </summary>
			BOOL			IsChild(TinyVisual* spvisParent, TinyVisual* spvis) const;
			/// <summary>
			/// 是否Visible
			/// </summary>
			BOOL			IsVisible(TinyVisual* spvis) const;
			/// <summary>
			/// 是否Enable
			/// </summary>
			BOOL			IsEnable(TinyVisual* spvis) const;
			/// <summary>
			/// x,y分别是屏幕坐标
			/// </summary>
			TinyVisual*		GetVisualByPos(INT x, INT y);
			/// <summary>
			/// 获得捕获元素
			/// </summary>
			TinyVisual*		GetCapture() const;
			/// <summary>
			/// 设置捕获元素
			/// </summary>
			TinyVisual*		SetCapture(TinyVisual* spvis);
			/// <summary>
			/// 获得焦点元素
			/// </summary>
			TinyVisual*		GetFocus() const;
			/// <summary>
			/// 设置焦点元素
			/// </summary>
			TinyVisual*		SetFocus(TinyVisual* spvis);
			/// <summary>
			/// 获得元素相对于窗口的坐标
			/// </summary>
			TinyPoint		GetWindowPos(const TinyVisual* spvis);
			/// <summary>
			/// 获得元素相对于屏幕的坐标
			/// </summary>
			TinyPoint		GetScreenPos(const TinyVisual* spvis);
#ifdef _DEBUG
			void			Dump();
			void			Dump(TinyVisual* spvis, INT& deep);
#endif
		public:
			HRESULT			OnMouseMove(const TinyPoint& pos, DWORD dwKey);
			HRESULT			OnLButtonDown(const TinyPoint& pos, DWORD dwKey);
			HRESULT			OnLButtonUp(const TinyPoint& pos, DWORD dwKey);
			HRESULT			OnRButtonDown(const TinyPoint& pos, DWORD dwKey);
			HRESULT			OnRButtonUp(const TinyPoint& pos, DWORD dwKey);
		private:
			void			ConvertToVisualPos(TinyVisual* spvis, TinyPoint& pos);//相对于原生窗口的坐标转换到元素坐标
			TinyVisual*		GetVisualByPos1(TinyVisual* spvis, INT x, INT y);
			TinyVisual*		GetVisualByPos2(TinyVisual* spvis, INT x, INT y);
			void			LinkVisual(TinyVisual* spvis, TinyVisual* spvisInsert, TinyVisual**pspvisFirst);
			void			UnlinkVisual(TinyVisual* spvisUnlink, TinyVisual** pspvisFirst);
			TinyVisual*		GetPrevVisual(TinyVisual* spvisList, TinyVisual* spvisFind) const;
		protected:
			TinyVisualHWND*						m_pWindow;
			TinyVisual*							m_spvisWindow;//根节点
			TinyVisual*							m_spvisCapture;
			TinyVisual*							m_spvisFocus;
			TinyScopedPtr<TinyVisualParse>		m_parse;
			TinyScopedPtr<TinyVisualFactory>	m_fs;
		};
	};
}



