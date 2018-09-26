#include "stdafx.h"
#include "DX10GraphicsCapture.h"

namespace GraphicsCapture
{
	DX10CaptureDATA::DX10CaptureDATA()
		:m_bCopying(FALSE),
		m_bIssue(FALSE),
		m_pitch(0)
	{

	}
	DX10CaptureDATA::~DX10CaptureDATA()
	{

	}
	BOOL DX10CaptureDATA::Create(ID3D10Device *device, INT cx, INT cy, DXGI_FORMAT dxgiFormat)
	{
		D3D10_TEXTURE2D_DESC desc = {};
		desc.Width = cx;
		desc.Height = cy;
		desc.Format = dxgiFormat;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D10_USAGE_STAGING;
		desc.CPUAccessFlags = D3D10_CPU_ACCESS_READ;
		HRESULT hRes = device->CreateTexture2D(&desc, NULL, &m_copy2D);
		if (FAILED(hRes))
			return FALSE;
		D3D10_MAPPED_TEXTURE2D map;
		hRes = m_copy2D->Map(0, D3D10_MAP_READ, 0, &map);
		if (FAILED(hRes))
			return FALSE;
		m_pitch = map.RowPitch;
		m_copy2D->Unmap(0);

		desc.Format = dxgiFormat;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D10_USAGE_DEFAULT;
		hRes = device->CreateTexture2D(&desc, NULL, &m_texture2D);
		if (FAILED(hRes))
			return FALSE;
		return TRUE;
	}
	void DX10CaptureDATA::Destory()
	{
		m_copy2D.Release();
		m_texture2D.Release();
	}
	BOOL DX10CaptureDATA::Enter()
	{
		return m_lock.Lock();
	}
	void DX10CaptureDATA::Leave()
	{
		m_lock.Unlock();
	}
	void DX10CaptureDATA::SetIssue(BOOL bIssue)
	{
		m_bIssue = bIssue;
	}
	BOOL DX10CaptureDATA::IsIssue() const
	{
		return m_bIssue;
	}
	void DX10CaptureDATA::SetCopying(BOOL bCopy)
	{
		m_bCopying = bCopy;
	}
	BOOL DX10CaptureDATA::IsCopying() const
	{
		return m_bCopying;
	}
	UINT32 DX10CaptureDATA::GetPitch() const
	{
		return m_pitch;
	}
	ID3D10Texture2D* DX10CaptureDATA::GetCopy2D()
	{
		return m_copy2D;
	}
	ID3D10Texture2D* DX10CaptureDATA::GetTexture2D()
	{
		return m_texture2D;
	}
	//////////////////////////////////////////////////////////////////////////
	DX10GraphicsCapture::DX10GraphicsCapture(DX& dx)
		:m_dxgiFormat(DXGI_FORMAT_UNKNOWN),
		m_handle(NULL),
		m_bCapturing(FALSE),
		m_bActive(FALSE),
		m_hD3D10(NULL),
		m_dx(dx),
		m_current(0),
		m_currentCopy(0),
		m_bits(NULL),
		m_dwCopy(0)
	{
		m_textures[0] = m_textures[1] = NULL;
		m_copy.CreateEvent();
		m_close.CreateEvent();
	}
	DX10GraphicsCapture::~DX10GraphicsCapture()
	{

	}
	BOOL DX10GraphicsCapture::Initialize(HWND hWND, TinyComPtr<IDXGISwapChain>& swap)
	{
		HRESULT hRes = S_OK;
		CHAR szD3DPath[MAX_PATH];
		if (FAILED(hRes = SHGetFolderPath(NULL, CSIDL_SYSTEM, NULL, SHGFP_TYPE_CURRENT, szD3DPath)))
			return FALSE;
		strcat_s(szD3DPath, MAX_PATH, TEXT("\\d3d10.dll"));
		m_hD3D10 = GetModuleHandle(szD3DPath);
		if (m_hD3D10 == NULL)
			return FALSE;
		D3D10CREATEPROC d3d10Create = (D3D10CREATEPROC)GetProcAddress(m_hD3D10, TEXT("D3D10CreateDeviceAndSwapChain"));
		if (!d3d10Create)
			return FALSE;
		DXGI_SWAP_CHAIN_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.BufferCount = 2;
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.BufferDesc.Width = 1;
		desc.BufferDesc.Height = 1;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.OutputWindow = hWND;
		desc.SampleDesc.Count = 1;
		desc.Windowed = TRUE;
		TinyComPtr<ID3D10Device> device;
		if (FAILED(hRes = (*d3d10Create)(NULL, D3D10_DRIVER_TYPE_NULL, NULL, 0, D3D10_SDK_VERSION, &desc, &swap, &device)))
			return FALSE;
		LOG(INFO) << "DX10Capture::Initialize OK\n";
		return TRUE;
	}
	void DX10GraphicsCapture::Reset()
	{
		m_bActive = FALSE;
		m_handle = NULL;
		m_texture2D.Release();
		m_dx.m_textureDATA.Close();
	}
	BOOL DX10GraphicsCapture::Setup(IDXGISwapChain *swap)
	{
		HookDATA* hookDATA = m_dx.GetHookDATA();
		ASSERT(hookDATA);
		HRESULT hRes = S_OK;
		TinyComPtr<ID3D10Device> device;
		hRes = swap->GetDevice(__uuidof(ID3D10Device), (void**)&device);
		if (hRes != S_OK)
			return FALSE;
		DXGI_SWAP_CHAIN_DESC scd;
		::ZeroMemory(&scd, sizeof(scd));
		hRes = swap->GetDesc(&scd);
		if (hRes != S_OK)
			return FALSE;
		m_dxgiFormat = GetDXTextureFormat(scd.BufferDesc.Format);
		hookDATA->Format = (DWORD)m_dxgiFormat;
		hookDATA->Size.cx = scd.BufferDesc.Width;
		hookDATA->Size.cy = scd.BufferDesc.Height;
		hookDATA->Window = scd.OutputWindow;
		hookDATA->bMultisample = scd.SampleDesc.Count > 1;
		TinyComPtr<ID3D10Texture2D> backBuffer;
		hRes = swap->GetBuffer(0, __uuidof(ID3D10Texture2D), (void**)&backBuffer);
		if (hRes != S_OK)
			return FALSE;
		m_dx.SetWindowsHook();
		return TRUE;
	}
	BOOL DX10GraphicsCapture::Draw(IDXGISwapChain *swap, UINT flags)
	{
		HRESULT hRes = S_OK;
		if (m_bCapturing && m_dx.m_stop.WaitEvent(0))
		{
			LOG(INFO) << "DX10Capture::Render m_stop OK\n";
			m_bCapturing = FALSE;
			Reset();
			return FALSE;
		}
		if (!m_bCapturing && m_dx.m_start.WaitEvent(0))
		{
			m_bCapturing = TRUE;
		}
		TinyComPtr<ID3D10Device> device;
		if (SUCCEEDED(hRes = swap->GetDevice(__uuidof(ID3D10Device), (void**)&device)))
		{
			if (m_bCapturing && !m_bActive)
			{
				HookDATA* hookDATA = m_dx.GetHookDATA();
				ASSERT(hookDATA);
				if (hookDATA->bCPU)
				{
					m_bActive = DX10CPUHook(device);
				}
				else
				{
					m_bActive = DX10GPUHook(device);
				}

			}
			if (m_bActive)
			{
				HookDATA* hookDATA = m_dx.GetHookDATA();
				ASSERT(hookDATA);
				TinyComPtr<ID3D10Texture2D> backBuffer;
				if (SUCCEEDED(swap->GetBuffer(0, __uuidof(ID3D10Texture2D), (void**)&backBuffer)))
				{
					if (hookDATA->bCPU)
					{
						QueryCopy(device);
						INT32 next = (m_current == NUM_BUFFERS - 1) ? 0 : (m_current + 1);
						DX10CaptureDATA* pDATA1 = m_captures[m_current];
						if (hookDATA->bMultisample)
						{
							D3D10_TEXTURE2D_DESC desc;
							backBuffer->GetDesc(&desc);
							device->ResolveSubresource(pDATA1->GetCopy2D(), 0, backBuffer, 0, desc.Format);
							device->CopyResource(pDATA1->GetTexture2D(), pDATA1->GetCopy2D());
						}
						else
						{
							device->CopyResource(pDATA1->GetTexture2D(), backBuffer);
						}
					}
					else
					{
						if (hookDATA->bMultisample)
						{
							device->ResolveSubresource(m_texture2D, 0, backBuffer, 0, m_dxgiFormat);
						}
						else
						{
							device->CopyResource(m_texture2D, backBuffer);
						}
					}
				}
			}
		}
		return TRUE;
	}
	BOOL DX10GraphicsCapture::DX10GPUHook(ID3D10Device *device)
	{
		HookDATA* hookDATA = m_dx.GetHookDATA();
		ASSERT(hookDATA);
		HRESULT hRes = S_OK;
		D3D10_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = hookDATA->Size.cx;
		desc.Height = hookDATA->Size.cy;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = m_dxgiFormat;
		desc.SampleDesc.Count = 1;
		desc.BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;
		desc.Usage = D3D10_USAGE_DEFAULT;
		desc.MiscFlags = D3D10_RESOURCE_MISC_SHARED;
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
		TextureDATA* textureDATA = m_dx.GetTextureDATA();
		textureDATA->TextureHandle = m_handle;
		m_dx.m_targetReady.SetEvent();
		LOG(INFO) << "[DX10GPUHook] OK";
		return TRUE;
	}
	void DX10GraphicsCapture::QueryCopy(ID3D10Device *device)
	{
		HRESULT hRes = S_OK;
		for (UINT i = 0; i < NUM_BUFFERS; i++)
		{
			DX10CaptureDATA* pDATA = m_captures[i];
			if (!pDATA->IsIssue())
				continue;
			D3D10_MAPPED_TEXTURE2D map;
			hRes = pDATA->GetCopy2D()->Map(0, D3D10_MAP_READ, 0, &map);
			if (SUCCEEDED(hRes))
			{
				g_dx.Enter();
				pDATA->SetCopying(TRUE);
				m_bits = map.pData;
				m_currentCopy = i;
				g_dx.Leave();
				m_copy.SetEvent();
			}
			break;
		}
	}
	BOOL DX10GraphicsCapture::DX10CPUHook(ID3D10Device *device)
	{
		HookDATA* hookDATA = m_dx.GetHookDATA();
		ASSERT(hookDATA);
		hookDATA->CaptureType = CAPTURETYPE_MEMORYTEXTURE;
		hookDATA->bFlip = FALSE;
		for (INT i = 0; i < NUM_BUFFERS; i++)
		{
			DX10CaptureDATA* pDATA = m_captures[i];
			if (!pDATA->Create(device, hookDATA->Size.cx, hookDATA->Size.cy, m_dxgiFormat))
				return FALSE;
		}
		hookDATA->Pitch = m_captures[0]->GetPitch();
		UINT size1 = (sizeof(TextureDATA) + 15) & 0xFFFFFFF0;
		UINT size2 = (hookDATA->Pitch * hookDATA->Size.cy + 15) & 0xFFFFFFF0;
		hookDATA->MapSize = size1 + size2 * 2;
		TextureDATA* textureDATA = m_dx.GetTextureDATA(hookDATA->MapSize);
		textureDATA->Texture1Offset = size1;
		textureDATA->Texture2Offset = size1 + size2;
		textureDATA->TextureHandle = NULL;
		BYTE* address = reinterpret_cast<BYTE*>(textureDATA);
		m_textures[0] = address + textureDATA->Texture1Offset;
		m_textures[1] = address + textureDATA->Texture2Offset;
		m_captureTask.Submit(BindCallback(&DX10GraphicsCapture::OnMessagePump, this));
		m_dx.m_targetReady.SetEvent();
		LOG(INFO) << "[DX10CPUHook] OK";
		return TRUE;
	}
	void DX10GraphicsCapture::OnMessagePump()
	{
		HRESULT hRes = S_OK;
		HANDLE events[] = { m_copy, m_close };
		DWORD dwCurrentID = 0;
		for (;;)
		{
			INT32 currentCopy = 0;
			LPVOID bits = NULL;
			hRes = WaitForMultipleObjects(2, events, FALSE, INFINITE);
			if (hRes != WAIT_OBJECT_0)
			{
				break;
			}
			g_dx.Enter();
			currentCopy = m_currentCopy;
			bits = m_bits;
			g_dx.Leave();
			if (currentCopy < NUM_BUFFERS && !!bits)
			{
				DX10CaptureDATA* pDATA = m_captures[currentCopy];
				pDATA->Enter();
				do
				{
					DWORD dwNextID = dwCurrentID == 0 ? 1 : 0;
					HookDATA* hookDATA = m_dx.GetHookDATA();
					ASSERT(hookDATA);
					if (m_dx.m_mutes[dwCurrentID].Lock(0))
					{
						memcpy(m_textures[dwCurrentID], bits, hookDATA->Pitch * hookDATA->Size.cy);
						TextureDATA* textureDATA = m_dx.GetTextureDATA(hookDATA->MapSize);
						textureDATA->CurrentID = dwCurrentID;
						m_dx.m_mutes[dwCurrentID].Unlock();
						dwCurrentID = dwCurrentID == 0 ? 1 : 0;
						break;
					}
					if (m_dx.m_mutes[dwNextID].Lock(0))
					{
						memcpy(m_textures[dwNextID], bits, hookDATA->Pitch * hookDATA->Size.cy);
						TextureDATA* textureDATA = m_dx.GetTextureDATA(hookDATA->MapSize);
						textureDATA->CurrentID = dwNextID;
						m_dx.m_mutes[dwNextID].Unlock();
						dwCurrentID = dwNextID == 0 ? 1 : 0;
						break;
					}
				} while (0);
				pDATA->Leave();
			}
		}
	}
}
