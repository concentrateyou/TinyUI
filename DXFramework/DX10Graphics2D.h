#pragma once
#include "DXCamera.h"
#include "DX10Image2D.h"
#include "DX10Line2D.h"
#include "DX10Rectangle2D.h"
#include "DX10NV12BT601Shader.h"
#include "DX10NV12BT709Shader.h"
#include "DX10NV12Video.h"
#include "DX10YUVBT601Shader.h"
#include "DX10YUVBT709Shader.h"
#include "DX10YUVVideo.h"

namespace DXFramework
{
	class DX10Graphics2D
	{
		DISALLOW_COPY_AND_ASSIGN(DX10Graphics2D)
	public:
		DX10Graphics2D(DX10& dx10);
		virtual ~DX10Graphics2D();
		DX10&					GetDX10();
		BOOL					InitializeShaders();
		BOOL					Create();//默认创建背景离屏
		BOOL					Create(D3D10_TEXTURE2D_DESC& desc);
		BOOL					Create(INT cx, INT cy);//B8G8R8A8
		void					Destory();
	public:
		BOOL					BeginDraw();
		BOOL					DrawImageNV12BT601(DX10NV12Video& image);
		BOOL					DrawImageNV12BT709(DX10NV12Video& image);
		BOOL					DrawImageYUVBT601(DX10YUVVideo& image);
		BOOL					DrawImageYUVBT709(DX10YUVVideo& image);
		BOOL					DrawImage(DX10Image2D& image);
		BOOL					DrawLine(DX10Line2D& line, const XMFLOAT2* points, DWORD count, const XMFLOAT4& color);
		BOOL					DrawRectangle(DX10Rectangle2D& rectangle, const XMFLOAT2 points[4], const XMFLOAT4& color);
		BOOL					FillRectangle(DX10Rectangle2D& rectangle, const XMFLOAT2 points[4], const XMFLOAT4& color);
		BOOL					EndDraw();
		BOOL					Map(D3D10_MAPPED_TEXTURE2D& map);
		BOOL					Unmap();
		HANDLE					GetHandle();//获得共享句柄
	public:
		BOOL					IsEmpty() const;
		TinySize				GetSize() const;
		ID3D10Texture2D*		GetTexture2D() const;
		ID3D10RenderTargetView* GetRTView() const;
		ID3D10DepthStencilView* GetDSView() const;
	public:
		BOOL					SaveAs(const CHAR* pzName, IMAGE_FILE_FORMAT format);
	protected:
		BOOL								m_swap;
		DX10&								m_dx10;
		TinySize							m_size;
		DXCamera							m_camera;
		DX10ColorShader						m_colorSharder;
		DX10TextureShader					m_textureShader;
		DX10NV12BT601Shader					m_shaderNV12BT601;
		DX10NV12BT709Shader					m_shaderNV12BT709;
		DX10YUVBT601Shader					m_shaderYUVBT601;
		DX10YUVBT709Shader					m_shaderYUVBT709;
		TinyComPtr<ID3D10Texture2D>			m_render2D;
		TinyComPtr<ID3D10Texture2D>			m_depth2D;
		TinyComPtr<ID3D10DepthStencilView>	m_depthView;
		TinyComPtr<ID3D10RenderTargetView>	m_renderView;
	};
}


