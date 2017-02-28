#include "stdafx.h"
#include "DXGIOutputDuplication.h"

namespace DWM
{
	DXGIOutputDuplication::DXGIOutputDuplication()
		:m_handle(NULL)
	{
		ZeroMemory(&m_monitor, sizeof(m_monitor));
	}

	DXGIOutputDuplication::~DXGIOutputDuplication()
	{
	}

	BOOL DXGIOutputDuplication::Initialize(ID3D11Device *device, IDXGIOutput *output)
	{
		DXGI_OUTPUT_DESC desc = { 0 };
		HRESULT hRes = output->GetDesc(&desc);
		if FAILED(hRes)
			return FALSE;
		if (!desc.AttachedToDesktop)
			return FALSE;
		m_monitor = desc.DesktopCoordinates;
		D3D11_TEXTURE2D_DESC textureDesc = { 0 };
		textureDesc.Width = TO_CX(m_monitor);
		textureDesc.Height = TO_CY(m_monitor);
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED;
		TinyComPtr<ID3D11Texture2D>	texture;
		hRes = device->CreateTexture2D(&textureDesc, NULL, &texture);
		if FAILED(hRes)
			return FALSE;
		TinyComPtr<IDXGIResource> dxgi;
		hRes = texture->QueryInterface(&dxgi);
		if FAILED(hRes)
			return FALSE;
		hRes = dxgi->GetSharedHandle(&m_handle);
		if FAILED(hRes)
			return FALSE;
		return TRUE;
	}

	void STDMETHODCALLTYPE DXGIOutputDuplication::GetDesc(_Out_ DXGI_OUTDUPL_DESC *pDesc)
	{
		pDesc->ModeDesc.Width = UINT(m_monitor.right - m_monitor.left);
		pDesc->ModeDesc.Height = UINT(m_monitor.bottom - m_monitor.top);
		pDesc->ModeDesc.RefreshRate.Numerator = 60;
		pDesc->ModeDesc.RefreshRate.Denominator = 1;
		pDesc->ModeDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		pDesc->ModeDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		pDesc->ModeDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		pDesc->Rotation = DXGI_MODE_ROTATION_UNSPECIFIED;
		pDesc->DesktopImageInSystemMemory = FALSE;
	}

	HRESULT STDMETHODCALLTYPE DXGIOutputDuplication::AcquireNextFrame(_In_ UINT TimeoutInMilliseconds, _Out_ DXGI_OUTDUPL_FRAME_INFO *pFrameInfo, _Out_ IDXGIResource **ppDesktopResource)
	{
		return TRUE;
	}

	HRESULT STDMETHODCALLTYPE DXGIOutputDuplication::GetFrameDirtyRects(_In_ UINT DirtyRectsBufferSize, _Out_writes_bytes_to_(DirtyRectsBufferSize, *pDirtyRectsBufferSizeRequired) RECT *pDirtyRectsBuffer, _Out_ UINT *pDirtyRectsBufferSizeRequired)
	{
		if (!pDirtyRectsBufferSizeRequired)
			return E_INVALIDARG;
		*pDirtyRectsBufferSizeRequired = 1;
		if (DirtyRectsBufferSize < 1)
			return DXGI_ERROR_MORE_DATA;
		if (!pDirtyRectsBuffer)
			return E_INVALIDARG;
		pDirtyRectsBuffer->left = 0;
		pDirtyRectsBuffer->top = 0;
		pDirtyRectsBuffer->right = m_monitor.right - m_monitor.left;
		pDirtyRectsBuffer->bottom = m_monitor.bottom - m_monitor.top;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE DXGIOutputDuplication::GetFrameMoveRects(_In_ UINT MoveRectsBufferSize, _Out_writes_bytes_to_(MoveRectsBufferSize, *pMoveRectsBufferSizeRequired) DXGI_OUTDUPL_MOVE_RECT *pMoveRectBuffer, _Out_ UINT *pMoveRectsBufferSizeRequired)
	{
		UNUSED(MoveRectsBufferSize);
		UNUSED(pMoveRectBuffer);
		if (!pMoveRectsBufferSizeRequired)
			return E_INVALIDARG;
		*pMoveRectsBufferSizeRequired = 0;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE DXGIOutputDuplication::GetFramePointerShape(_In_ UINT PointerShapeBufferSize, _Out_writes_bytes_to_(PointerShapeBufferSize, *pPointerShapeBufferSizeRequired) void *pPointerShapeBuffer, _Out_ UINT *pPointerShapeBufferSizeRequired, _Out_ DXGI_OUTDUPL_POINTER_SHAPE_INFO *pPointerShapeInfo)
	{
		return S_FALSE;
	}

	HRESULT STDMETHODCALLTYPE DXGIOutputDuplication::MapDesktopSurface(_Out_ DXGI_MAPPED_RECT *pLockedRect)
	{
		return S_FALSE;
	}

	HRESULT STDMETHODCALLTYPE DXGIOutputDuplication::UnMapDesktopSurface(void)
	{
		return S_FALSE;
	}

	HRESULT STDMETHODCALLTYPE DXGIOutputDuplication::ReleaseFrame(void)
	{
		return S_FALSE;
	}
	HRESULT STDMETHODCALLTYPE DXGIOutputDuplication::QueryInterface(REFIID riid, void **ppvObject)
	{
		if (IsEqualIID(riid, IID_IDXGIOutputDuplication) || IsEqualIID(riid, IID_IUnknown))
		{
			*ppvObject = static_cast<IDXGIOutputDuplication*>(this);
		}
		else
		{
			*ppvObject = NULL;
			return E_NOINTERFACE;
		}
		AddRef();
		return NOERROR;
	}

	ULONG STDMETHODCALLTYPE DXGIOutputDuplication::AddRef(void)
	{
		TinyReference < DXGIOutputDuplication >::AddRef();
		return TinyReference < DXGIOutputDuplication >::GetReference();
	}

	ULONG STDMETHODCALLTYPE DXGIOutputDuplication::Release(void)
	{
		TinyReference < DXGIOutputDuplication >::Release();
		return TinyReference < DXGIOutputDuplication >::GetReference();
	}

}
