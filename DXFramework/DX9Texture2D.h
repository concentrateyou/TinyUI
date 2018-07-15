//#pragma once
//#include "DX9.h"
//
//namespace DXFramework
//{
//	class DX9Texture2D
//	{
//		DISALLOW_COPY_AND_ASSIGN(DX9Texture2D)
//	public:
//		DX9Texture2D();
//		virtual ~DX9Texture2D();
//		virtual BOOL Load(DX9& dx9, const CHAR* pzFile, TinySize& size);
//		virtual BOOL Load(DX9& dx9, const BYTE* bits, LONG size, TinySize& texture2DSize);
//		virtual BOOL Create(DX9& dx9, INT cx, INT cy, const BYTE* bits);
//		virtual void Destory();
//		virtual BOOL SaveAs(const CHAR* pzFile, D3DXIMAGE_FILEFORMAT dxgi);
//	public:
//		BOOL	IsEmpty() const;
//		BOOL	Copy(BYTE* bits, INT linesize, INT cy);
//		BOOL	Copy(BYTE* bits, LONG size);
//	protected:
//		TinyComPtr<IDirect3DTexture9>	m_texture2D;
//	};
//}
//
//
