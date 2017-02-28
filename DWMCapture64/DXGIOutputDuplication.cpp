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
		DXGI_OUTPUT_DESC desc;
		HRESULT hRes = output->GetDesc(&desc);
		if FAILED(hRes)
			return FALSE;
		if (!desc.AttachedToDesktop)
			return FALSE;
		m_monitor = desc.DesktopCoordinates;
		D3D11_TEXTURE2D_DESC desc;
		desc.Width = TO_CX(m_monitor);
		desc.Height = TO_CY(m_monitor);
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		desc.SampleDesc;
		desc.Count = 1;
		desc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = D3D11_RESOURCE_MISC_SHARED;
		TinyComPtr<ID3D11Texture2D>	texture;
		hRes = device->CreateTexture2D(&desc, NULL, &texture);
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
		pDesc->ModeDesc.RefreshRate.Numerator = 60; /*FIXME: assume 60.0Hz */
		pDesc->ModeDesc.RefreshRate.Denominator = 1;
		pDesc->ModeDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		pDesc->ModeDesc.ScanlineOrdering = /*FIXME*/DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		pDesc->ModeDesc.Scaling = /*FIXME*/DXGI_MODE_SCALING_UNSPECIFIED;
		pDesc->Rotation = DXGI_MODE_ROTATION_UNSPECIFIED;
		pDesc->DesktopImageInSystemMemory = FALSE;
	}

	HRESULT STDMETHODCALLTYPE DXGIOutputDuplication::AcquireNextFrame(_In_ UINT TimeoutInMilliseconds, _Out_ DXGI_OUTDUPL_FRAME_INFO *pFrameInfo, _Out_ IDXGIResource **ppDesktopResource)
	{
		throw std::logic_error("The method or operation is not implemented.");
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
		throw std::logic_error("The method or operation is not implemented.");
	}

	HRESULT STDMETHODCALLTYPE DXGIOutputDuplication::MapDesktopSurface(_Out_ DXGI_MAPPED_RECT *pLockedRect)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	HRESULT STDMETHODCALLTYPE DXGIOutputDuplication::UnMapDesktopSurface(void)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	HRESULT STDMETHODCALLTYPE DXGIOutputDuplication::ReleaseFrame(void)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	HRESULT STDMETHODCALLTYPE DXGIOutputDuplication::SetPrivateData(__in REFGUID Name, UINT DataSize, __in_bcount(DataSize) const void *pData)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE DXGIOutputDuplication::SetPrivateData(_In_ REFGUID Name, UINT DataSize, _In_reads_bytes_(DataSize) const void *pData)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE DXGIOutputDuplication::SetPrivateDataInterface(__in REFGUID Name, __in const IUnknown *pUnknown)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE DXGIOutputDuplication::SetPrivateDataInterface(_In_ REFGUID Name, _In_ const IUnknown *pUnknown)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE DXGIOutputDuplication::GetPrivateData(__in REFGUID Name, __inout UINT *pDataSize, __out_bcount(*pDataSize) void *pData)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE DXGIOutputDuplication::GetPrivateData(_In_ REFGUID Name, _Inout_ UINT *pDataSize, _Out_writes_bytes_(*pDataSize) void *pData)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE DXGIOutputDuplication::GetParent(__in REFIID riid, __out void **ppParent)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE DXGIOutputDuplication::GetParent(_In_ REFIID riid, _Out_ void **ppParent)
	{
		return E_NOTIMPL;
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
