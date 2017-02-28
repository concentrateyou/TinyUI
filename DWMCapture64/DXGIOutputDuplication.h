#pragma once
#include "Common/TinyCommon.h"
#include <dxgi1_2.h>
#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
using namespace TinyUI;

namespace DWM
{
	class DXGIOutputDuplication : public IDXGIOutputDuplication, public TinyReference < DXGIOutputDuplication >
	{
	public:
		DXGIOutputDuplication();
		virtual ~DXGIOutputDuplication();
	public:
		BOOL Initialize(ID3D11Device *device, IDXGIOutput *output);
	public:
		void STDMETHODCALLTYPE GetDesc(_Out_ DXGI_OUTDUPL_DESC *pDesc) OVERRIDE;
		HRESULT STDMETHODCALLTYPE AcquireNextFrame(_In_ UINT TimeoutInMilliseconds, _Out_ DXGI_OUTDUPL_FRAME_INFO *pFrameInfo, _Out_ IDXGIResource **ppDesktopResource) OVERRIDE;
		HRESULT STDMETHODCALLTYPE GetFrameDirtyRects(_In_ UINT DirtyRectsBufferSize, _Out_writes_bytes_to_(DirtyRectsBufferSize, *pDirtyRectsBufferSizeRequired) RECT *pDirtyRectsBuffer, _Out_ UINT *pDirtyRectsBufferSizeRequired) OVERRIDE;
		HRESULT STDMETHODCALLTYPE GetFrameMoveRects(_In_ UINT MoveRectsBufferSize, _Out_writes_bytes_to_(MoveRectsBufferSize, *pMoveRectsBufferSizeRequired) DXGI_OUTDUPL_MOVE_RECT *pMoveRectBuffer, _Out_ UINT *pMoveRectsBufferSizeRequired) OVERRIDE;
		HRESULT STDMETHODCALLTYPE GetFramePointerShape(_In_ UINT PointerShapeBufferSize, _Out_writes_bytes_to_(PointerShapeBufferSize, *pPointerShapeBufferSizeRequired) void *pPointerShapeBuffer, _Out_ UINT *pPointerShapeBufferSizeRequired, _Out_ DXGI_OUTDUPL_POINTER_SHAPE_INFO *pPointerShapeInfo) OVERRIDE;
		HRESULT STDMETHODCALLTYPE MapDesktopSurface(_Out_ DXGI_MAPPED_RECT *pLockedRect) OVERRIDE;
		HRESULT STDMETHODCALLTYPE UnMapDesktopSurface(void) OVERRIDE;
		HRESULT STDMETHODCALLTYPE ReleaseFrame(void) OVERRIDE;
		HRESULT STDMETHODCALLTYPE SetPrivateData(__in REFGUID Name, UINT DataSize, __in_bcount(DataSize) const void *pData) OVERRIDE;
		HRESULT STDMETHODCALLTYPE SetPrivateDataInterface(__in REFGUID Name, __in const IUnknown *pUnknown) OVERRIDE;
		HRESULT STDMETHODCALLTYPE GetPrivateData(__in REFGUID Name, __inout UINT *pDataSize, __out_bcount(*pDataSize) void *pData) OVERRIDE;
		HRESULT STDMETHODCALLTYPE GetParent(__in REFIID riid, __out void **ppParent) OVERRIDE;
		HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject) OVERRIDE;
		ULONG STDMETHODCALLTYPE AddRef(void) OVERRIDE;
		ULONG STDMETHODCALLTYPE Release(void) OVERRIDE;
	private:
		RECT						m_monitor;
		HANDLE						m_handle;
	};

}


