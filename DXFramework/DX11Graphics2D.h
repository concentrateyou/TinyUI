#pragma once
#include "DX11Image2D.h"
#include "DXCamera.h"
#include "DX11Line2D.h"
#include "DX11Rectangle2D.h"
#include "DX11NV12Shader.h"
#include "DX11NV12Video.h"
#include "DX11YUVShader.h"
#include "DX11YUY2Shader.h"
#include "DX11YUVVideo.h"
#include "DX11YUY2Video.h"

namespace DXFramework
{
	class DX11Graphics2D
	{
		DISALLOW_COPY_AND_ASSIGN(DX11Graphics2D)
	public:
		DX11Graphics2D(DX11& dx11);
		virtual ~DX11Graphics2D();
		BOOL					InitializeShaders();
		BOOL					Create();//ƒ¨»œ¥¥Ω®±≥æ∞¿Î∆¡
		BOOL					Create(D3D11_TEXTURE2D_DESC& desc);
		BOOL					Create(INT cx, INT cy);//B8G8R8A8
		void					Destory();
	public:
		BOOL					BeginDraw();
		BOOL					DrawImageNV12BT601(DX11NV12Video& image);
		BOOL					DrawImageNV12BT709(DX11NV12Video& image);
		BOOL					DrawImageYUVBT601(DX11YUVVideo& image);
		BOOL					DrawImageYUVBT709(DX11YUVVideo& image);
		BOOL					DrawImageYUY2BT601(DX11YUY2Video& image);
		BOOL					DrawImageYUY2BT709(DX11YUY2Video& image);
		BOOL					DrawImage(DX11Image2D& image);
		BOOL					DrawLine(DX11Line2D& line, const XMFLOAT2* points, DWORD count, const XMFLOAT4& color);
		BOOL					DrawRectangle(DX11Rectangle2D& rectangle, const XMFLOAT2 points[4], const XMFLOAT4& color);
		BOOL					FillRectangle(DX11Rectangle2D& rectangle, const XMFLOAT2 points[4], const XMFLOAT4& color);
		BOOL					EndDraw();
		BOOL					Map(D3D11_MAPPED_SUBRESOURCE& map);
		BOOL					Unmap();
		HANDLE					GetHandle();
	public:
		BOOL					IsEmpty() const;
		TinySize				GetSize() const;
		ID3D11Texture2D*		GetTexture2D() const;
		ID3D11RenderTargetView* GetRTView() const;
		ID3D11DepthStencilView* GetDSView() const;
	public:
		BOOL					SaveAs(const CHAR* pzName, IMAGE_FILE_FORMAT format);
	protected:
		BOOL								m_swap;
		DX11&								m_dx11;
		TinySize							m_size;
		DXCamera							m_camera;
		DX11ColorShader						m_colorSharder;
		DX11TextureShader					m_textureShader;
		DX11NV12Shader						m_shaderNV12BT601;
		DX11NV12Shader						m_shaderNV12BT709;
		DX11YUVShader						m_shaderYUVBT601;
		DX11YUVShader						m_shaderYUVBT709;
		DX11YUY2Shader						m_shaderYUY2BT601;
		DX11YUY2Shader						m_shaderYUY2BT709;
		TinyComPtr<ID3D11Texture2D>			m_render2D;
		TinyComPtr<ID3D11Texture2D>			m_depth2D;
		TinyComPtr<ID3D11DepthStencilView>	m_depthView;
		TinyComPtr<ID3D11RenderTargetView>	m_renderView;
	};
}


