#include "stdafx.h"
#include "DX10Graphics2D.h"

namespace DXFramework
{
	DX10Graphics2D::DX10Graphics2D(DX10& dx10)
		:m_dx10(dx10),
		m_swap(TRUE)
	{

	}

	DX10Graphics2D::~DX10Graphics2D()
	{
	}
	DX10& DX10Graphics2D::GetDX10()
	{
		return m_dx10;
	}
	BOOL DX10Graphics2D::IsEmpty() const
	{
		return m_render2D == NULL || m_renderView == NULL;
	}
	TinySize DX10Graphics2D::GetSize() const
	{
		return m_size;
	}
	BOOL DX10Graphics2D::InitializeShaders()
	{
		string str;
		str.resize(MAX_PATH);
		GetModuleFileName(NULL, &str[0], MAX_PATH);
		str = str.substr(0, str.find_last_of("\\", string::npos, 1));
		string vs = str + "\\Shader\\DX10\\texture.vs";
		ASSERT(PathFileExists(vs.c_str()));
		string ps = str + "\\Shader\\DX10\\texture.ps";
		ASSERT(PathFileExists(ps.c_str()));
		if (!m_textureShader.Initialize(m_dx10, vs.c_str(), ps.c_str()))
			return FALSE;
		vs = str + "\\Shader\\DX10\\color.vs";
		ASSERT(PathFileExists(vs.c_str()));
		ps = str + "\\Shader\\DX10\\color.ps";
		ASSERT(PathFileExists(ps.c_str()));
		if (!m_colorSharder.Initialize(m_dx10, vs.c_str(), ps.c_str()))
			return FALSE;
		vs = str + "\\Shader\\DX10\\NV12BT601.vs";
		ASSERT(PathFileExists(vs.c_str()));
		ps = str + "\\Shader\\DX10\\NV12BT601.ps";
		ASSERT(PathFileExists(ps.c_str()));
		if (!m_shaderNV12BT601.Initialize(m_dx10, vs.c_str(), ps.c_str()))
			return FALSE;
		vs = str + "\\Shader\\DX10\\NV12BT709.vs";
		ASSERT(PathFileExists(vs.c_str()));
		ps = str + "\\Shader\\DX10\\NV12BT709.ps";
		ASSERT(PathFileExists(ps.c_str()));
		if (!m_shaderNV12BT709.Initialize(m_dx10, vs.c_str(), ps.c_str()))
			return FALSE;
		vs = str + "\\Shader\\DX10\\YUVBT601.vs";
		ASSERT(PathFileExists(vs.c_str()));
		ps = str + "\\Shader\\DX10\\YUVBT601.ps";
		ASSERT(PathFileExists(ps.c_str()));
		if (!m_shaderYUVBT601.Initialize(m_dx10, vs.c_str(), ps.c_str()))
			return FALSE;
		vs = str + "\\Shader\\DX10\\YUVBT709.vs";
		ASSERT(PathFileExists(vs.c_str()));
		ps = str + "\\Shader\\DX10\\YUVBT709.ps";
		ASSERT(PathFileExists(ps.c_str()));
		if (!m_shaderYUVBT709.Initialize(m_dx10, vs.c_str(), ps.c_str()))
			return FALSE;
		m_camera.SetPosition(0.0F, 0.0F, -10.0F);
		m_camera.UpdateView();
		return TRUE;
	}
	BOOL DX10Graphics2D::Create()
	{
		if (m_dx10.IsEmpty())
			return FALSE;
		HRESULT hRes = m_dx10.GetSwapChain()->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID*)&m_render2D);
		if (hRes != S_OK)
			return FALSE;
		hRes = m_dx10.GetD3D()->CreateRenderTargetView(m_render2D, NULL, &m_renderView);
		if (hRes != S_OK)
			return FALSE;
		D3D10_TEXTURE2D_DESC desc;
		m_render2D->GetDesc(&desc);
		m_size.cx = desc.Width;
		m_size.cy = desc.Height;
		D3D10_TEXTURE2D_DESC depthDesc;
		ZeroMemory(&depthDesc, sizeof(depthDesc));
		depthDesc.Width = desc.Width;
		depthDesc.Height = desc.Height;
		depthDesc.MipLevels = 1;
		depthDesc.ArraySize = 1;
		depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthDesc.SampleDesc.Count = 1;
		depthDesc.SampleDesc.Quality = 0;
		depthDesc.Usage = D3D10_USAGE_DEFAULT;
		depthDesc.BindFlags = D3D10_BIND_DEPTH_STENCIL;
		depthDesc.CPUAccessFlags = 0;
		depthDesc.MiscFlags = 0;
		hRes = m_dx10.GetD3D()->CreateTexture2D(&depthDesc, NULL, &m_depth2D);
		if (hRes != S_OK)
			return FALSE;
		D3D10_DEPTH_STENCIL_VIEW_DESC depthViewDesc;
		ZeroMemory(&depthViewDesc, sizeof(depthViewDesc));
		depthViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthViewDesc.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
		depthViewDesc.Texture2D.MipSlice = 0;
		hRes = m_dx10.GetD3D()->CreateDepthStencilView(m_depth2D, &depthViewDesc, &m_depthView);
		if (hRes != S_OK)
			return FALSE;
		m_swap = TRUE;
		return TRUE;
	}
	BOOL DX10Graphics2D::Create(D3D10_TEXTURE2D_DESC& desc)
	{
		if (m_dx10.IsEmpty())
			return FALSE;
		m_size.cx = desc.Width;
		m_size.cy = desc.Height;
		HRESULT hRes = m_dx10.GetD3D()->CreateTexture2D(&desc, NULL, &m_render2D);
		if (hRes != S_OK)
			return FALSE;
		hRes = m_dx10.GetD3D()->CreateRenderTargetView(m_render2D, NULL, &m_renderView);
		if (hRes != S_OK)
			return FALSE;
		D3D10_TEXTURE2D_DESC depthDesc;
		ZeroMemory(&depthDesc, sizeof(depthDesc));
		depthDesc.Width = m_size.cx;
		depthDesc.Height = m_size.cy;
		depthDesc.MipLevels = 1;
		depthDesc.ArraySize = 1;
		depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthDesc.SampleDesc.Count = 1;
		depthDesc.SampleDesc.Quality = 0;
		depthDesc.Usage = D3D10_USAGE_DEFAULT;
		depthDesc.BindFlags = D3D10_BIND_DEPTH_STENCIL;
		depthDesc.CPUAccessFlags = 0;
		depthDesc.MiscFlags = 0;
		hRes = m_dx10.GetD3D()->CreateTexture2D(&depthDesc, NULL, &m_depth2D);
		if (hRes != S_OK)
			return FALSE;
		D3D10_DEPTH_STENCIL_VIEW_DESC depthViewDesc;
		ZeroMemory(&depthViewDesc, sizeof(depthViewDesc));
		depthViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthViewDesc.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
		depthViewDesc.Texture2D.MipSlice = 0;
		hRes = m_dx10.GetD3D()->CreateDepthStencilView(m_depth2D, &depthViewDesc, &m_depthView);
		if (hRes != S_OK)
			return FALSE;
		m_swap = FALSE;
		return TRUE;
	}
	BOOL DX10Graphics2D::Create(INT cx, INT cy)
	{
		if (m_dx10.IsEmpty())
			return FALSE;
		m_size.cx = cx;
		m_size.cy = cy;
		D3D10_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = cx;
		desc.Height = cy;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.BindFlags = D3D10_BIND_SHADER_RESOURCE | D3D10_BIND_RENDER_TARGET;
		desc.MiscFlags = D3D10_RESOURCE_MISC_SHARED | D3D10_RESOURCE_MISC_GDI_COMPATIBLE;
		desc.Usage = D3D10_USAGE_DEFAULT;
		HRESULT hRes = m_dx10.GetD3D()->CreateTexture2D(&desc, NULL, &m_render2D);
		if (hRes != S_OK)
			return FALSE;
		hRes = m_dx10.GetD3D()->CreateRenderTargetView(m_render2D, NULL, &m_renderView);
		if (hRes != S_OK)
			return FALSE;
		D3D10_TEXTURE2D_DESC depthDesc;
		ZeroMemory(&depthDesc, sizeof(depthDesc));
		depthDesc.Width = cx;
		depthDesc.Height = cy;
		depthDesc.MipLevels = 1;
		depthDesc.ArraySize = 1;
		depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthDesc.SampleDesc.Count = 1;
		depthDesc.SampleDesc.Quality = 0;
		depthDesc.Usage = D3D10_USAGE_DEFAULT;
		depthDesc.BindFlags = D3D10_BIND_DEPTH_STENCIL;
		depthDesc.CPUAccessFlags = 0;
		depthDesc.MiscFlags = 0;
		hRes = m_dx10.GetD3D()->CreateTexture2D(&depthDesc, NULL, &m_depth2D);
		if (hRes != S_OK)
			return FALSE;
		D3D10_DEPTH_STENCIL_VIEW_DESC depthViewDesc;
		ZeroMemory(&depthViewDesc, sizeof(depthViewDesc));
		depthViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthViewDesc.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
		depthViewDesc.Texture2D.MipSlice = 0;
		hRes = m_dx10.GetD3D()->CreateDepthStencilView(m_depth2D, &depthViewDesc, &m_depthView);
		if (hRes != S_OK)
			return FALSE;
		m_swap = FALSE;
		return TRUE;
	}
	void DX10Graphics2D::Destory()
	{
		m_render2D.Release();
		m_renderView.Release();
		m_depth2D.Release();
		m_depthView.Release();
	}
	ID3D10RenderTargetView* DX10Graphics2D::GetRTView() const
	{
		return m_renderView;
	}
	ID3D10DepthStencilView* DX10Graphics2D::GetDSView() const
	{
		return m_depthView;
	}
	ID3D10Texture2D* DX10Graphics2D::GetTexture2D() const
	{
		return m_render2D;
	}
	BOOL DX10Graphics2D::Map(D3D10_MAPPED_TEXTURE2D& map)
	{
		if (m_dx10.IsEmpty())
			return FALSE;
		if (!m_render2D)
			return FALSE;
		if (FAILED(m_render2D->Map(D3D10CalcSubresource(0, 0, 1), D3D10_MAP_READ, 0, &map)))
			return FALSE;
		return TRUE;
	}
	BOOL DX10Graphics2D::Unmap()
	{
		if (m_dx10.IsEmpty())
			return FALSE;
		if (!m_render2D)
			return FALSE;
		m_render2D->Unmap(0);
		return TRUE;
	}

	HANDLE	DX10Graphics2D::GetHandle()
	{
		HANDLE handle = NULL;
		do
		{
			if (!m_render2D)
				break;
			TinyComPtr<IDXGIResource> resource;
			if (FAILED(m_render2D->QueryInterface(__uuidof(IDXGIResource), (void**)&resource)))
				break;
			if (FAILED(resource->GetSharedHandle(&handle)))
				break;
		} while (0);
		return handle;
	}
	BOOL DX10Graphics2D::BeginDraw()
	{
		if (m_dx10.IsEmpty())
			return FALSE;
		m_dx10.GetD3D()->OMSetRenderTargets(1, &m_renderView, m_depthView);
		m_dx10.SetViewport(TinyPoint(0, 0), m_size);
		m_dx10.SetMatrixs(m_size);
		FLOAT color[4] = { 0.0F, 0.0F, 0.0F, 1.0F };
		m_dx10.GetD3D()->ClearRenderTargetView(m_renderView, color);
		m_dx10.GetD3D()->ClearDepthStencilView(m_depthView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0F, 0);
		return TRUE;
	}
	BOOL DX10Graphics2D::EndDraw()
	{
		return TRUE;
	}
	BOOL DX10Graphics2D::DrawImageNV12BT601(DX10NV12Video& image)
	{
		if (image.IsEmpty())
			return FALSE;
		if (!image.DrawImage(m_dx10))
			return FALSE;
		XMMATRIX* ms = m_dx10.GetMatrixs();
		DX10Texture2D* s[2] = { image.GetTextureY(),image.GetTextureNV() };
		m_shaderNV12BT601.SetShaderParameters(m_dx10, ms[1], m_camera.GetView(), ms[2], s);
		ms[1] = XMMatrixIdentity();
		m_shaderNV12BT601.Render(m_dx10);
		return TRUE;
	}
	BOOL DX10Graphics2D::DrawImageNV12BT709(DX10NV12Video& image)
	{
		if (image.IsEmpty())
			return FALSE;
		if (!image.DrawImage(m_dx10))
			return FALSE;
		XMMATRIX* ms = m_dx10.GetMatrixs();
		DX10Texture2D* s[2] = { image.GetTextureY(),image.GetTextureNV() };
		m_shaderNV12BT709.SetShaderParameters(m_dx10, ms[1], m_camera.GetView(), ms[2], s);
		ms[1] = XMMatrixIdentity();
		m_shaderNV12BT709.Render(m_dx10);
		return TRUE;
	}

	BOOL DX10Graphics2D::DrawImageYUVBT601(DX10YUVVideo& image)
	{
		if (image.IsEmpty())
			return FALSE;
		if (!image.DrawImage(m_dx10))
			return FALSE;
		XMMATRIX* ms = m_dx10.GetMatrixs();
		DX10Texture2D* s[3] = { image.GetTextureY(),image.GetTextureU(),image.GetTextureV() };
		m_shaderYUVBT709.SetShaderParameters(m_dx10, ms[1], m_camera.GetView(), ms[2], s);
		ms[1] = XMMatrixIdentity();
		m_shaderYUVBT709.Render(m_dx10);
		return TRUE;
	}

	BOOL DX10Graphics2D::DrawImageYUVBT709(DX10YUVVideo& image)
	{
		if (image.IsEmpty())
			return FALSE;
		if (!image.DrawImage(m_dx10))
			return FALSE;
		XMMATRIX* ms = m_dx10.GetMatrixs();
		DX10Texture2D* s[3] = { image.GetTextureY(),image.GetTextureU(),image.GetTextureV() };
		m_shaderYUVBT601.SetShaderParameters(m_dx10, ms[1], m_camera.GetView(), ms[2], s);
		ms[1] = XMMatrixIdentity();
		m_shaderYUVBT601.Render(m_dx10);
		return TRUE;
	}

	BOOL DX10Graphics2D::DrawImage(DX10Image2D& image)
	{
		if (image.IsEmpty())
			return FALSE;
		if (!image.DrawImage(m_dx10))
			return FALSE;
		XMMATRIX* ms = m_dx10.GetMatrixs();
		m_textureShader.SetShaderParameters(m_dx10, ms[1], m_camera.GetView(), ms[2], &image);
		ms[1] = XMMatrixIdentity();
		m_textureShader.Render(m_dx10);
		return TRUE;
	}
	BOOL DX10Graphics2D::DrawLine(DX10Line2D& line, const XMFLOAT2* points, DWORD count, const XMFLOAT4& color)
	{
		if (!line.DrawLine(m_dx10, points, count, color))
			return FALSE;
		XMMATRIX* ms = m_dx10.GetMatrixs();
		ms[1] = XMMatrixIdentity();
		m_colorSharder.SetShaderParameters(m_dx10, ms[1], m_camera.GetView(), ms[2], line.GetIndexs());
		m_colorSharder.Render(m_dx10);
		return TRUE;
	}
	BOOL DX10Graphics2D::DrawRectangle(DX10Rectangle2D& rectangle, const XMFLOAT2 points[4], const XMFLOAT4& color)
	{
		if (!rectangle.DrawRectangle(m_dx10, points, color))
			return FALSE;
		XMMATRIX* ms = m_dx10.GetMatrixs();
		ms[1] = XMMatrixIdentity();
		m_colorSharder.SetShaderParameters(m_dx10, ms[1], m_camera.GetView(), ms[2], rectangle.GetIndexs());
		m_colorSharder.Render(m_dx10);
		return TRUE;
	}
	BOOL DX10Graphics2D::FillRectangle(DX10Rectangle2D& rectangle, const XMFLOAT2 points[4], const XMFLOAT4& color)
	{
		if (!rectangle.FillRectangle(m_dx10, points, color))
			return FALSE;
		XMMATRIX* ms = m_dx10.GetMatrixs();
		ms[1] = XMMatrixIdentity();
		m_colorSharder.SetShaderParameters(m_dx10, ms[1], m_camera.GetView(), ms[2], rectangle.GetIndexs());
		m_colorSharder.Render(m_dx10);
		return TRUE;
	}
	BOOL DX10Graphics2D::SaveAs(const CHAR* pzName, IMAGE_FILE_FORMAT format)
	{
		if (m_dx10.IsEmpty())
			return FALSE;
		wstring ws = StringToWString(pzName);
		if (m_render2D != NULL)
		{
			/*HRESULT hRes = SaveWICTextureToFile(m_dx11.GetImmediateContext(), m_render2D, GetWICCodec(format), ws.c_str());
			if (SUCCEEDED(hRes))
			{
				return TRUE;
			}*/
		}
		else
		{
			TinyComPtr<ID3D10Texture2D> texture2D;
			HRESULT hRes = m_dx10.GetSwapChain()->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID*)&texture2D);
			if (hRes != S_OK)
				return FALSE;
			/*hRes = SaveWICTextureToFile(m_dx11.GetImmediateContext(), texture2D, GetWICCodec(format), ws.c_str());
			if (SUCCEEDED(hRes))
			{
				return TRUE;
			}*/
		}
		return FALSE;
	}
}
