//#pragma once
//#include "DX9Element2D.h"
//#include "DX9Texture2D.h"
//#include "Render/TinyGDI.h"
//using namespace TinyFramework;
//
//namespace DXFramework
//{
//	class DX9Image2D : public DX9Element2D, public DX9Texture2D
//	{
//		struct VERTEXTYPE
//		{
//			FLOAT x, y, z, rhw;
//			DWORD color;
//			FLOAT u, v;
//		};
//		DECLARE_DYNAMIC(DX9Image2D)
//		DISALLOW_COPY_AND_ASSIGN(DX9Image2D)
//	public:
//		DX9Image2D();
//		virtual ~DX9Image2D();
//		BOOL	Load(DX9& dx9, const CHAR* pzFile);
//		BOOL	Load(DX9& dx9, const BYTE* bits, LONG size);
//	public:
//		BOOL	Create(DX9& dx9, INT cx, INT cy, const BYTE* bits) OVERRIDE;
//		BOOL	Allocate(DX9& dx9) OVERRIDE;
//		BOOL	Process(DX9& dx9) OVERRIDE;
//		void	Deallocate(DX9& dx9) OVERRIDE;
//	private:
//		BOOL	Initialize(DX9& dx9);
//		BOOL	Calculate(DX9& dx9);
//	protected:
//		TinyComPtr<IDirect3DVertexBuffer9>	m_vertexs;
//	};
//}
//
