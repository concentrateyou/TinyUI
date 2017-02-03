#include "stdafx.h"
#include "DX8Capture.h"

namespace DXCapture
{
	void ConvertPixelFormat(LPBYTE in, DWORD inPitch, D3DFORMAT inFormat, DWORD outWidth, DWORD outHeight, LPBYTE out)
	{
		BOOL x8r8g8b8 = FALSE;
		BOOL x1r5g5b5 = FALSE;
		BOOL r5g6b5 = FALSE;
		BOOL r3g3b2 = FALSE;

		switch (inFormat)
		{
		case D3DFMT_X8R8G8B8:
		case D3DFMT_A8R8G8B8:
			x8r8g8b8 = TRUE;
			break;
		case D3DFMT_R5G6B5:
			r5g6b5 = TRUE;
			break;
		case D3DFMT_A1R5G5B5:
		case D3DFMT_X1R5G5B5:
			x1r5g5b5 = TRUE;
			break;
		case D3DFMT_R3G3B2:
			r3g3b2 = TRUE;
			break;
		default:
			return;
		}
		if (x8r8g8b8)
		{
			LPBYTE src = in;
			LPBYTE dst = out;
			DWORD size = 4 * outWidth;
			for (DWORD i = 0; i < outHeight; i++)
			{
				memcpy(dst, src, size);
				src += inPitch;
				dst += size;
			}
		}
		else if (r5g6b5)
		{
			LPDWORD dst = (LPDWORD)out;
			LPWORD src_row_begin = (LPWORD)in;
			for (DWORD j = 0; j < outHeight; j++)
			{
				LPWORD src = src_row_begin;
				for (DWORD i = 0; i < outWidth; i++)
				{
					WORD pxl = *src;
					*dst = 0xFF000000 + ((0xF800 & pxl) << 8) + ((0x07E0 & pxl) << 5) + ((0x001F & pxl) << 3);
					src++;
					dst++;
				}
				src_row_begin += inPitch;
			}
		}
		else if (x1r5g5b5)
		{
			LPDWORD dst = (LPDWORD)out;
			LPWORD src_row_begin = (LPWORD)in;
			for (DWORD j = 0; j < outHeight; j++)
			{
				LPWORD src = src_row_begin;
				for (DWORD i = 0; i < outWidth; i++)
				{
					WORD pxl = *src;
					*dst = 0xFF000000 + ((0x7C00 & pxl) << 9) + ((0x03E0 & pxl) << 6) + ((0x001F & pxl) << 3);
					src++;
					dst++;
				}
				src_row_begin += inPitch;
			}
		}
		else if (r3g3b2)
		{
			LPDWORD dst = (LPDWORD)out;
			LPBYTE src_row_begin = (LPBYTE)in;
			for (DWORD j = 0; j < outHeight; j++)
			{
				LPBYTE src = src_row_begin;
				for (DWORD i = 0; i < outWidth; i++)
				{
					BYTE pxl = *src;
					*dst = 0xFF000000 + ((0xE0 & pxl) << 16) + ((0x1C & pxl) << 11) + ((0x03 & pxl) << 6);
					src++;
					dst++;
				}
				src_row_begin += inPitch;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	HRESULT STDMETHODCALLTYPE DX8EndScene(IDirect3DDevice8 *device)
	{
		g_dx8.m_dX8EndScene.EndDetour();
		if (g_dx8.m_currentPointer == NULL)
		{
			g_dx8.m_currentPointer = g_dx8.Setup(device) ? device : NULL;
		}
		HRESULT hRes = device->EndScene();
		g_dx8.m_dX8EndScene.BeginDetour();
		return hRes;
	}
	HRESULT STDMETHODCALLTYPE DX8Present(IDirect3DDevice8* device, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion)
	{
		g_dx8.m_dX8Present.EndDetour();
		if (g_dx8.m_currentPointer == device)
		{
			g_dx8.Render(device);
		}
		HRESULT hRes = device->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
		g_dx8.m_dX8Present.BeginDetour();
		return hRes;
	}
	HRESULT STDMETHODCALLTYPE DX8Reset(IDirect3DDevice8* device, D3DPRESENT_PARAMETERS* pPresentationParameters)
	{
		g_dx8.m_dX8Reset.EndDetour();
		if (g_dx8.m_currentPointer == device)
		{
			g_dx8.Reset();
		}
		HRESULT hRes = device->Reset(pPresentationParameters);
		g_dx8.m_dX8Reset.BeginDetour();
		return hRes;
	}
	DXGI_FORMAT GetDXGIFormat8(D3DFORMAT format)
	{
		switch (format)
		{
		case D3DFMT_A2B10G10R10:    return DXGI_FORMAT_R10G10B10A2_UNORM;
		case D3DFMT_A8R8G8B8:       return DXGI_FORMAT_B8G8R8A8_UNORM;
		case D3DFMT_X8R8G8B8:       return DXGI_FORMAT_B8G8R8X8_UNORM;
		case D3DFMT_A1R5G5B5:       return DXGI_FORMAT_B5G5R5A1_UNORM;
		case D3DFMT_R5G6B5:         return DXGI_FORMAT_B5G6R5_UNORM;
		}
		return DXGI_FORMAT_UNKNOWN;
	}
	//////////////////////////////////////////////////////////////////////////
	DX8Capture::DX8Capture(DX& dx)
		:m_dx(dx),
		m_currentPointer(NULL),
		m_currentBits(NULL),
		m_dwCurrentTexture(0),
		m_dwCapture(0)
	{
		m_textures[0] = m_textures[1] = NULL;
		m_copy.CreateEvent();
		m_close.CreateEvent();
		for (INT i = 0; i < NUM_BUFFERS; i++)
		{
			m_surfaces[i].copying = FALSE;
		}
	}
	DX8Capture::~DX8Capture()
	{

	}

	BOOL DX8Capture::Initialize(HWND hWND)
	{
		HRESULT hRes = S_OK;
		CHAR szPath[MAX_PATH];
		if (FAILED(hRes = SHGetFolderPath(NULL, CSIDL_SYSTEM, NULL, SHGFP_TYPE_CURRENT, szPath)))
		{
			return FALSE;
		}
		strcat_s(szPath, MAX_PATH, TEXT("\\d3d8.dll"));
		m_hD3D8 = GetModuleHandle(szPath);
		if (m_hD3D8 == NULL)
		{
			return FALSE;
		}
		D3D8CREATEPROC d3d8Create = (D3D8CREATEPROC)GetProcAddress(m_hD3D8, TEXT("Direct3DCreate8"));
		if (!d3d8Create)
		{
			return FALSE;
		}
		TinyComPtr<IDirect3D8> d3d8(d3d8Create(D3D_SDK_VERSION));
		D3DPRESENT_PARAMETERS pp;
		::ZeroMemory(&pp, sizeof(pp));
		pp.Windowed = 1;
		pp.BackBufferWidth = 1;
		pp.BackBufferHeight = 1;
		pp.SwapEffect = D3DSWAPEFFECT_FLIP;
		pp.BackBufferFormat = D3DFMT_A8R8G8B8;
		pp.BackBufferCount = 1;
		pp.hDeviceWindow = hWND;
		TinyComPtr<IDirect3DDevice8> d3dDevice;
		if (FAILED(hRes = d3d8->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWND, D3DCREATE_HARDWARE_VERTEXPROCESSING, &pp, &d3dDevice)))
		{
			return FALSE;
		}
		ULONG *vtable = *(ULONG**)d3dDevice.Ptr();
		if (m_dX8EndScene.Initialize((FARPROC)*(vtable + 35), (FARPROC)DX8EndScene))
		{
			if (m_dX8EndScene.BeginDetour())
			{
				LOG(INFO) << "m_dX8EndScene.BeginDetour()\n";
				return TRUE;
			}
		}
		return TRUE;
	}
	BOOL DX8Capture::Render(IDirect3DDevice8 *d3d)
	{
		ASSERT(d3d);
		if (m_bCapturing && m_dx.m_stop.Lock(0))
		{
			LOG(INFO) << "DX8Capture m_stop\n";
			m_bCapturing = FALSE;
			Reset();
			return FALSE;
		}
		if (!m_bCapturing && m_dx.m_start.Lock(0))
		{
			LOG(INFO) << "DX8Capture m_start\n";
			m_bCapturing = TRUE;
		}
		if (m_bCapturing)
		{
			if (!m_bTextures)
			{
				TinyComPtr<IDirect3DSurface8> backBuffer;
				if (SUCCEEDED(d3d->GetRenderTarget(&backBuffer)))
				{
					D3DSURFACE_DESC sd;
					::ZeroMemory(&sd, sizeof(sd));
					if (SUCCEEDED(backBuffer->GetDesc(&sd)))
					{
						m_captureDATA.Format = sd.Format;
						m_captureDATA.Size.cx = sd.Width;
						m_captureDATA.Size.cy = sd.Height;
						m_bTextures = DX8CPUHook(d3d);
					}
				}
			}
			if (m_bTextures)
			{
				HRESULT hRes = S_OK;
				TinyComPtr<IDirect3DSurface8> backBuffer;
				hRes = d3d->GetRenderTarget(&backBuffer);
				if (FAILED(hRes))
					return FALSE;
				for (INT i = 0; i < NUM_BUFFERS; i++)
				{
					if (!m_surfaces[i].copying)
					{
						hRes = d3d->CopyRects(backBuffer, NULL, 0, m_surfaces[i].surface, NULL);
						if (FAILED(hRes))
							return FALSE;
						D3DLOCKED_RECT lr;
						hRes = m_surfaces[i].surface->LockRect(&lr, NULL, D3DLOCK_READONLY);
						if (FAILED(hRes))
							return FALSE;
						m_surfaces[i].copying = TRUE;
						m_lock.Lock();
						m_currentBits = lr.pBits;
						m_dwPitch = lr.Pitch;
						m_dwCurrentTexture = i;
						m_lock.Unlock();
						m_copy.SetEvent();
					}
				}
				DWORD dwNextCapture = (m_dwCapture == NUM_BUFFERS - 1) ? 0 : (m_dwCapture + 1);
				if (m_surfaces[dwNextCapture].copying)
				{
					TinyAutoLock lock(m_locks[dwNextCapture]);
					m_surfaces[dwNextCapture].surface->UnlockRect();
					m_surfaces[dwNextCapture].copying = FALSE;
				}
				m_dwCapture = dwNextCapture;
			}
		}
		return TRUE;
	}
	void DX8Capture::Reset()
	{
		m_bTextures = FALSE;
		m_copy.SetEvent();
		m_close.SetEvent();
		m_captureTask.Close(INFINITE);
		for (INT i = 0; i < NUM_BUFFERS; i++)
		{
			if (m_surfaces[i].surface)
			{
				m_surfaces[i].surface->UnlockRect();
				m_surfaces[i].surface.Release();
			}
		}
		m_dx.m_textureMemery.Unmap();
		m_dx.m_textureMemery.Close();
	}
	BOOL DX8Capture::Setup(IDirect3DDevice8 *device)
	{
		D3DDEVICE_CREATION_PARAMETERS parameters;
		if (SUCCEEDED(device->GetCreationParameters(&parameters)))
		{
			m_captureDATA.HwndCapture = parameters.hFocusWindow;
		}
		TinyComPtr<IDirect3DSurface8> backBuffer;
		if (SUCCEEDED(device->GetRenderTarget(&backBuffer)))
		{
			D3DSURFACE_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			if (SUCCEEDED(backBuffer->GetDesc(&desc)))
			{
				switch (desc.Format)
				{
				case D3DFMT_A8R8G8B8:
				case D3DFMT_X8R8G8B8:
				case D3DFMT_R5G6B5:
				case D3DFMT_A1R5G5B5:
				case D3DFMT_X1R5G5B5:
				case D3DFMT_R3G3B2:
				{
					m_d3dFormat = desc.Format;
					m_dxgiFormat = GetDXGIFormat8(desc.Format);
					m_captureDATA.CaptureType = CAPTURETYPE_MEMORYTEXTURE;
					m_captureDATA.Format = desc.Format;
					m_captureDATA.Size.cx = desc.Width;
					m_captureDATA.Size.cy = desc.Height;
					m_captureDATA.Pitch = 4 * desc.Width;
					m_dx.SetWindowsHook();
					TinyComPtr<IDirect3D8> d3d8;
					if (SUCCEEDED(device->GetDirect3D(&d3d8)))
					{
						m_dX8Present.Initialize(GetVTable(device, 15), (FARPROC)DX8Present);
						m_dX8Reset.Initialize(GetVTable(device, 14), (FARPROC)DX8Reset);
						m_dX8Present.BeginDetour();
						m_dX8Reset.BeginDetour();
						return TRUE;
					}
					return FALSE;
				}
				break;
				}
			}
		}
		return FALSE;
	}
	BOOL DX8Capture::DX8CPUHook(IDirect3DDevice8 *device)
	{
		m_captureDATA.CaptureType = CAPTURETYPE_MEMORYTEXTURE;
		m_captureDATA.bFlip = FALSE;
		SharedCaptureDATA* sharedCapture = m_dx.GetSharedCaptureDATA();
		memcpy(sharedCapture, &m_captureDATA, sizeof(m_captureDATA));
		HRESULT hRes = S_OK;
		for (INT i = 0; i < NUM_BUFFERS; i++)
		{
			hRes = device->CreateImageSurface(m_captureDATA.Size.cx, m_captureDATA.Size.cy, (D3DFORMAT)m_captureDATA.Format, &m_surfaces[i].surface);
			if (FAILED(hRes))
				return FALSE;
		}
		D3DLOCKED_RECT rect;
		hRes = m_surfaces[0].surface->LockRect(&rect, NULL, D3DLOCK_READONLY);
		if (FAILED(hRes))
			return FALSE;
		m_dwPitch = rect.Pitch;
		m_surfaces[0].surface->UnlockRect();
		UINT size1 = (sizeof(SharedTextureDATA) + 15) & 0xFFFFFFF0;
		UINT size2 = (m_captureDATA.Pitch * m_captureDATA.Size.cy + 15) & 0xFFFFFFF0;
		m_captureDATA.MapSize = size1 + size2 * 2;
		SharedTextureDATA* sharedTexture = m_dx.GetSharedTextureDATA(m_captureDATA.MapSize);
		sharedTexture->Texture1Offset = size1;
		sharedTexture->Texture2Offset = size1 + size2;
		sharedTexture->FrameTime = 0;
		sharedTexture->TextureHandle = NULL;
		BYTE* ps = m_dx.GetSharedTexture(m_captureDATA.MapSize);
		m_textures[0] = ps + sharedTexture->Texture1Offset;
		m_textures[1] = ps + sharedTexture->Texture2Offset;
		m_captureTask.Submit(BindCallback(&DX8Capture::OnMessagePump, this));
		m_dx.m_ready.SetEvent();
		return TRUE;
	}
	void DX8Capture::OnMessagePump()
	{
		HRESULT hRes = S_OK;
		HANDLE events[] = { NULL,NULL };
		DuplicateHandle(GetCurrentProcess(), m_copy, GetCurrentProcess(), &events[0], 0, FALSE, DUPLICATE_SAME_ACCESS);
		DuplicateHandle(GetCurrentProcess(), m_close, GetCurrentProcess(), &events[1], 0, FALSE, DUPLICATE_SAME_ACCESS);
		DWORD dwCurrentMutex = 0;
		for (;;)
		{
			DWORD dwCurrentTexture = 0;
			void *currentBits = NULL;
			hRes = WaitForMultipleObjects(2, events, FALSE, INFINITE);
			if (hRes != WAIT_OBJECT_0)
			{
				break;
			}

			DWORD dwNextMutex = dwCurrentMutex == 0 ? 1 : 0;

			m_lock.Lock();
			dwCurrentTexture = m_dwCurrentTexture;
			currentBits = m_currentBits;
			m_lock.Unlock();

			if (dwCurrentTexture < NUM_BUFFERS && currentBits != NULL)
			{
				TinyAutoLock lock(m_locks[dwCurrentTexture]);
				do
				{
					if (m_dx.m_mutes[dwCurrentMutex].Lock(0))
					{
						ConvertPixelFormat((LPBYTE)currentBits, m_dwPitch, m_d3dFormat, m_captureDATA.Size.cx, m_captureDATA.Size.cy, m_textures[dwCurrentMutex]);
						SharedTextureDATA* sharedTexture = m_dx.GetSharedTextureDATA(m_captureDATA.MapSize);
						sharedTexture->CurrentMutex = dwCurrentMutex;
						m_dx.m_mutes[dwCurrentMutex].Unlock();
						break;
					}
					if (m_dx.m_mutes[dwNextMutex].Lock(0))
					{
						ConvertPixelFormat((LPBYTE)currentBits, m_dwPitch, m_d3dFormat, m_captureDATA.Size.cx, m_captureDATA.Size.cy, m_textures[dwNextMutex]);
						SharedTextureDATA* sharedTexture = m_dx.GetSharedTextureDATA(m_captureDATA.MapSize);
						sharedTexture->CurrentMutex = dwNextMutex;
						m_dx.m_mutes[dwNextMutex].Unlock();
						break;
					}
				} while (0);
			}
			dwCurrentMutex = dwNextMutex;
		}
		for (INT i = 0; i < 2; i++)
		{
			if (events[i])
			{
				CloseHandle(events[i]);
				events[i] = NULL;
			}
		}
	}
}