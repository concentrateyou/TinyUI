#include "stdafx.h"
#include "DX10GraphicsCapture.h"

namespace GraphicsCapture
{
	DX10CaptureDATA::DX10CaptureDATA()
		:m_iFlag(0),
		m_linesize(0)
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
		m_linesize = map.RowPitch;
		m_copy2D->Unmap(0);
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = cx;
		desc.Height = cy;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = dxgiFormat;
		desc.BindFlags = 0;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D10_USAGE_DEFAULT;
		desc.MiscFlags = 0;
		hRes = device->CreateTexture2D(&desc, NULL, &m_texture2D);
		if (FAILED(hRes))
			return FALSE;
		return TRUE;
	}
	void DX10CaptureDATA::Destory()
	{
		if (TestF(DX_LOCK_STATE))
		{
			LOG(INFO) << "[DX10CaptureDATA] Destory Unmap";
			m_texture2D->Unmap(0);
		}
		m_iFlag = 0;
		m_linesize = 0;
		m_size.SetSize(0, 0);
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
	BOOL DX10CaptureDATA::TestF(INT32 val) const
	{
		return ((m_iFlag & val) != 0);
	}
	void DX10CaptureDATA::SetF(INT32 val)
	{
		m_iFlag |= val;
	}
	void DX10CaptureDATA::CrlF(INT32 val)
	{
		m_iFlag &= ~val;
	}
	UINT32 DX10CaptureDATA::GetPitch() const
	{
		return m_linesize;
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
		m_bGPU(FALSE),
		m_hD3D10(NULL),
		m_dx(dx),
		m_currentCopy(0)
	{
		m_copy.CreateEvent();
		m_stop.CreateEvent();
		ZeroMemory(&m_tls, sizeof(m_tls));
		for (INT i = 0; i < NUM_BUFFERS; i++)
		{
			m_captures[i] = new DX10CaptureDATA();
		}
	}
	DX10GraphicsCapture::~DX10GraphicsCapture()
	{
		for (INT i = 0; i < NUM_BUFFERS; i++)
		{
			m_captures[i]->Destory();
			SAFE_DELETE(m_captures[i]);
			m_captures[i] = NULL;
		}
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
		if (FAILED(hRes = (*d3d10Create)(NULL, D3D10_DRIVER_TYPE_HARDWARE, NULL, 0, D3D10_SDK_VERSION, &desc, &swap, &device)))
			return FALSE;
		LOG(INFO) << "[DX10GraphicsCapture] Initialize OK";
		return TRUE;
	}
	void DX10GraphicsCapture::Reset()
	{
		m_stop.SetEvent();
		m_captureTask.Close(500);
		m_currentCopy = 0;
		if (!m_bGPU)
		{
			for (INT i = 0; i < NUM_BUFFERS; i++)
			{
				DX10CaptureDATA* pDATA = m_captures[i];
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
	BOOL DX10GraphicsCapture::Setup(IDXGISwapChain *swap)
	{
		HookDATA* hookDATA = m_dx.GetHookDATA();
		HRESULT hRes = S_OK;
		TinyComPtr<ID3D10Device> device;
		hRes = swap->GetDevice(__uuidof(ID3D10Device), (void**)&device);
		if (FAILED(hRes))
			return FALSE;
		DXGI_SWAP_CHAIN_DESC scd;
		::ZeroMemory(&scd, sizeof(scd));
		hRes = swap->GetDesc(&scd);
		if (FAILED(hRes))
			return FALSE;
		m_dxgiFormat = GetDXTextureFormat(scd.BufferDesc.Format);
		hookDATA->Format = m_dxgiFormat;
		hookDATA->Size.cx = scd.BufferDesc.Width;
		hookDATA->Size.cy = scd.BufferDesc.Height;
		hookDATA->Window = scd.OutputWindow;
		hookDATA->bMultisample = scd.SampleDesc.Count > 1;
		TinyComPtr<ID3D10Texture2D> backBuffer;
		hRes = swap->GetBuffer(0, __uuidof(ID3D10Texture2D), (void**)&backBuffer);
		if (FAILED(hRes))
			return FALSE;
		m_dx.SetWindowsHook();
		return TRUE;
	}
	BOOL DX10GraphicsCapture::Draw(IDXGISwapChain *swap, UINT flags)
	{
		HRESULT hRes = S_OK;
		if (m_bCapturing && m_dx.m_stop.WaitEvent(0))
		{
			LOG(INFO) << "[Draw] Stop OK\n";
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
			HookDATA* hookDATA = m_dx.GetHookDATA();
			if (m_bCapturing && !m_bActive)
			{
				m_bGPU = !hookDATA->bCPU;
				if (m_bGPU)
				{
					m_bActive = DX10GPUHook(device);
				}
				else
				{
					m_bActive = DX10CPUHook(device);
				}
			}
			if (m_bActive && g_dx.IsFrameReady(hookDATA->Interval))
			{
				TinyComPtr<ID3D10Resource> backBuffer;
				if (SUCCEEDED(swap->GetBuffer(0, __uuidof(ID3D10Resource), (void**)&backBuffer)))
				{
					if (m_bGPU)
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
					else
					{
						QueryCopy(device);
						INT32 nextCopy = (m_currentCopy == NUM_BUFFERS - 1) ? 0 : (m_currentCopy + 1);
						DX10CaptureDATA* pDATA1 = m_captures[m_currentCopy];
						if (hookDATA->bMultisample)
						{
							device->ResolveSubresource(pDATA1->GetTexture2D(), 0, backBuffer, 0, m_dxgiFormat);
						}
						else
						{
							device->CopyResource(pDATA1->GetTexture2D(), backBuffer);
						}
						if (m_tls.m_wait < NUM_BUFFERS - 1)
						{
							m_tls.m_wait++;
						}
						else
						{
							DX10CaptureDATA* pDATA2 = m_captures[nextCopy];
							ID3D10Texture2D *src = pDATA2->GetTexture2D();
							ID3D10Texture2D *dst = pDATA2->GetCopy2D();
							BOOL bMap = FALSE;
							{
								TinyAutoLock autolock(m_lock);
								bMap = m_tls.m_map[nextCopy];
							}
							if (bMap)
							{
								pDATA2->Enter();
								dst->Unmap(0);
								pDATA2->CrlF(DX_LOCK_STATE);
								{
									TinyAutoLock autolock(m_lock);
									m_tls.m_map[nextCopy] = FALSE;
								}
								pDATA2->Leave();
							}
							if (hookDATA->bMultisample)
							{
								device->ResolveSubresource(dst, 0, src, 0, m_dxgiFormat);
							}
							else
							{
								device->CopyResource(dst, src);
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
	BOOL DX10GraphicsCapture::DX10GPUHook(ID3D10Device *device)
	{
		HookDATA* hookDATA = m_dx.GetHookDATA();
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
		if (!g_dx.CreateTextureDATA(hookDATA->MapID, hookDATA->MapSize))
			return FALSE;
		TextureDATA* textureDATA = m_dx.GetTextureDATA();
		textureDATA->TextureHandle = m_handle;
		m_dx.m_targetReady.SetEvent();
		LOG(INFO) << "[DX10GPUHook] OK";
		return TRUE;
	}
	void DX10GraphicsCapture::QueryCopy(ID3D10Device *device)
	{
		HRESULT hRes = S_OK;
		for (INT index = 0; index < NUM_BUFFERS; index++)
		{
			DX10CaptureDATA* pDATA = m_captures[index];
			if (!pDATA->TestF(DX_ISSUE_STATE))
				continue;
			pDATA->CrlF(DX_ISSUE_STATE);
			D3D10_MAPPED_TEXTURE2D map;
			hRes = pDATA->GetCopy2D()->Map(0, D3D10_MAP_READ, 0, &map);
			if (SUCCEEDED(hRes))
			{
				pDATA->SetF(DX_LOCK_STATE);
				{
					TinyAutoLock autolock(m_lock);
					m_tls.m_bits = map.pData;
					m_tls.m_current = index;
					m_tls.m_map[index] = TRUE;
				}
				m_copy.SetEvent();
			}
			break;
		}
	}
	BOOL DX10GraphicsCapture::DX10CPUHook(ID3D10Device *device)
	{
		HookDATA* hookDATA = m_dx.GetHookDATA();
		hookDATA->CaptureType = CAPTURETYPE_MEMORYTEXTURE;
		hookDATA->bFlip = FALSE;
		hookDATA->MapID++;
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
		if (!g_dx.CreateTextureDATA(hookDATA->MapID, hookDATA->MapSize))
			return FALSE;
		TextureDATA* textureDATA = m_dx.GetTextureDATA();
		textureDATA->Texture1Offset = size1;
		textureDATA->Texture2Offset = size1 + size2;
		textureDATA->TextureHandle = NULL;
		BYTE* address = reinterpret_cast<BYTE*>(textureDATA);
		m_tls.m_textures[0] = address + textureDATA->Texture1Offset;
		m_tls.m_textures[1] = address + textureDATA->Texture2Offset;
		m_captureTask.Submit(BindCallback(&DX10GraphicsCapture::OnMessagePump, this));
		m_dx.m_targetReady.SetEvent();
		LOG(INFO) << "[DX10CPUHook] OK";
		return TRUE;
	}
	DX10CaptureDATA* DX10GraphicsCapture::GetDX10CaptureDATA(LPVOID& bits)
	{
		TinyAutoLock autolock(m_lock);
		DX10CaptureDATA* pDATA = NULL;
		if (m_tls.m_current < NUM_BUFFERS)
		{
			pDATA = m_captures[m_tls.m_current];
			bits = m_tls.m_bits;
		}
		return pDATA;
	}
	void DX10GraphicsCapture::OnMessagePump()
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
			DX10CaptureDATA* pDATA = GetDX10CaptureDATA(bits);
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
}
