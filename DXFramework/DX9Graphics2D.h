//#pragma once
//#include "DX9Image2D.h"
//#include "DX9Font2D.h"
//#include "DX9Line2D.h"
//
//namespace DXFramework
//{
//	/// <summary>
//	/// DX9 2D»­²¼
//	/// </summary>
//	class DX9Graphics2D
//	{
//		DISALLOW_COPY_AND_ASSIGN(DX9Graphics2D)
//	public:
//		DX9Graphics2D(DX9& dx9);
//		virtual ~DX9Graphics2D();
//		DX9&		GetDX9();
//		TinySize	GetSize() const;
//		BOOL		Create();
//		BOOL		Create(INT cx, INT cy);
//		void		Destory();
//		BOOL		Resize();
//		BOOL		Resize(INT cx, INT cy);
//	public:
//		BOOL		BeginDraw();
//		BOOL		DrawImage(DX9Image2D& image);
//		BOOL		DrawString(DX9Font2D& dx9Font2D, LPCSTR pzText, INT count, LPRECT pRect, DWORD dwFormat, D3DCOLOR color);
//		BOOL		DrawLine(DX9Line2D& dx9Line2D, D3DXVECTOR2 *pVertexList, DWORD dwVertexListCount, D3DCOLOR color);
//		BOOL		EndDraw();
//	protected:
//		DX9&							m_dx9;
//		TinySize						m_size;
//		TinyComPtr<IDirect3DSurface9>	m_render2D;
//	};
//}
//
//
