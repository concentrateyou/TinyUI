#include "stdafx.h"
#include "DX11GraphicsCapture.h"
#include <wincodec.h>

namespace GraphicsCapture
{
	DX11CaptureDATA::DX11CaptureDATA()
		:m_iFlag(0),
		m_linesize(0)
	{
	}
	DX11CaptureDATA::~DX11CaptureDATA()
	{

	}
	BOOL DX11CaptureDATA::Create(ID3D11Device *device, INT cx, INT cy, DXGI_FORMAT dxgiFormat)
	{
		HRESULT hRes = S_OK;
		TinyComPtr<ID3D11DeviceContext> context;
		device->GetImmediateContext(&context);
		ASSERT(context);
		D3D11_TEXTURE2D_DESC desc = {};
		desc.Width = cx;
		desc.Height = cy;
		desc.Format = dxgiFormat;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_STAGING;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		hRes = device->CreateTexture2D(&desc, NULL, &m_copy2D);
		if (FAILED(hRes))
			return FALSE;
		D3D11_MAPPED_SUBRESOURCE map;
		hRes = context->Map(m_copy2D, 0, D3D11_MAP_READ, 0, &map);
		if (FAILED(hRes))
			return FALSE;
		m_linesize = map.RowPitch;
		context->Unmap(m_copy2D, 0);
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = cx;
		desc.Height = cy;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = dxgiFormat;
		desc.BindFlags = 0;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.MiscFlags = 0;
		hRes = device->CreateTexture2D(&desc, NULL, &m_texture2D);
		if (FAILED(hRes))
			return FALSE;
		return TRUE;
	}
	void DX11CaptureDATA::Destory()
	{
		TinyComPtr<ID3D11Device> device;
		m_copy2D->GetDevice(&device);
		TinyComPtr<ID3D11DeviceContext> context;
		device->GetImmediateContext(&context);
		ASSERT(context);
		if (TestF(DX_LOCK_STATE))
		{
			LOG(INFO) << "[DX11CaptureDATA] Destory Unmap";
			context->Unmap(m_copy2D, 0);
		}
		m_iFlag = 0;
		m_linesize = 0;
		m_size.SetSize(0, 0);
		m_copy2D.Release();
		m_texture2D.Release();
	}
	BOOL DX11CaptureDATA::Enter()
	{
		return m_lock.Lock();
	}
	void DX11CaptureDATA::Leave()
	{
		m_lock.Unlock();
	}
	BOOL DX11CaptureDATA::TestF(INT32 val) const
	{
		return ((m_iFlag & val) != 0);
	}
	void DX11CaptureDATA::SetF(INT32 val)
	{
		m_iFlag |= val;
	}
	void DX11CaptureDATA::CrlF(INT32 val)
	{
		m_iFlag &= ~val;
	}
	UINT32 DX11CaptureDATA::GetPitch() const
	{
		return m_linesize;
	}
	ID3D11Texture2D* DX11CaptureDATA::GetCopy2D()
	{
		return m_copy2D;
	}
	ID3D11Texture2D* DX11CaptureDATA::GetTexture2D()
	{
		return m_texture2D;
	}
	//////////////////////////////////////////////////////////////////////////
	DX11GraphicsCapture::DX11GraphicsCapture(DX& dx)
		:m_dxgiFormat(DXGI_FORMAT_UNKNOWN),
		m_handle(NULL),
		m_bCapturing(FALSE),
		m_bActive(FALSE),
		m_dx(dx)
	{
		m_copy.CreateEvent();
		m_stop.CreateEvent();
		ZeroMemory(&m_tls, sizeof(m_tls));
		for (INT i = 0; i < NUM_BUFFERS; i++)
		{
			m_captures[i] = new DX11CaptureDATA();
		}
	}

	DX11GraphicsCapture::~DX11GraphicsCapture()
	{
		for (INT i = 0; i < NUM_BUFFERS; i++)
		{
			m_captures[i]->Destory();
			SAFE_DELETE(m_captures[i]);
			m_captures[i] = NULL;
		}
	}
	BOOL DX11GraphicsCapture::Initialize(HWND hWND, TinyComPtr<IDXGISwapChain>& swap)
	{
		HRESULT hRes = S_OK;
		CHAR szD3DPath[MAX_PATH];
		if (FAILED(hRes = SHGetFolderPath(NULL, CSIDL_SYSTEM, NULL, SHGFP_TYPE_CURRENT, szD3DPath)))
			return FALSE;
		strcat_s(szD3DPath, MAX_PATH, TEXT("\\d3d11.dll"));
		HMODULE hInstance = GetModuleHandle(szD3DPath);
		if (hInstance == NULL)
			return FALSE;
		D3D11CREATEPROC d3d11Create = (D3D11CREATEPROC)GetProcAddress(hInstance, TEXT("D3D11CreateDeviceAndSwapChain"));
		if (!d3d11Create)
			return FALSE;
		D3D_FEATURE_LEVEL levels[6] =
		{
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1,
		};
		DXGI_SWAP_CHAIN_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.BufferCount = 2;
		desc.BufferDesc.Width = 1;
		desc.BufferDesc.Height = 1;
		desc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		desc.BufferDesc.RefreshRate.Numerator = 0;
		desc.BufferDesc.RefreshRate.Denominator = 1;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.OutputWindow = hWND;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Windowed = TRUE;
		desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		desc.Flags = 0;
		TinyComPtr<ID3D11Device> device;
		TinyComPtr<ID3D11DeviceContext>	immediateContext;
		D3D_FEATURE_LEVEL level = D3D_FEATURE_LEVEL_9_3;
		if (FAILED(hRes = (*d3d11Create)(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, levels, 6, D3D11_SDK_VERSION, &desc, &swap, &device, &level, &immediateContext)))
			return FALSE;
		return TRUE;
	}
	void DX11GraphicsCapture::Reset()
	{
		m_stop.SetEvent();
		m_captureTask.Close(500);
		m_currentCopy = 0;
		if (!m_bGPU)
		{
			for (INT i = 0; i < NUM_BUFFERS; i++)
			{
				DX11CaptureDATA* pDATA = m_captures[i];
				pDATA->Destory();
			}
		}
		ZeroMemory(&m_tls, sizeof(m_tls));
		m_dx.m_textureDATA.Close();
		g_dx.m_start.SetEvent();
		m_handle = NULL;
		m_bActive = FALSE;
		m_bCapturing = FALSE;
		m_texture2D.Release();
	}
	BOOL DX11GraphicsCapture::Setup(IDXGISwapChain *swap)
	{
		HookDATA* hookDATA = m_dx.GetHookDATA();
		HRESULT hRes = S_OK;
		TinyComPtr<ID3D11Device> device;
		hRes = swap->GetDevice(__uuidof(ID3D11Device), (void**)&device);
		if (hRes != S_OK)
			return FALSE;
		DXGI_SWAP_CHAIN_DESC scd;
		::ZeroMemory(&scd, sizeof(scd));
		hRes = swap->GetDesc(&scd);
		if (hRes != S_OK)
			return FALSE;
		m_dxgiFormat = GetDXTextureFormat(scd.BufferDesc.Format);
		hookDATA->Format = m_dxgiFormat;
		hookDATA->Size.cx = scd.BufferDesc.Width;
		hookDATA->Size.cy = scd.BufferDesc.Height;
		hookDATA->Window = scd.OutputWindow;
		hookDATA->bMultisample = scd.SampleDesc.Count > 1;
		TinyComPtr<ID3D11Texture2D> backBuffer;
		hRes = swap->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
		if (hRes != S_OK)
			return FALSE;
		m_dx.SetWindowsHook();
		return TRUE;
	}
	BOOL DX11GraphicsCapture::Draw(IDXGISwapChain *swap, UINT flags)
	{
		HRESULT hRes = S_OK;
		if (m_bCapturing && m_dx.m_stop.WaitEvent(0))
		{
			m_bCapturing = FALSE;
			Reset();
			return FALSE;
		}
		if (!m_bCapturing && m_dx.m_start.WaitEvent(0))
		{
			m_bCapturing = TRUE;
		}
		TinyComPtr<ID3D11Device> device;
		if (SUCCEEDED(hRes = swap->GetDevice(__uuidof(ID3D11Device), (void**)&device)))
		{
			HookDATA* hookDATA = m_dx.GetHookDATA();
			if (m_bCapturing && !m_bActive)
			{
				m_bGPU = !hookDATA->bCPU;
				if (m_bGPU)
				{
					m_bActive = DX11GPUHook(device);
				}
				else
				{
					m_bActive = DX11CPUHook(device);
				}
			}
			if (m_bActive && g_dx.IsFrameReady(hookDATA->Interval))
			{
				TinyComPtr<ID3D11DeviceContext> context;
				device->GetImmediateContext(&context);
				TinyComPtr<ID3D11Resource> backBuffer;
				if (SUCCEEDED(swap->GetBuffer(0, __uuidof(ID3D11Resource), (void**)&backBuffer)))
				{
					if (m_bGPU)
					{
						if (hookDATA->bMultisample)
						{
							context->ResolveSubresource(m_texture2D, 0, backBuffer, 0, m_dxgiFormat);
						}
						else
						{
							context->CopyResource(m_texture2D, backBuffer);
						}
					}
					else
					{
						QueryCopy(context);
						INT32 nextCopy = (m_currentCopy == NUM_BUFFERS - 1) ? 0 : (m_currentCopy + 1);
						DX11CaptureDATA* pDATA1 = m_captures[m_currentCopy];
						if (hookDATA->bMultisample)
						{
							context->ResolveSubresource(pDATA1->GetTexture2D(), 0, backBuffer, 0, m_dxgiFormat);
						}
						else
						{
							context->CopyResource(pDATA1->GetTexture2D(), backBuffer);
						}
						if (m_tls.m_wait < NUM_BUFFERS - 1)
						{
							m_tls.m_wait++;
						}
						else
						{
							DX11CaptureDATA* pDATA2 = m_captures[nextCopy];
							ID3D11Texture2D *src = pDATA2->GetTexture2D();
							ID3D11Texture2D *dst = pDATA2->GetCopy2D();
							BOOL bMap = FALSE;
							{
								TinyAutoLock autolock(m_lock);
								bMap = m_tls.m_map[nextCopy];
							}
							if (bMap)
							{
								pDATA2->Enter();
								context->Unmap(dst, 0);
								pDATA2->CrlF(DX_LOCK_STATE);
								{
									TinyAutoLock autolock(m_lock);
									m_tls.m_map[nextCopy] = FALSE;
								}
								pDATA2->Leave();
							}
							if (hookDATA->bMultisample)
							{
								context->ResolveSubresource(dst, 0, src, 0, m_dxgiFormat);
							}
							else
							{
								context->CopyResource(dst, src);
							}
							pDATA2->SetF(DX_ISSUE_STATE);
						}
						m_currentCopy = nextCopy;
					}
				}
			}
		}
		return TRUE;
	}
	BOOL DX11GraphicsCapture::DX11CPUHook(ID3D11Device *device)
	{
		HookDATA* hookDATA = m_dx.GetHookDATA();
		hookDATA->CaptureType = CAPTURETYPE_MEMORYTEXTURE;
		hookDATA->bFlip = FALSE;
		hookDATA->MapID++;
		for (INT i = 0; i < NUM_BUFFERS; i++)
		{
			DX11CaptureDATA* pDATA = m_captures[i];
			if (!pDATA->Create(device, hookDATA->Size.cx, hookDATA->Size.cy, m_dxgiFormat))
				return FALSE;
		}
		hookDATA->Pitch = m_captures[0]->GetPitch();
		UINT size1 = (sizeof(TextureDATA) + 15) & 0xFFFFFFF0;
		UINT size2 = (hookDATA->Pitch * hookDATA->Size.cy + 15) & 0xFFFFFFF0;
		hookDATA->MapSize = size1 + size2 * 2;
		if (!g_dx.CreateTextureDATA(hookDATA->MapID, hookDATA->MapSize))
			return FALSE;
		TextureDATA* textureDATA = m_dx.GetTextureDATA();
		textureDATA->Texture1Offset = size1;
		textureDATA->Texture2Offset = size1 + size2;
		textureDATA->TextureHandle = NULL;
		BYTE* address = reinterpret_cast<BYTE*>(textureDATA);
		m_tls.m_textures[0] = address + textureDATA->Texture1Offset;
		m_tls.m_textures[1] = address + textureDATA->Texture2Offset;
		m_captureTask.Submit(BindCallback(&DX11GraphicsCapture::OnMessagePump, this));
		m_dx.m_targetReady.SetEvent();
		LOG(INFO) << "[DX11CPUHook] OK";
		return TRUE;
	}
	BOOL DX11GraphicsCapture::DX11GPUHook(ID3D11Device *device)
	{
		HookDATA* hookDATA = m_dx.GetHookDATA();
		HRESULT hRes = S_OK;
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = hookDATA->Size.cx;
		desc.Height = hookDATA->Size.cy;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = m_dxgiFormat;
		desc.SampleDesc.Count = 1;
		desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.MiscFlags = D3D11_RESOURCE_MISC_SHARED;
		if (FAILED(hRes = device->CreateTexture2D(&desc, NULL, &m_texture2D)))
			return FALSE;
		TinyComPtr<IDXGIResource> dxgiResource;
		if (FAILED(hRes = m_texture2D->QueryInterface(__uuidof(IDXGIResource), (void**)&dxgiResource)))
			return FALSE;
		if (FAILED(hRes = dxgiResource->GetSharedHandle(&m_handle)))
			return FALSE;
		hookDATA->CaptureType = CAPTURETYPE_SHAREDTEXTURE;
		hookDATA->bFlip = FALSE;
		hookDATA->MapSize = sizeof(TextureDATA);
		if (!g_dx.CreateTextureDATA(hookDATA->MapID, hookDATA->MapSize))
			return FALSE;
		TextureDATA* textureDATA = m_dx.GetTextureDATA();
		textureDATA->TextureHandle = m_handle;
		m_dx.m_targetReady.SetEvent();
		LOG(INFO) << "[DX11GPUHook] OK";
		return TRUE;
	}
	DX11CaptureDATA* DX11GraphicsCapture::GetDX11CaptureDATA(LPVOID& data)
	{
		TinyAutoLock autolock(m_lock);
		DX11CaptureDATA* pDATA = NULL;
		if (m_tls.m_current < NUM_BUFFERS)
		{
			pDATA = m_captures[m_tls.m_current];
			data = m_tls.m_data;
		}
		return pDATA;
	}
	void DX11GraphicsCapture::OnMessagePump()
	{
		HRESULT hRes = S_OK;
		HANDLE events[] = { m_copy, m_stop };
		DWORD dwCurrentID = 0;
		HookDATA* hookDATA = m_dx.GetHookDATA();
		INT32 cy = hookDATA->Size.cy;
		INT32 pitch = hookDATA->Pitch;
		for (;;)
		{
			INT32 currentCopy = 0;
			LPVOID bits = NULL;
			hRes = WaitForMultipleObjects(2, events, FALSE, INFINITE);
			if (hRes != WAIT_OBJECT_0)
			{
				break;
			}
			DX11CaptureDATA* pDATA = GetDX11CaptureDATA(bits);
			if (pDATA != NULL && !!bits)
			{
				pDATA->Enter();
				DWORD dwNextID = dwCurrentID == 0 ? 1 : 0;
				do
				{
					if (m_dx.m_mutes[dwCurrentID].Lock(0))
					{
						memcpy(m_tls.m_textures[dwCurrentID], bits, pitch * cy);
						TextureDATA* textureDATA = m_dx.GetTextureDATA();
						textureDATA->CurrentID = dwCurrentID;
						dwCurrentID = (dwCurrentID == 0 ? 1 : 0);
						m_dx.m_mutes[dwCurrentID].Unlock();
						break;
					}
					if (m_dx.m_mutes[dwNextID].Lock(0))
					{
						memcpy(m_tls.m_textures[dwNextID], bits, pitch * cy);
						TextureDATA* textureDATA = m_dx.GetTextureDATA();
						textureDATA->CurrentID = dwNextID;
						dwCurrentID = (dwNextID == 0 ? 1 : 0);
						m_dx.m_mutes[dwNextID].Unlock();
						break;
					}
				} while (0);
				pDATA->Leave();
			}
		}
	}
	void DX11GraphicsCapture::QueryCopy(ID3D11DeviceContext *context)
	{
		HRESULT hRes = S_OK;
		for (INT index = 0; index < NUM_BUFFERS; index++)
		{
			DX11CaptureDATA* pDATA = m_captures[index];
			if (!pDATA->TestF(DX_ISSUE_STATE))
				continue;
			pDATA->CrlF(DX_ISSUE_STATE);
			D3D11_MAPPED_SUBRESOURCE map;
			hRes = context->Map(pDATA->GetCopy2D(), 0, D3D11_MAP_READ, 0, &map);
			if (SUCCEEDED(hRes))
			{
				pDATA->SetF(DX_LOCK_STATE);
				{
					TinyAutoLock autolock(m_lock);
					m_tls.m_data = map.pData;
					m_tls.m_current = index;
					m_tls.m_map[index] = TRUE;
				}
				m_copy.SetEvent();
			}
			break;
		}
	}
}
