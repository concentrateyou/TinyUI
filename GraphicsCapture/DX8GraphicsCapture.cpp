#include "stdafx.h"
#include "DX8GraphicsCapture.h"
#include "d3d8.h"

namespace GraphicsCapture
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
	typedef IDirect3D8* (WINAPI*D3D8CREATEPROC)(UINT);
	//////////////////////////////////////////////////////////////////////////
	HRESULT STDMETHODCALLTYPE DX8EndScene(IDirect3DDevice8 *pThis)
	{
		g_dx8.m_dX8EndScene.EndDetour();
		if (g_dx8.m_currentPointer == NULL)
		{
			g_dx8.m_currentPointer = g_dx8.Setup(pThis) ? pThis : NULL;
		}
		HRESULT hRes = pThis->EndScene();
		g_dx8.m_dX8EndScene.BeginDetour();
		return hRes;
	}
	HRESULT STDMETHODCALLTYPE DX8Present(IDirect3DDevice8* pThis, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion)
	{
		g_dx8.m_dX8Present.EndDetour();
		if (g_dx8.m_currentPointer == pThis)
		{
			g_dx8.Render(pThis);
		}
		HRESULT hRes = pThis->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
		g_dx8.m_dX8Present.BeginDetour();
		return hRes;
	}
	HRESULT STDMETHODCALLTYPE DX8Reset(IDirect3DDevice8* pThis, D3DPRESENT_PARAMETERS* pPresentationParameters)
	{
		g_dx8.m_dX8Reset.EndDetour();
		if (g_dx8.m_currentPointer == pThis)
		{
			g_dx8.Reset();
		}
		HRESULT hRes = pThis->Reset(pPresentationParameters);
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
	class DX8CaptureDATA
	{
	public:
		DX8CaptureDATA();
		~DX8CaptureDATA();
		BOOL				Create(IDirect3DDevice8* pThis, INT cx, INT cy, D3DFORMAT format);
		void				Destory();
		BOOL				Update();
		BOOL				Enter();
		void				Leave();
		void				SetCopying(BOOL bCopy);
		BOOL				IsCopying() const;
		BYTE*				GetPointer() const;
		UINT32				GetPitch() const;
		IDirect3DSurface8*	GetSurface();
	private:
		BOOL							m_bCopying;
		UINT32							m_pitch;
		BYTE*							m_bits;
		TinyLock						m_lock;
		TinySize						m_size;
		TinyComPtr<IDirect3DSurface8>	m_surface;
	};
	DX8CaptureDATA::DX8CaptureDATA()
		:m_bCopying(FALSE),
		m_bits(NULL),
		m_pitch(0)
	{

	}
	DX8CaptureDATA::~DX8CaptureDATA()
	{
		Destory();
	}
	void DX8CaptureDATA::SetCopying(BOOL bCopying)
	{
		m_bCopying = bCopying;
	}
	BOOL DX8CaptureDATA::IsCopying() const
	{
		return m_bCopying;
	}
	IDirect3DSurface8*	DX8CaptureDATA::GetSurface()
	{
		return m_surface;
	}
	BYTE* DX8CaptureDATA::GetPointer() const
	{
		return m_bits;
	}
	UINT32 DX8CaptureDATA::GetPitch() const
	{
		return m_pitch;
	}
	BOOL DX8CaptureDATA::Create(IDirect3DDevice8* pThis, INT cx, INT cy, D3DFORMAT format)
	{
		Destory();
		if (SUCCEEDED(pThis->CreateImageSurface(cx, cy, format, &m_surface)))
		{
			D3DLOCKED_RECT lr;
			if (SUCCEEDED(m_surface->LockRect(&lr, NULL, D3DLOCK_READONLY)))
			{
				if (m_pitch != lr.Pitch || m_size.cy != cy)
				{
					m_pitch = lr.Pitch;
					m_size.cx = cx;
					m_size.cy = cy;
					SAFE_DELETE_ARRAY(m_bits);
					m_bits = new BYTE[m_pitch * cy];
				}
				memcpy(m_bits, lr.pBits, m_pitch * cy);
				m_surface->UnlockRect();
				return TRUE;
			}
			return TRUE;
		}
		return FALSE;
	}
	BOOL DX8CaptureDATA::Update()
	{
		TinyAutoLock lock(m_lock);
		D3DLOCKED_RECT lr;
		if (SUCCEEDED(m_surface->LockRect(&lr, NULL, D3DLOCK_READONLY)))
		{
			if (m_pitch != lr.Pitch)
			{
				m_pitch = lr.Pitch;
				SAFE_DELETE_ARRAY(m_bits);
				m_bits = new BYTE[m_pitch * m_size.cy];
			}
			memcpy(m_bits, static_cast<LPBYTE>(lr.pBits), m_pitch * m_size.cy);
			m_surface->UnlockRect();
			m_bCopying = TRUE;
			return TRUE;
		}
		return FALSE;
	}
	void DX8CaptureDATA::Destory()
	{
		if (m_bCopying)
		{
			TinyAutoLock autolock(m_lock);
			m_surface->UnlockRect();
			m_bCopying = FALSE;
		}
		m_surface.Release();
		SAFE_DELETE_ARRAY(m_bits);
		m_pitch = 0;
	}
	BOOL DX8CaptureDATA::Enter()
	{
		return m_lock.Lock();
	}
	void DX8CaptureDATA::Leave()
	{
		m_lock.Unlock();
	}
	//////////////////////////////////////////////////////////////////////////
	DX8GraphicsCapture::DX8GraphicsCapture(DX& dx)
		:m_dx(dx),
		m_currentPointer(NULL),
		m_currentCapture(0)
	{
		m_textures[0] = m_textures[1] = NULL;
		m_copy.CreateEvent();
		m_close.CreateEvent();
		for (INT i = 0; i < NUM_BUFFERS; i++)
		{
			m_captures[i] = new DX8CaptureDATA();
		}
	}
	DX8GraphicsCapture::~DX8GraphicsCapture()
	{
		for (INT i = 0; i < NUM_BUFFERS; i++)
		{
			DX8CaptureDATA* pDATA = reinterpret_cast<DX8CaptureDATA*>(m_captures[i]);
			pDATA->Destory();
			SAFE_DELETE(pDATA);
			m_captures[i] = NULL;
		}
	}
	BOOL DX8GraphicsCapture::Initialize(HWND hWND)
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
	BOOL DX8GraphicsCapture::Render(LPVOID pThis)
	{
		IDirect3DDevice8 *d3d = reinterpret_cast<IDirect3DDevice8*>(pThis);
		ASSERT(pThis);
		if (m_bCapturing && m_dx.m_stop.WaitEvent(0))
		{
			LOG(INFO) << "DX8Capture m_stop\n";
			m_bCapturing = FALSE;
			Reset();
			return FALSE;
		}
		if (!m_bCapturing && m_dx.m_start.WaitEvent(0))
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
				if (hRes != S_OK)
					return FALSE;

				for (INT i = 0; i < NUM_BUFFERS; i++)
				{
					DX8CaptureDATA* pDATA = reinterpret_cast<DX8CaptureDATA*>(m_captures[i]);
					if (!pDATA->IsCopying())
					{
						hRes = d3d->CopyRects(backBuffer, NULL, 0, pDATA->GetSurface(), NULL);
						if (hRes != S_OK)
							return FALSE;
						if (!pDATA->Update())
							return FALSE;
						m_currentCapture = i;
						m_copy.SetEvent();
					}
				}
			}
		}
		return TRUE;
	}
	void DX8GraphicsCapture::Reset()
	{
		m_bTextures = FALSE;
		m_copy.SetEvent();
		m_close.SetEvent();
		m_captureTask.Close(INFINITE);
		for (INT i = 0; i < NUM_BUFFERS; i++)
		{
			DX8CaptureDATA* pDATA = reinterpret_cast<DX8CaptureDATA*>(m_captures[i]);
			pDATA->Destory();
		}
		m_dx.m_textureDATA.Unmap();
		m_dx.m_textureDATA.Close();
	}
	BOOL DX8GraphicsCapture::Setup(LPVOID pThis)
	{
		IDirect3DDevice8 *d3d = reinterpret_cast<IDirect3DDevice8*>(pThis);
		ASSERT(d3d);
		D3DDEVICE_CREATION_PARAMETERS parameters;
		if (SUCCEEDED(d3d->GetCreationParameters(&parameters)))
		{
			m_captureDATA.Window = parameters.hFocusWindow;
		}
		TinyComPtr<IDirect3DSurface8> backBuffer;
		if (SUCCEEDED(d3d->GetRenderTarget(&backBuffer)))
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
					m_d3dFormat = static_cast<DWORD>(desc.Format);
					m_dxgiFormat = GetDXGIFormat8(desc.Format);
					m_captureDATA.CaptureType = CAPTURETYPE_MEMORYTEXTURE;
					m_captureDATA.Format = desc.Format;
					m_captureDATA.Size.cx = desc.Width;
					m_captureDATA.Size.cy = desc.Height;
					m_captureDATA.Pitch = 4 * desc.Width;
					m_dx.SetWindowsHook();
					TinyComPtr<IDirect3D8> d3d8;
					if (SUCCEEDED(d3d->GetDirect3D(&d3d8)))
					{
						m_dX8Present.Initialize(GetVTable(d3d, 15), (FARPROC)DX8Present);
						m_dX8Reset.Initialize(GetVTable(d3d, 14), (FARPROC)DX8Reset);
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
	BOOL DX8GraphicsCapture::DX8CPUHook(LPVOID pThis)
	{
		IDirect3DDevice8 *d3d = reinterpret_cast<IDirect3DDevice8*>(pThis);
		ASSERT(d3d);
		HRESULT hRes = S_OK;
		m_captureDATA.CaptureType = CAPTURETYPE_MEMORYTEXTURE;
		m_captureDATA.bFlip = FALSE;
		for (INT i = 0; i < NUM_BUFFERS; i++)
		{
			DX8CaptureDATA* pDATA = reinterpret_cast<DX8CaptureDATA*>(m_captures[i]);
			if (!pDATA->Create(d3d, m_captureDATA.Size.cx, m_captureDATA.Size.cy, (D3DFORMAT)m_captureDATA.Format))
				return FALSE;
		}
		DX8CaptureDATA* pDATA = reinterpret_cast<DX8CaptureDATA*>(m_captures[0]);
		m_captureDATA.Pitch = pDATA->GetPitch();
		UINT size1 = (sizeof(TextureDATA) + 15) & 0xFFFFFFF0;
		UINT size2 = (m_captureDATA.Pitch * m_captureDATA.Size.cy + 15) & 0xFFFFFFF0;
		m_captureDATA.MapSize = size1 + size2 * 2;
		HookDATA* hookDATA = m_dx.GetHookDATA();
		memcpy(hookDATA, &m_captureDATA, sizeof(m_captureDATA));
		TextureDATA* textureDATA = m_dx.GetTextureDATA(m_captureDATA.MapSize);
		textureDATA->Texture1Offset = size1;
		textureDATA->Texture2Offset = size1 + size2;
		textureDATA->TextureHandle = NULL;
		BYTE* address = reinterpret_cast<BYTE*>(textureDATA);
		m_textures[0] = address + textureDATA->Texture1Offset;
		m_textures[1] = address + textureDATA->Texture2Offset;
		m_captureTask.Submit(BindCallback(&DX8GraphicsCapture::OnMessagePump, this));
		m_dx.m_targetReady.SetEvent();
		return TRUE;
	}
	void DX8GraphicsCapture::OnMessagePump()
	{
		HRESULT hRes = S_OK;
		HANDLE events[] = { m_copy,m_close };
		DWORD dwCurrentID = 0;
		for (;;)
		{
			UINT currentCapture = 0;
			hRes = WaitForMultipleObjects(2, events, FALSE, INFINITE);
			if (hRes != WAIT_OBJECT_0)
			{
				break;
			}
			currentCapture = m_currentCapture;
			DWORD dwNextID = dwCurrentID == 0 ? 1 : 0;
			DX8CaptureDATA* pDATA = reinterpret_cast<DX8CaptureDATA*>(m_captures[currentCapture]);
			if (currentCapture < NUM_BUFFERS && pDATA->GetPointer())
			{
				pDATA->Enter();
				do
				{
					if (m_dx.m_mutes[dwCurrentID].Lock(0))
					{
						ConvertPixelFormat(pDATA->GetPointer(), pDATA->GetPitch(), static_cast<D3DFORMAT>(m_d3dFormat), m_captureDATA.Size.cx, m_captureDATA.Size.cy, m_textures[dwCurrentID]);
						TextureDATA* textureDATA = m_dx.GetTextureDATA(m_captureDATA.MapSize);
						textureDATA->CurrentID = dwCurrentID;
						m_dx.m_mutes[dwCurrentID].Unlock();
						break;
					}
					if (m_dx.m_mutes[dwNextID].Lock(0))
					{
						ConvertPixelFormat(pDATA->GetPointer(), pDATA->GetPitch(), static_cast<D3DFORMAT>(m_d3dFormat), m_captureDATA.Size.cx, m_captureDATA.Size.cy, m_textures[dwNextID]);
						TextureDATA* textureDATA = m_dx.GetTextureDATA(m_captureDATA.MapSize);
						textureDATA->CurrentID = dwNextID;
						m_dx.m_mutes[dwNextID].Unlock();
						break;
					}
				} while (0);
				pDATA->SetCopying(FALSE);
				pDATA->Leave();
				dwCurrentID = dwNextID;
			}
		}
	}
}