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
		class TinyVisualFrame;
		/// <summary>
		/// 可视化文档
		/// </summary>
		class TinyVisualDocument
		{
			friend class TinyVisualBuilder;
			friend class TinyVisualFactory;
			friend class TinyVisualFrame;
			DISALLOW_COPY_AND_ASSIGN(TinyVisualDocument)
		public:
			TinyVisualDocument(TinyVisualFrame* pv);
			virtual ~TinyVisualDocument();
			virtual BOOL	Initialize(TinyVisualBuilder* builder);
			virtual void	Uninitialize();
		public:
			TinyVisual*		Create(INT x, INT y, INT cx, INT cy, const TinyString& tag, TinyVisual* spvisParent);
			TinyVisual*		Create(const TinyString& tag, TinyVisual* spvisParent);
			BOOL			Destory(TinyVisual* spvis);
		public:
			HWND				Handle() const;
			TinyVisualFrame*	GetVisualHWND() const;
			TinyVisualDC*		GetVisualDC() const;
			TinyVisual*			GetVisual(TinyVisual* spvis, UINT cmd) const;
			TinyVisual*			SetParent(TinyVisual* spvis, TinyVisual* spvisNewParent);
			TinyVisual*			GetParent(TinyVisual* spvis) const;
			BOOL				IsChild(TinyVisual* spvisParent, TinyVisual* spvis) const;
			BOOL				IsVisible(TinyVisual* spvis) const;
			BOOL				IsEnable(TinyVisual* spvis) const;
			BOOL				IsActive(TinyVisual* spvis) const;
			TinyVisual*			GetVisualByName(const TinyString& name);
			TinyVisual*			GetVisualByPos(INT x, INT y);
			TinyVisual*			GetCapture() const;
			TinyVisual*			SetCapture(TinyVisual* spvis);
			BOOL				ReleaseCapture();
			TinyVisual*			GetFocus() const;
			TinyVisual*			SetFocus(TinyVisual* spvis);
			TinyVisual*			SetActive(TinyVisual* spvis);
			TinyVisual*			GetActive() const;
			TinyPoint			VisualToClient(const TinyVisual* spvis, const TinyPoint& pos);
			TinyPoint			GetWindowPos(const TinyVisual* spvis);
			TinyRectangle		GetWindowRect(const TinyVisual* spvis);
			TinyPoint			GetScreenPos(const TinyVisual* spvis);
			BOOL				Invalidate(RECT* lpRect = NULL);
			BOOL				Redraw(RECT *lprcUpdate = NULL, HRGN hrgnUpdate = NULL);
		public:
			HRESULT				OnSize(const TinySize& size);
			HRESULT				OnMouseLeave();
			HRESULT				OnMouseMove(const TinyPoint& pos, DWORD dwFlags);
			HRESULT				OnMouseWheel(const TinyPoint& pos, SHORT zDelta, DWORD dwFlags);
			HRESULT				OnLButtonDown(const TinyPoint& pos, DWORD dwFlags);
			HRESULT				OnLButtonUp(const TinyPoint& pos, DWORD dwFlags);
			HRESULT				OnLButtonDBClick(const TinyPoint& pos, DWORD dwFlags);
			HRESULT				OnRButtonDown(const TinyPoint& pos, DWORD dwFlags);
			HRESULT				OnRButtonUp(const TinyPoint& pos, DWORD dwFlags);
			HRESULT				OnRButtonDBClick(const TinyPoint& pos, DWORD dwFlags);
			HRESULT				OnMButtonDown(const TinyPoint& pos, DWORD dwFlags);
			HRESULT				OnMButtonUp(const TinyPoint& pos, DWORD dwFlags);
			HRESULT				OnMButtonDBClick(const TinyPoint& pos, DWORD dwFlags);
			HRESULT				OnKeyDown(DWORD dwChar, DWORD dwRepCnt, DWORD dwFlags);
			HRESULT				OnKeyUp(DWORD dwChar, DWORD dwRepCnt, DWORD dwFlags);
			LRESULT				OnChar(DWORD dwChar, DWORD dwRepCnt, DWORD dwFlags);
			HRESULT				OnSetCursor(HWND hWND, DWORD dwHitTest, DWORD dwMessage);
			HRESULT				OnSetFocus(HWND hWND);
			HRESULT				OnKillFocus(HWND hWND);
		private:
			void				ConvertToClientPos(const TinyVisual* spvis, TinyPoint& pos);//把元素坐标转化为窗口坐标
			void				ConvertToVisualPos(const TinyVisual* spvis, TinyPoint& pos);//相对于原生窗口的坐标转换到元素坐标
			TinyVisual*			GetVisualByName1(TinyVisual* spvis, const TinyString& name);
			TinyVisual*			GetVisualByName2(TinyVisual* spvis, const TinyString& name);
			TinyVisual*			GetVisualByPos1(TinyVisual* spvis, INT x, INT y);
			TinyVisual*			GetVisualByPos2(TinyVisual* spvis, INT x, INT y);
			void				LinkVisual(TinyVisual* spvis, TinyVisual* spvisInsert, TinyVisual**pspvisFirst);
			void				UnlinkVisual(TinyVisual* spvisUnlink, TinyVisual** pspvisFirst);
			TinyVisual*			GetPrevVisual(TinyVisual* spvisList, TinyVisual* spvisFind) const;
			void				Draw(TinyVisualDC* ps, const RECT& rcPaint);
			void				Draw(TinyVisual* spvis, HDC hDC, const RECT& rcPaint);
		private:
			class TinyVisualFactory
			{
				friend class TinyVisual;
				friend class TinyVisualDocument;
				DISALLOW_COPY_AND_ASSIGN(TinyVisualFactory)
			private:
				TinyVisualFactory(TinyVisualDocument* document);
			public:
				TinyVisual*		Create(const TinyString& className, TinyVisual* spvisParent);
				TinyVisual*		Create(INT x, INT y, INT cx, INT cy, const TinyString& className, TinyVisual* spvisParent);
				BOOL			Destory(TinyVisual* spvis);
			private:
				TinyVisualDocument* m_document;
			};
		protected:
			TinyVisual*							m_spvisWindow;//根节点
			TinyVisual*							m_spvisCapture;
			TinyVisual*							m_spvisFocus;
			TinyVisual*							m_spvisActive;
			TinyVisual*							m_spvisLastMouse;//当前鼠标所在的元素
			TinyVisualFrame*					m_pWindow;
			TinyScopedPtr<TinyVisualBuilder>	m_parse;
			TinyScopedPtr<TinyVisualFactory>	m_vs;
		public:
#ifdef _DEBUG
			void			Dump();
			void			Dump(TinyVisual* spvis, INT& deep);
#endif
		};
	};
}



