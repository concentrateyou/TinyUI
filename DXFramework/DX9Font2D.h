//#pragma once
//#include "DX9.h"
//#include <Richedit.h>
//#include <d3dx9core.h>
//
//namespace DXFramework
//{
//	/// <summary>
//	/// DX9字体
//	/// </summary>
//	class DX9Font2D
//	{
//		DISALLOW_COPY_AND_ASSIGN(DX9Font2D)
//	public:
//		DX9Font2D();
//		virtual ~DX9Font2D();
//		virtual BOOL Load(DX9& dx9, HFONT hFONT);
//		virtual BOOL Create(DX9& dx9, D3DXFONT_DESC& desc);
//		virtual void Destory();
//	public:
//		BOOL	IsEmpty() const;
//	public:
//		/// <summary>
//		/// 获得字体高度
//		/// </summary>
//		INT	 MeasureString(DX9& dx9, LPCSTR pzText, INT count, LPRECT pRect, DWORD dwFormat, D3DCOLOR color);
//		BOOL DrawString(DX9& dx9, LPCSTR pzText, INT count, LPRECT pRect, DWORD dwFormat, D3DCOLOR color);
//	private:
//		TinyComPtr<ID3DXFont>	m_d3dFONT;
//	};
//}
//
//
//
