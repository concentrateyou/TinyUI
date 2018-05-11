#pragma once
#include "DX11Texture2D.h"

namespace DXFramework
{
	class DX11RenderView
	{
	public:
		DX11RenderView(DX11& dx11);
		virtual ~DX11RenderView();
		DX11&					GetDX11();
		TinySize				GetSize() const;
		BOOL					Create();//ƒ¨»œ¥¥Ω®±≥æ∞¿Î∆¡
		BOOL					Create(D3D11_TEXTURE2D_DESC& desc);
		BOOL					Create(INT cx, INT cy);//B8G8R8A8
		void					Destory();
		BOOL					Resize();//÷ÿ÷√±≥æ∞¿Î∆¡
		BOOL					Resize(INT cx, INT cy);
	public:
		BOOL					BeginDraw();
		BOOL					EndDraw();
		BOOL					Map(D3D11_MAPPED_SUBRESOURCE& map);
		BOOL					Unmap();
		HANDLE					GetHandle();//ªÒµ√π≤œÌæ‰±˙
	public:
		ID3D11Texture2D*		GetTexture2D() const;
		ID3D11RenderTargetView* GetRTView() const;
		ID3D11DepthStencilView* GetDSView() const;
	public:
		BOOL					SaveAs(const CHAR* pzName, D3DX11_IMAGE_FILE_FORMAT format);
	protected:
		DX11&								m_dx11;
		TinySize							m_size;
		TinyComPtr<ID3D11Texture2D>			m_render2D;
		TinyComPtr<ID3D11Texture2D>			m_depth2D;
		TinyComPtr<ID3D11DepthStencilView>	m_depthView;
		TinyComPtr<ID3D11RenderTargetView>	m_renderView;
	};
}


