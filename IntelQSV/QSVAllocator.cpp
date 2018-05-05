#include "stdafx.h"
#include "QSVAllocator.h"
#include <memory.h>
#include <algorithm>
#include <iterator>
#include <iostream>

namespace QSV
{
#define ID_BUFFER MFX_MAKEFOURCC('B','U','F','F')
#define ID_FRAME  MFX_MAKEFOURCC('F','R','M','E')
	template <typename T>
	struct sequence
	{
		T x;
		sequence(T seed) : x(seed) { }
	};
	template <>
	struct sequence<mfxHDL>
	{
		mfxHDL x;
		sequence(mfxHDL seed) : x(seed) { }

		mfxHDL operator ()()
		{
			mfxHDL y = x;
			x = (mfxHDL)(1 + (size_t)(x));
			return y;
		}
	};
	mfxFrameAllocResponseEx::mfxFrameAllocResponseEx()
		:Width(0),
		Height(0),
		RefCount(1),
		Type(0)
	{

	}
	mfxFrameAllocResponseEx::mfxFrameAllocResponseEx(const mfxFrameAllocResponse &response, mfxU16 width, mfxU16 height, mfxU16 type)
		: mfxFrameAllocResponse(response),
		Width(width),
		Height(height),
		RefCount(1),
		Type(type)
	{
	}
	BOOL mfxFrameAllocResponseEx::operator () (const mfxFrameAllocResponseEx &response)const
	{
		return Width <= response.Width && Height <= response.Height && (Type & response.Type)&(MFX_MEMTYPE_FROM_DECODE | MFX_MEMTYPE_FROM_ENC | MFX_MEMTYPE_FROM_PAK);
	}
	//////////////////////////////////////////////////////////////////////////
	QSVBufferAllocatorBase::QSVBufferAllocatorBase()
	{
		pthis = this;
		Alloc = Alloc_;
		Lock = Lock_;
		Free = Free_;
		Unlock = Unlock_;
	}
	QSVBufferAllocatorBase::~QSVBufferAllocatorBase()
	{
	}
	mfxStatus QSVBufferAllocatorBase::Alloc_(mfxHDL pthis, mfxU32 nbytes, mfxU16 type, mfxMemId *mid)
	{
		if (NULL == pthis)
			return MFX_ERR_MEMORY_ALLOC;
		QSVBufferAllocatorBase& self = *(QSVBufferAllocatorBase *)pthis;
		return self.AllocBuffer(nbytes, type, mid);
	}
	mfxStatus QSVBufferAllocatorBase::Lock_(mfxHDL pthis, mfxMemId mid, mfxU8 **ptr)
	{
		if (NULL == pthis)
			return MFX_ERR_MEMORY_ALLOC;
		QSVBufferAllocatorBase& self = *(QSVBufferAllocatorBase *)pthis;
		return self.LockBuffer(mid, ptr);
	}
	mfxStatus QSVBufferAllocatorBase::Unlock_(mfxHDL pthis, mfxMemId mid)
	{
		if (NULL == pthis)
			return MFX_ERR_MEMORY_ALLOC;
		QSVBufferAllocatorBase& self = *(QSVBufferAllocatorBase *)pthis;
		return self.UnlockBuffer(mid);
	}
	mfxStatus QSVBufferAllocatorBase::Free_(mfxHDL pthis, mfxMemId mid)
	{
		if (NULL == pthis)
			return MFX_ERR_MEMORY_ALLOC;
		QSVBufferAllocatorBase& self = *(QSVBufferAllocatorBase *)pthis;
		return self.FreeBuffer(mid);
	}
	//////////////////////////////////////////////////////////////////////////
	QSVBufferAllocator::QSVBufferAllocator()
	{

	}

	QSVBufferAllocator::~QSVBufferAllocator()
	{

	}

	mfxStatus QSVBufferAllocator::AllocBuffer(mfxU32 nbytes, mfxU16 type, mfxMemId *mid)
	{
		if (!mid)
			return MFX_ERR_NULL_PTR;
		if (0 == (type & MFX_MEMTYPE_SYSTEM_MEMORY))
			return MFX_ERR_UNSUPPORTED;
		mfxU32 header_size = MSDK_ALIGN32(sizeof(sBuffer));
		mfxU8 *bufferPtr = (mfxU8 *)calloc(header_size + nbytes + 32, 1);
		if (!bufferPtr)
			return MFX_ERR_MEMORY_ALLOC;
		sBuffer *bs = (sBuffer *)bufferPtr;
		bs->id = ID_BUFFER;
		bs->type = type;
		bs->nbytes = nbytes;
		*mid = (mfxHDL)bs;
		return MFX_ERR_NONE;
	}

	mfxStatus QSVBufferAllocator::LockBuffer(mfxMemId mid, mfxU8 **ptr)
	{
		if (!ptr)
			return MFX_ERR_NULL_PTR;
		sBuffer *bs = (sBuffer *)mid;
		if (!bs)
			return MFX_ERR_INVALID_HANDLE;
		if (ID_BUFFER != bs->id)
			return MFX_ERR_INVALID_HANDLE;
		*ptr = (mfxU8*)((size_t)((mfxU8 *)bs + MSDK_ALIGN32(sizeof(sBuffer)) + 31)&(~((size_t)31)));
		return MFX_ERR_NONE;
	}

	mfxStatus QSVBufferAllocator::UnlockBuffer(mfxMemId mid)
	{
		sBuffer *bs = (sBuffer *)mid;
		if (!bs || ID_BUFFER != bs->id)
			return MFX_ERR_INVALID_HANDLE;
		return MFX_ERR_NONE;
	}

	mfxStatus QSVBufferAllocator::FreeBuffer(mfxMemId mid)
	{
		sBuffer *bs = (sBuffer *)mid;
		if (!bs || ID_BUFFER != bs->id)
			return MFX_ERR_INVALID_HANDLE;
		SAFE_FREE(bs);
		return MFX_ERR_NONE;
	}

	//////////////////////////////////////////////////////////////////////////
	QSVAllocatorBase::QSVAllocatorBase()
	{
		pthis = this;
		Alloc = Alloc_;
		Lock = Lock_;
		Free = Free_;
		Unlock = Unlock_;
		GetHDL = GetHDL_;
	}
	QSVAllocatorBase::~QSVAllocatorBase()
	{
	}
	mfxStatus QSVAllocatorBase::Alloc_(mfxHDL pthis, mfxFrameAllocRequest *request, mfxFrameAllocResponse *response)
	{
		if (NULL == pthis)
			return MFX_ERR_MEMORY_ALLOC;
		QSVAllocatorBase& self = *(QSVAllocatorBase *)pthis;
		return self.AllocFrames(request, response);
	}
	mfxStatus QSVAllocatorBase::Lock_(mfxHDL pthis, mfxMemId mid, mfxFrameData *ptr)
	{
		if (NULL == pthis)
			return MFX_ERR_MEMORY_ALLOC;
		QSVAllocatorBase& self = *(QSVAllocatorBase *)pthis;
		return self.LockFrame(mid, ptr);
	}
	mfxStatus QSVAllocatorBase::Unlock_(mfxHDL pthis, mfxMemId mid, mfxFrameData *ptr)
	{
		if (NULL == pthis)
			return MFX_ERR_MEMORY_ALLOC;
		QSVAllocatorBase& self = *(QSVAllocatorBase *)pthis;
		return self.UnlockFrame(mid, ptr);
	}
	mfxStatus QSVAllocatorBase::GetHDL_(mfxHDL pthis, mfxMemId mid, mfxHDL *handle)
	{
		if (NULL == pthis)
			return MFX_ERR_MEMORY_ALLOC;
		QSVAllocatorBase& self = *(QSVAllocatorBase *)pthis;
		return self.GetFrameHDL(mid, handle);
	}
	mfxStatus QSVAllocatorBase::Free_(mfxHDL pthis, mfxFrameAllocResponse *response)
	{
		if (NULL == pthis)
			return MFX_ERR_MEMORY_ALLOC;
		QSVAllocatorBase& self = *(QSVAllocatorBase *)pthis;
		return self.FreeFrames(response);
	}
	////////////////////////////////////////////////////////////////////////
	QSVAllocator::QSVAllocator()
	{

	}
	QSVAllocator::~QSVAllocator()
	{

	}
	mfxStatus QSVAllocator::Close()
	{
		ITERATOR s = m_responsexs.First();
		while (s != NULL)
		{
			mfxFrameAllocResponseEx& resex = m_responsexs.GetAt(s);
			Deallocate(&resex);
			s = m_responsexs.Next(s);
		}
		m_responsexs.RemoveAll();
		s = m_responses.First();
		while (s != NULL)
		{
			mfxFrameAllocResponse& res = m_responses.GetAt(s);
			Deallocate(&res);
			s = m_responses.Next(s);
		}
		m_responses.RemoveAll();
		return MFX_ERR_NONE;
	}
	mfxStatus QSVAllocator::AllocFrames(mfxFrameAllocRequest *request, mfxFrameAllocResponse *response)
	{
		if (NULL == request || 0 == response || 0 == request->NumFrameSuggested)
			return MFX_ERR_MEMORY_ALLOC;
		if (MSDK_FAILED(CheckRequestType(request)))
			return MFX_ERR_UNSUPPORTED;
		mfxStatus status = MFX_ERR_NONE;
		if ((request->Type & MFX_MEMTYPE_EXTERNAL_FRAME) && ((request->Type & MFX_MEMTYPE_FROM_DECODE) || (request->Type & MFX_MEMTYPE_FROM_ENC) || (request->Type & MFX_MEMTYPE_FROM_PAK)))
		{
			BOOL bFlag = FALSE;
			mfxFrameAllocResponseEx checker(*response, request->Info.Width, request->Info.Height, request->Type);
			ITERATOR s = m_responsexs.First();
			while (s != NULL)
			{
				mfxFrameAllocResponseEx& resex = m_responsexs.GetAt(s);
				if (request->AllocId == resex.AllocId && checker(resex))
				{
					if (request->NumFrameSuggested > resex.NumFrameActual)
						return MFX_ERR_MEMORY_ALLOC;
					resex.RefCount++;
					*response = (mfxFrameAllocResponse&)resex;
					bFlag = TRUE;
					break;
				}
				s = m_responsexs.Next(s);
			}
			if (!bFlag)
			{
				status = Allocate(request, response);
				if (status == MFX_ERR_NONE)
				{
					response->AllocId = request->AllocId;
					m_responsexs.InsertLast(mfxFrameAllocResponseEx(*response, request->Info.Width, request->Info.Height, request->Type & MEMTYPE_FROM_MASK));
				}
			}
		}
		else
		{
			status = Allocate(request, response);
			if (MSDK_SUCCEEDED(status))
			{
				m_responses.InsertLast(*response);
			}
		}
		return status;
	}
	mfxStatus QSVAllocator::FreeFrames(mfxFrameAllocResponse *response)
	{
		if (response == 0)
			return MFX_ERR_INVALID_HANDLE;
		mfxStatus status = MFX_ERR_NONE;
		ITERATOR s = m_responsexs.First();
		while (s != NULL)
		{
			mfxFrameAllocResponseEx& resex = m_responsexs.GetAt(s);
			if (resex.mids != NULL &&
				response->mids != NULL &&
				resex.mids[0] == response->mids[0] &&
				resex.NumFrameActual == response->NumFrameActual)
			{
				if ((--resex.RefCount) == 0)
				{
					status = Deallocate(response);
					m_responsexs.RemoveAt(s);
					return status;
				}
			}
			s = m_responsexs.Next(s);
		}
		s = m_responses.First();
		while (s != NULL)
		{
			mfxFrameAllocResponse& res = m_responses.GetAt(s);
			if (res.mids != NULL &&
				response->mids != NULL &&
				res.mids[0] == response->mids[0] &&
				res.NumFrameActual == response->NumFrameActual)
			{
				status = Deallocate(response);
				m_responses.RemoveAt(s);
				return status;
			}
			s = m_responses.Next(s);
		}
		return MFX_ERR_INVALID_HANDLE;
	}
	mfxStatus QSVAllocator::CheckRequestType(mfxFrameAllocRequest *request)
	{
		if (NULL == request)
			return MFX_ERR_NULL_PTR;
		if ((request->Type & MEMTYPE_FROM_MASK) != 0)
			return MFX_ERR_NONE;
		else
			return MFX_ERR_UNSUPPORTED;
	}
	//////////////////////////////////////////////////////////////////////////
	QSVD3D9Allocator::QSVD3D9Allocator()
		:m_hProcessor(NULL),
		m_hDecoder(NULL),
		m_dwUsage(0)
	{

	}
	QSVD3D9Allocator::~QSVD3D9Allocator()
	{

	}
	mfxStatus QSVD3D9Allocator::Initialize(mfxAllocatorParams *pParams)
	{
		QSVD3D9AllocatorParams *d3dParams = NULL;
		d3dParams = dynamic_cast<QSVD3D9AllocatorParams *>(pParams);
		if (!d3dParams)
			return MFX_ERR_NOT_INITIALIZED;
		m_manager = d3dParams->pManager;
		m_dwUsage = d3dParams->dwUsage;
		return MFX_ERR_NONE;
	}
	mfxStatus QSVD3D9Allocator::Close()
	{
		if (m_manager != NULL)
		{
			if (m_hDecoder != NULL)
			{
				m_manager->CloseDeviceHandle(m_hDecoder);
				m_hDecoder = NULL;
			}
			if (m_hProcessor != NULL)
			{
				m_manager->CloseDeviceHandle(m_hProcessor);
				m_hProcessor = NULL;
			}
			m_manager = NULL;
		}
		return QSVAllocator::Close();
	}
	mfxStatus QSVD3D9Allocator::CheckRequestType(mfxFrameAllocRequest *request)
	{
		mfxStatus status = QSVAllocator::CheckRequestType(request);
		if (MFX_ERR_NONE != status)
			return status;
		if ((request->Type & (MFX_MEMTYPE_VIDEO_MEMORY_DECODER_TARGET | MFX_MEMTYPE_VIDEO_MEMORY_PROCESSOR_TARGET)) != 0)
			return MFX_ERR_NONE;
		else
			return MFX_ERR_UNSUPPORTED;
	}
	mfxStatus QSVD3D9Allocator::Allocate(mfxFrameAllocRequest *request, mfxFrameAllocResponse *response)
	{
		HRESULT hRes = S_OK;
		MSDK_CHECK_POINTER(request, MFX_ERR_NULL_PTR);
		if (request->NumFrameSuggested == 0)
			return MFX_ERR_UNKNOWN;
		D3DFORMAT d3dFormat = ConvertMfxFourccToD3dFormat(request->Info.FourCC);
		if (d3dFormat == D3DFMT_UNKNOWN)
			return MFX_ERR_UNSUPPORTED;
		DWORD  dwTarget = 0;
		if (MFX_MEMTYPE_DXVA2_DECODER_TARGET & request->Type)
		{
			dwTarget = DXVA2_VideoDecoderRenderTarget;
		}
		else if (MFX_MEMTYPE_DXVA2_PROCESSOR_TARGET & request->Type)
		{
			dwTarget = DXVA2_VideoProcessorRenderTarget;
		}
		else
			return MFX_ERR_UNSUPPORTED;
		IDirectXVideoAccelerationService* videoService = NULL;
		if (dwTarget == DXVA2_VideoProcessorRenderTarget)
		{
			if (!m_hProcessor)
			{
				hRes = m_manager->OpenDeviceHandle(&m_hProcessor);
				if (FAILED(hRes))
					return MFX_ERR_MEMORY_ALLOC;
				hRes = m_manager->GetVideoService(m_hProcessor, __uuidof(IDirectXVideoProcessorService), (void**)&m_processorService);
				if (FAILED(hRes))
					return MFX_ERR_MEMORY_ALLOC;
			}
			videoService = m_processorService;
		}
		else
		{
			if (!m_hDecoder)
			{
				hRes = m_manager->OpenDeviceHandle(&m_hDecoder);
				if (FAILED(hRes))
					return MFX_ERR_MEMORY_ALLOC;
				hRes = m_manager->GetVideoService(m_hDecoder, __uuidof(IDirectXVideoDecoderService), (void**)&m_decoderService);
				if (FAILED(hRes))
					return MFX_ERR_MEMORY_ALLOC;
			}
			videoService = m_decoderService;
		}
		mfxHDLPair **dxMidPtrs = (mfxHDLPair**)calloc(request->NumFrameSuggested, sizeof(mfxHDLPair*));
		if (!dxMidPtrs)
			return MFX_ERR_MEMORY_ALLOC;
		for (INT i = 0; i < request->NumFrameSuggested; i++)
		{
			dxMidPtrs[i] = (mfxHDLPair*)calloc(1, sizeof(mfxHDLPair));
			if (!dxMidPtrs[i])
			{
				DeallocateMids(dxMidPtrs, i);
				return MFX_ERR_MEMORY_ALLOC;
			}
		}
		response->mids = (mfxMemId*)dxMidPtrs;
		response->NumFrameActual = request->NumFrameSuggested;
		if (request->Type & MFX_MEMTYPE_EXTERNAL_FRAME)
		{
			for (INT i = 0; i < request->NumFrameSuggested; i++)
			{
				hRes = videoService->CreateSurface(request->Info.Width, request->Info.Height, 0, d3dFormat, D3DPOOL_DEFAULT, m_dwUsage, dwTarget, (IDirect3DSurface9**)&dxMidPtrs[i]->first, &dxMidPtrs[i]->second);
				//hRes = videoService->CreateSurface(request->Info.Width, request->Info.Height, 0, d3dFormat, D3DPOOL_DEFAULT, m_dwUsage, dwTarget, (IDirect3DSurface9**)&dxMidPtrs[i]->first, NULL);
				if (FAILED(hRes))
				{
					Deallocate(response);
					return MFX_ERR_MEMORY_ALLOC;
				}
			}
		}
		else
		{
			TinyScopedArray<IDirect3DSurface9*> surfaces(new IDirect3DSurface9*[request->NumFrameSuggested]);
			if (!surfaces)
			{
				DeallocateMids(dxMidPtrs, request->NumFrameSuggested);
				return MFX_ERR_MEMORY_ALLOC;
			}
			hRes = videoService->CreateSurface(request->Info.Width, request->Info.Height, request->NumFrameSuggested - 1, d3dFormat, D3DPOOL_DEFAULT, m_dwUsage, dwTarget, surfaces, NULL);
			if (FAILED(hRes))
			{
				DeallocateMids(dxMidPtrs, request->NumFrameSuggested);
				return MFX_ERR_MEMORY_ALLOC;
			}
			for (INT i = 0; i < request->NumFrameSuggested; i++)
			{
				dxMidPtrs[i]->first = surfaces[i];
			}
		}
		return MFX_ERR_NONE;
	}
	void QSVD3D9Allocator::DeallocateMids(mfxHDLPair** pair, INT size)
	{
		for (INT i = 0; i < size; i++)
		{
			SAFE_FREE(pair[i]);
		}
		SAFE_FREE(pair);
	}
	mfxStatus QSVD3D9Allocator::Deallocate(mfxFrameAllocResponse *response)
	{
		if (!response)
			return MFX_ERR_NULL_PTR;
		mfxStatus ststus = MFX_ERR_NONE;
		if (response->mids)
		{
			for (mfxU32 i = 0; i < response->NumFrameActual; i++)
			{
				if (response->mids[i])
				{
					mfxHDLPair *dxMids = (mfxHDLPair*)response->mids[i];
					static_cast<IDirect3DSurface9*>(dxMids->first)->Release();
					SAFE_FREE(dxMids);
				}
			}
			SAFE_FREE(response->mids);
		}
		return ststus;
	}
	mfxStatus QSVD3D9Allocator::LockFrame(mfxMemId mid, mfxFrameData *ptr)
	{
		if (!ptr || !mid)
			return MFX_ERR_NULL_PTR;
		mfxHDLPair *dxmid = (mfxHDLPair*)mid;
		IDirect3DSurface9 *pSurface = static_cast<IDirect3DSurface9*>(dxmid->first);
		if (pSurface == NULL)
			return MFX_ERR_INVALID_HANDLE;
		D3DSURFACE_DESC desc;
		HRESULT hRes = pSurface->GetDesc(&desc);
		if (FAILED(hRes))
			return MFX_ERR_LOCK_MEMORY;
		if (desc.Format != D3DFMT_NV12 &&
			desc.Format != D3DFMT_YV12 &&
			desc.Format != D3DFMT_YUY2 &&
			desc.Format != D3DFMT_R8G8B8 &&
			desc.Format != D3DFMT_A8R8G8B8 &&
			desc.Format != D3DFMT_P8 &&
			desc.Format != D3DFMT_P010 &&
			desc.Format != D3DFMT_A2R10G10B10 &&
			desc.Format != D3DFMT_A16B16G16R16 &&
			desc.Format != D3DFMT_IMC3 &&
			desc.Format != D3DFMT_AYUV)
			return MFX_ERR_LOCK_MEMORY;
		D3DLOCKED_RECT locked;
		hRes = pSurface->LockRect(&locked, 0, D3DLOCK_NOSYSLOCK);
		if (FAILED(hRes))
			return MFX_ERR_LOCK_MEMORY;
		switch ((DWORD)desc.Format)
		{
		case D3DFMT_NV12:
		case D3DFMT_P010:
			ptr->Pitch = (mfxU16)locked.Pitch;
			ptr->Y = (mfxU8 *)locked.pBits;
			ptr->U = (mfxU8 *)locked.pBits + desc.Height * locked.Pitch;
			ptr->V = (desc.Format == D3DFMT_P010) ? ptr->U + 2 : ptr->U + 1;
			break;
		case D3DFMT_YV12:
			ptr->Pitch = (mfxU16)locked.Pitch;
			ptr->Y = (mfxU8 *)locked.pBits;
			ptr->V = ptr->Y + desc.Height * locked.Pitch;
			ptr->U = ptr->V + (desc.Height * locked.Pitch) / 4;
			break;
		case D3DFMT_YUY2:
			ptr->Pitch = (mfxU16)locked.Pitch;
			ptr->Y = (mfxU8 *)locked.pBits;
			ptr->U = ptr->Y + 1;
			ptr->V = ptr->Y + 3;
			break;
		case D3DFMT_R8G8B8:
			ptr->Pitch = (mfxU16)locked.Pitch;
			ptr->B = (mfxU8 *)locked.pBits;
			ptr->G = ptr->B + 1;
			ptr->R = ptr->B + 2;
			break;
		case D3DFMT_A8R8G8B8:
		case D3DFMT_A2R10G10B10:
			ptr->Pitch = (mfxU16)locked.Pitch;
			ptr->B = (mfxU8 *)locked.pBits;
			ptr->G = ptr->B + 1;
			ptr->R = ptr->B + 2;
			ptr->A = ptr->B + 3;
			break;
		case D3DFMT_P8:
			ptr->Pitch = (mfxU16)locked.Pitch;
			ptr->Y = (mfxU8 *)locked.pBits;
			ptr->U = 0;
			ptr->V = 0;
			break;
		case D3DFMT_A16B16G16R16:
			ptr->V16 = (mfxU16*)locked.pBits;
			ptr->U16 = ptr->V16 + 1;
			ptr->Y16 = ptr->V16 + 2;
			ptr->A = (mfxU8*)(ptr->V16 + 3);
			ptr->PitchHigh = (mfxU16)((mfxU32)locked.Pitch / (1 << 16));
			ptr->PitchLow = (mfxU16)((mfxU32)locked.Pitch % (1 << 16));
			break;
		case D3DFMT_IMC3:
			ptr->Pitch = (mfxU16)locked.Pitch;
			ptr->Y = (mfxU8 *)locked.pBits;
			ptr->V = ptr->Y + desc.Height * locked.Pitch;
			ptr->U = ptr->Y + desc.Height * locked.Pitch * 2;
			break;
		case D3DFMT_AYUV:
			ptr->Pitch = (mfxU16)locked.Pitch;
			ptr->V = (mfxU8 *)locked.pBits;
			ptr->U = ptr->V + 1;
			ptr->Y = ptr->V + 2;
			ptr->A = ptr->V + 3;
			break;
		}
		return MFX_ERR_NONE;
	}
	mfxStatus QSVD3D9Allocator::UnlockFrame(mfxMemId mid, mfxFrameData *ptr)
	{
		if (!mid)
			return MFX_ERR_NULL_PTR;
		mfxHDLPair *dxmid = (mfxHDLPair*)mid;
		IDirect3DSurface9 *pSurface = static_cast<IDirect3DSurface9*>(dxmid->first);
		if (pSurface == 0)
			return MFX_ERR_INVALID_HANDLE;
		pSurface->UnlockRect();
		if (NULL != ptr)
		{
			ptr->Pitch = 0;
			ptr->Y = 0;
			ptr->U = 0;
			ptr->V = 0;
		}
		return MFX_ERR_NONE;
	}
	mfxStatus QSVD3D9Allocator::GetFrameHDL(mfxMemId mid, mfxHDL *handle)
	{
		if (!mid || !handle)
			return MFX_ERR_NULL_PTR;
		mfxHDLPair *dxMid = (mfxHDLPair*)mid;
		*handle = dxMid->first;
		return MFX_ERR_NONE;
	}
	//////////////////////////////////////////////////////////////////////////
	QSVD3D11Allocator::QSVD3D11Allocator()
	{

	}
	QSVD3D11Allocator::~QSVD3D11Allocator()
	{

	}
	mfxStatus QSVD3D11Allocator::Initialize(mfxAllocatorParams *pParams)
	{
		QSVD3D11AllocatorParams *d3dParams = dynamic_cast<QSVD3D11AllocatorParams*>(pParams);
		if (NULL == d3dParams || NULL == d3dParams->pDevice)
		{
			return MFX_ERR_NOT_INITIALIZED;
		}
		m_initParams = *d3dParams;
		m_deviceContext.Release();
		d3dParams->pDevice->GetImmediateContext(&m_deviceContext);
		return MFX_ERR_NONE;
	}
	mfxStatus QSVD3D11Allocator::Close()
	{
		mfxStatus status = QSVAllocator::Close();
		for (referenceType s = m_resourcesByRequest.begin(); s != m_resourcesByRequest.end(); s++)
		{
			s->Release();
		}
		m_resourcesByRequest.clear();
		m_memIDMap.clear();
		m_deviceContext.Release();
		return status;
	}
	mfxStatus QSVD3D11Allocator::LockFrame(mfxMemId mid, mfxFrameData *ptr)
	{
		HRESULT hRes = S_OK;

		D3D11_TEXTURE2D_DESC desc = { 0 };
		D3D11_MAPPED_SUBRESOURCE lockedRect = { 0 };
		TextureSubResource sr = GetResourceFromMID(mid);
		if (!sr.GetTexture())
			return MFX_ERR_LOCK_MEMORY;
		D3D11_MAP mapType = D3D11_MAP_READ;
		UINT mapFlags = D3D11_MAP_FLAG_DO_NOT_WAIT;
		{
			if (NULL == sr.GetStaging())
			{
				hRes = m_deviceContext->Map(sr.GetTexture(), sr.GetSubResource(), D3D11_MAP_READ, D3D11_MAP_FLAG_DO_NOT_WAIT, &lockedRect);
				desc.Format = DXGI_FORMAT_P8;
			}
			else
			{
				sr.GetTexture()->GetDesc(&desc);
				if (DXGI_FORMAT_NV12 != desc.Format &&
					DXGI_FORMAT_420_OPAQUE != desc.Format &&
					DXGI_FORMAT_YUY2 != desc.Format &&
					DXGI_FORMAT_P8 != desc.Format &&
					DXGI_FORMAT_B8G8R8A8_UNORM != desc.Format &&
					DXGI_FORMAT_R16_UINT != desc.Format &&
					DXGI_FORMAT_R16_UNORM != desc.Format &&
					DXGI_FORMAT_R10G10B10A2_UNORM != desc.Format &&
					DXGI_FORMAT_R16G16B16A16_UNORM != desc.Format &&
					DXGI_FORMAT_P010 != desc.Format &&
					DXGI_FORMAT_AYUV != desc.Format)
				{
					return MFX_ERR_LOCK_MEMORY;
				}
				if (MFXReadWriteMid(mid, MFXReadWriteMid::reuse).IsRead())
				{
					m_deviceContext->CopySubresourceRegion(sr.GetStaging(), 0, 0, 0, 0, sr.GetTexture(), sr.GetSubResource(), NULL);
				}
				do
				{
					hRes = m_deviceContext->Map(sr.GetStaging(), 0, mapType, mapFlags, &lockedRect);
					if (S_OK != hRes && DXGI_ERROR_WAS_STILL_DRAWING != hRes)
						break;
				} while (DXGI_ERROR_WAS_STILL_DRAWING == hRes);
			}
		}
		if (FAILED(hRes))
			return MFX_ERR_LOCK_MEMORY;
		switch (desc.Format)
		{
		case DXGI_FORMAT_P010:
		case DXGI_FORMAT_NV12:
			ptr->Pitch = (mfxU16)lockedRect.RowPitch;
			ptr->Y = (mfxU8 *)lockedRect.pData;
			ptr->U = (mfxU8 *)lockedRect.pData + desc.Height * lockedRect.RowPitch;
			ptr->V = (desc.Format == DXGI_FORMAT_P010) ? ptr->U + 2 : ptr->U + 1;
			break;

		case DXGI_FORMAT_420_OPAQUE:
			ptr->Pitch = (mfxU16)lockedRect.RowPitch;
			ptr->Y = (mfxU8 *)lockedRect.pData;
			ptr->V = ptr->Y + desc.Height * lockedRect.RowPitch;
			ptr->U = ptr->V + (desc.Height * lockedRect.RowPitch) / 4;
			break;
		case DXGI_FORMAT_YUY2:
			ptr->Pitch = (mfxU16)lockedRect.RowPitch;
			ptr->Y = (mfxU8 *)lockedRect.pData;
			ptr->U = ptr->Y + 1;
			ptr->V = ptr->Y + 3;
			break;
		case DXGI_FORMAT_P8:
			ptr->Pitch = (mfxU16)lockedRect.RowPitch;
			ptr->Y = (mfxU8 *)lockedRect.pData;
			ptr->U = 0;
			ptr->V = 0;
			break;
		case DXGI_FORMAT_AYUV:
		case DXGI_FORMAT_B8G8R8A8_UNORM:
			ptr->Pitch = (mfxU16)lockedRect.RowPitch;
			ptr->B = (mfxU8 *)lockedRect.pData;
			ptr->G = ptr->B + 1;
			ptr->R = ptr->B + 2;
			ptr->A = ptr->B + 3;
			break;
		case DXGI_FORMAT_R10G10B10A2_UNORM:
			ptr->Pitch = (mfxU16)lockedRect.RowPitch;
			ptr->B = (mfxU8 *)lockedRect.pData;
			ptr->G = ptr->B + 1;
			ptr->R = ptr->B + 2;
			ptr->A = ptr->B + 3;
			break;
		case DXGI_FORMAT_R16G16B16A16_UNORM:
			ptr->V16 = (mfxU16*)lockedRect.pData;
			ptr->U16 = ptr->V16 + 1;
			ptr->Y16 = ptr->V16 + 2;
			ptr->A = (mfxU8*)(ptr->V16 + 3);
			ptr->PitchHigh = (mfxU16)((mfxU32)lockedRect.RowPitch / (1 << 16));
			ptr->PitchLow = (mfxU16)((mfxU32)lockedRect.RowPitch % (1 << 16));
			break;
		case DXGI_FORMAT_R16_UNORM:
		case DXGI_FORMAT_R16_UINT:
			ptr->Pitch = (mfxU16)lockedRect.RowPitch;
			ptr->Y16 = (mfxU16 *)lockedRect.pData;
			ptr->U16 = 0;
			ptr->V16 = 0;
			break;
		default:
			return MFX_ERR_LOCK_MEMORY;
		}
		return MFX_ERR_NONE;
	}
	mfxStatus QSVD3D11Allocator::GetFrameHDL(mfxMemId mid, mfxHDL *handle)
	{
		if (NULL == handle)
			return MFX_ERR_INVALID_HANDLE;
		TextureSubResource sr = GetResourceFromMID(mid);
		if (!sr.GetTexture())
			return MFX_ERR_INVALID_HANDLE;
		mfxHDLPair *pPair = (mfxHDLPair*)handle;
		pPair->first = sr.GetTexture();
		pPair->second = (mfxHDL)(UINT_PTR)sr.GetSubResource();
		return MFX_ERR_NONE;
	}
	mfxStatus QSVD3D11Allocator::UnlockFrame(mfxMemId mid, mfxFrameData *ptr)
	{
		TextureSubResource sr = GetResourceFromMID(mid);
		if (!sr.GetTexture())
			return MFX_ERR_LOCK_MEMORY;
		if (NULL == sr.GetStaging())
		{
			m_deviceContext->Unmap(sr.GetTexture(), sr.GetSubResource());
		}
		else
		{
			m_deviceContext->Unmap(sr.GetStaging(), 0);
			if (MFXReadWriteMid(mid, MFXReadWriteMid::reuse).IsWrite())
			{
				m_deviceContext->CopySubresourceRegion(sr.GetTexture(), sr.GetSubResource(), 0, 0, 0, sr.GetStaging(), 0, NULL);
			}
		}
		if (ptr)
		{
			ptr->Pitch = 0;
			ptr->U = ptr->V = ptr->Y = 0;
			ptr->A = ptr->R = ptr->G = ptr->B = 0;
		}
		return MFX_ERR_NONE;
	}
	mfxStatus QSVD3D11Allocator::CheckRequestType(mfxFrameAllocRequest *request)
	{
		mfxStatus status = QSVAllocator::CheckRequestType(request);
		if (MFX_ERR_NONE != status)
			return status;
		if ((request->Type & (MFX_MEMTYPE_VIDEO_MEMORY_DECODER_TARGET | MFX_MEMTYPE_VIDEO_MEMORY_PROCESSOR_TARGET)) != 0)
			return MFX_ERR_NONE;
		else
			return MFX_ERR_UNSUPPORTED;
	}
	mfxStatus QSVD3D11Allocator::Allocate(mfxFrameAllocRequest *request, mfxFrameAllocResponse *response)
	{
		HRESULT hRes = S_OK;
		DXGI_FORMAT colorFormat = ConverColortFormat(request->Info.FourCC);
		if (DXGI_FORMAT_UNKNOWN == colorFormat)
		{
			return MFX_ERR_UNSUPPORTED;
		}
		TextureResource texture;
		if (request->Info.FourCC == MFX_FOURCC_P8)
		{
			D3D11_BUFFER_DESC desc = { 0 };

			desc.ByteWidth = request->Info.Width * request->Info.Height;
			desc.Usage = D3D11_USAGE_STAGING;
			desc.BindFlags = 0;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			desc.MiscFlags = 0;
			desc.StructureByteStride = 0;
			ID3D11Buffer * buffer = 0;
			hRes = m_initParams.pDevice->CreateBuffer(&desc, 0, &buffer);
			if (FAILED(hRes))
				return MFX_ERR_MEMORY_ALLOC;
			texture.textures.push_back(reinterpret_cast<ID3D11Texture2D *>(buffer));
		}
		else
		{
			D3D11_TEXTURE2D_DESC desc = { 0 };
			desc.Width = request->Info.Width;
			desc.Height = request->Info.Height;
			desc.MipLevels = 1;
			desc.ArraySize = m_initParams.bUseSingleTexture ? request->NumFrameSuggested : 1;
			desc.Format = ConverColortFormat(request->Info.FourCC);
			desc.SampleDesc.Count = 1;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.MiscFlags = m_initParams.uncompressedResourceMiscFlags | D3D11_RESOURCE_MISC_SHARED;
			desc.BindFlags = D3D11_BIND_DECODER;
			if ((MFX_MEMTYPE_FROM_VPPIN & request->Type) && (DXGI_FORMAT_YUY2 == desc.Format) ||
				(DXGI_FORMAT_B8G8R8A8_UNORM == desc.Format) ||
				(DXGI_FORMAT_R10G10B10A2_UNORM == desc.Format) ||
				(DXGI_FORMAT_R16G16B16A16_UNORM == desc.Format))
			{
				desc.BindFlags = D3D11_BIND_RENDER_TARGET;
				if (desc.ArraySize > 2)
					return MFX_ERR_MEMORY_ALLOC;
			}
			if ((MFX_MEMTYPE_FROM_VPPOUT & request->Type) || (MFX_MEMTYPE_VIDEO_MEMORY_PROCESSOR_TARGET & request->Type))
			{
				desc.BindFlags = D3D11_BIND_RENDER_TARGET;
				if (desc.ArraySize > 2)
					return MFX_ERR_MEMORY_ALLOC;
			}
			if (request->Type&MFX_MEMTYPE_SHARED_RESOURCE)
			{
				desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
				desc.MiscFlags = D3D11_RESOURCE_MISC_SHARED;
			}
			if (DXGI_FORMAT_P8 == desc.Format)
			{
				desc.BindFlags = 0;
			}
			ID3D11Texture2D* pTexture2D;
			for (size_t i = 0; i < request->NumFrameSuggested / desc.ArraySize; i++)
			{
				hRes = m_initParams.pDevice->CreateTexture2D(&desc, NULL, &pTexture2D);
				if (FAILED(hRes))
				{
					return MFX_ERR_MEMORY_ALLOC;
				}
				texture.textures.push_back(pTexture2D);
			}
			desc.ArraySize = 1;
			desc.Usage = D3D11_USAGE_STAGING;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			desc.BindFlags = 0;
			desc.MiscFlags = 0;
			for (size_t i = 0; i < request->NumFrameSuggested; i++)
			{
				hRes = m_initParams.pDevice->CreateTexture2D(&desc, NULL, &pTexture2D);
				if (FAILED(hRes))
				{
					return MFX_ERR_MEMORY_ALLOC;
				}
				texture.stagingTexture.push_back(pTexture2D);
			}
		}
		sequence<mfxHDL> seq_initializer(m_resourcesByRequest.empty() ? 0 : m_resourcesByRequest.back().outerMids.back());
		seq_initializer();
		std::generate_n(std::back_inserter(texture.outerMids), request->NumFrameSuggested, seq_initializer);
		m_resourcesByRequest.push_back(texture);
		response->mids = &m_resourcesByRequest.back().outerMids.front();
		response->NumFrameActual = request->NumFrameSuggested;
		std::list <TextureResource>::iterator it_last = m_resourcesByRequest.end();
		std::fill_n(std::back_inserter(m_memIDMap), request->NumFrameSuggested, --it_last);
		return MFX_ERR_NONE;
	}
	mfxStatus QSVD3D11Allocator::Deallocate(mfxFrameAllocResponse *response)
	{
		if (NULL == response)
			return MFX_ERR_NULL_PTR;
		if (response->mids && 0 != response->NumFrameActual)
		{
			TextureSubResource sr = GetResourceFromMID(response->mids[0]);
			if (!sr.GetTexture())
				return MFX_ERR_NULL_PTR;
			sr.Release();
			if (m_resourcesByRequest.end() == std::find_if(m_resourcesByRequest.begin(), m_resourcesByRequest.end(), TextureResource::IsAllocated))
			{
				m_resourcesByRequest.clear();
				m_memIDMap.clear();
			}
		}
		return MFX_ERR_NONE;
	}
	QSVD3D11Allocator::TextureSubResource QSVD3D11Allocator::GetResourceFromMID(mfxMemId mid)
	{
		size_t index = (size_t)MFXReadWriteMid(mid).raw() - 1;
		if (m_memIDMap.size() <= index)
			return TextureSubResource();
		TextureResource * p = &(*m_memIDMap[index]);
		if (!p->bAlloc)
			return TextureSubResource();
		return TextureSubResource(p, mid);
	}
	DXGI_FORMAT QSVD3D11Allocator::ConverColortFormat(mfxU32 fourcc)
	{
		switch (fourcc)
		{
		case MFX_FOURCC_NV12:
			return DXGI_FORMAT_NV12;
		case MFX_FOURCC_YUY2:
			return DXGI_FORMAT_YUY2;
		case MFX_FOURCC_RGB4:
			return DXGI_FORMAT_B8G8R8A8_UNORM;
		case MFX_FOURCC_P8:
		case MFX_FOURCC_P8_TEXTURE:
			return DXGI_FORMAT_P8;
		default:
			return DXGI_FORMAT_UNKNOWN;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	GeneralAllocator::GeneralAllocator()
	{
	};
	GeneralAllocator::~GeneralAllocator()
	{
	};
	mfxStatus GeneralAllocator::Initialize(mfxAllocatorParams *pParams)
	{
		mfxStatus status = MFX_ERR_NONE;
		QSVD3D9AllocatorParams *d3dAllocParams = dynamic_cast<QSVD3D9AllocatorParams*>(pParams);
		if (d3dAllocParams)
		{
			m_d3dAllocator.Reset(new QSVD3D9Allocator());
			status = m_d3dAllocator->Initialize(pParams);
			if (status != MFX_ERR_NONE)
				return status;
		}
		m_memeryAllocator.Reset(new QSVMemeryAllocator());
		status = m_memeryAllocator->Initialize(NULL);
		if (status != MFX_ERR_NONE)
			return status;
		return status;
	}
	mfxStatus GeneralAllocator::Close()
	{
		mfxStatus status = MFX_ERR_NONE;
		if (m_d3dAllocator)
		{
			status = m_d3dAllocator->Close();
			if (status != MFX_ERR_NONE)
				return status;
		}
		if (m_memeryAllocator)
			status = m_memeryAllocator->Close();
		if (status != MFX_ERR_NONE)
			return status;
		return status;
	}

	mfxStatus GeneralAllocator::LockFrame(mfxMemId mid, mfxFrameData *ptr)
	{
		if (IsD3DMid(mid) && m_d3dAllocator)
			return m_d3dAllocator->Lock(m_d3dAllocator, mid, ptr);
		else
			return m_memeryAllocator->Lock(m_memeryAllocator, mid, ptr);
	}
	mfxStatus GeneralAllocator::UnlockFrame(mfxMemId mid, mfxFrameData *ptr)
	{
		if (IsD3DMid(mid) && m_d3dAllocator)
			return m_d3dAllocator->Unlock(m_d3dAllocator, mid, ptr);
		else
			return m_memeryAllocator->Unlock(m_memeryAllocator, mid, ptr);
	}

	mfxStatus GeneralAllocator::GetFrameHDL(mfxMemId mid, mfxHDL *handle)
	{
		if (IsD3DMid(mid) && m_d3dAllocator)
			return m_d3dAllocator->GetHDL(m_d3dAllocator, mid, handle);
		else
			return m_memeryAllocator->GetHDL(m_memeryAllocator, mid, handle);
	}

	mfxStatus GeneralAllocator::Deallocate(mfxFrameAllocResponse *response)
	{
		if (IsD3DMid(response->mids[0]) && m_d3dAllocator)
			return m_d3dAllocator->Free(m_d3dAllocator, response);
		else
			return m_memeryAllocator->Free(m_memeryAllocator, response);
	}
	mfxStatus GeneralAllocator::Allocate(mfxFrameAllocRequest *request, mfxFrameAllocResponse *response)
	{
		mfxStatus status = MFX_ERR_NONE;
		if ((request->Type & MFX_MEMTYPE_VIDEO_MEMORY_DECODER_TARGET || request->Type & MFX_MEMTYPE_VIDEO_MEMORY_PROCESSOR_TARGET) && m_d3dAllocator)
		{
			status = m_d3dAllocator->Alloc(m_d3dAllocator, request, response);
			MSDK_CHECK_NOT_EQUAL(MFX_ERR_NONE, status, status);
			StoreFrameMids(TRUE, response);
		}
		else
		{
			status = m_memeryAllocator->Alloc(m_memeryAllocator, request, response);
			MSDK_CHECK_NOT_EQUAL(MFX_ERR_NONE, status, status);
			StoreFrameMids(FALSE, response);
		}
		return status;
	}
	void    GeneralAllocator::StoreFrameMids(BOOL isD3DFrames, mfxFrameAllocResponse *response)
	{
		for (mfxU32 i = 0; i < response->NumFrameActual; i++)
		{
			m_Mids.insert(std::pair<mfxHDL, BOOL>(response->mids[i], isD3DFrames));
		}
	}
	BOOL GeneralAllocator::IsD3DMid(mfxHDL mid)
	{
		std::map<mfxHDL, BOOL>::iterator it;
		it = m_Mids.find(mid);
		if (it == m_Mids.end())
			return FALSE;
		else
			return it->second;
	}
	//////////////////////////////////////////////////////////////////////////
	QSVMemeryAllocator::QSVMemeryAllocator()
		: m_pBufferAllocator(0), m_bOwnBufferAllocator(false)
	{
	}

	QSVMemeryAllocator::~QSVMemeryAllocator()
	{
		Close();
	}

	mfxStatus QSVMemeryAllocator::Initialize(mfxAllocatorParams *pParams)
	{
		if (pParams)
		{
			QSVMemeryAllocatorParams *ps = 0;
			ps = dynamic_cast<QSVMemeryAllocatorParams *>(pParams);
			if (!ps)
				return MFX_ERR_NOT_INITIALIZED;
			m_pBufferAllocator = ps->pBufferAllocator;
			m_bOwnBufferAllocator = FALSE;
		}
		if (!m_pBufferAllocator)
		{
			m_pBufferAllocator = new QSVBufferAllocator();
			if (!m_pBufferAllocator)
				return MFX_ERR_MEMORY_ALLOC;
			m_bOwnBufferAllocator = TRUE;
		}
		return MFX_ERR_NONE;
	}

	mfxStatus QSVMemeryAllocator::Close()
	{
		mfxStatus status = QSVAllocator::Close();
		if (m_bOwnBufferAllocator)
		{
			SAFE_FREE(m_pBufferAllocator);
		}
		return status;
	}

	mfxStatus QSVMemeryAllocator::LockFrame(mfxMemId mid, mfxFrameData *ptr)
	{
		if (!m_pBufferAllocator)
			return MFX_ERR_NOT_INITIALIZED;
		if (!ptr)
			return MFX_ERR_NULL_PTR;
		if (!mid && ptr->Y)
			return MFX_ERR_NONE;
		sFrame *fs = 0;
		mfxStatus status = m_pBufferAllocator->Lock(m_pBufferAllocator->pthis, mid, (mfxU8 **)&fs);
		if (MFX_ERR_NONE != status)
			return status;
		if (ID_FRAME != fs->id)
		{
			m_pBufferAllocator->Unlock(m_pBufferAllocator->pthis, mid);
			return MFX_ERR_INVALID_HANDLE;
		}
		mfxU16 Width2 = (mfxU16)MSDK_ALIGN32(fs->info.Width);
		mfxU16 Height2 = (mfxU16)MSDK_ALIGN32(fs->info.Height);
		ptr->B = ptr->Y = (mfxU8 *)fs + MSDK_ALIGN32(sizeof(sFrame));
		switch (fs->info.FourCC)
		{
		case MFX_FOURCC_NV12:
			ptr->U = ptr->Y + Width2 * Height2;
			ptr->V = ptr->U + 1;
			ptr->Pitch = Width2;
			break;
		case MFX_FOURCC_NV16:
			ptr->U = ptr->Y + Width2 * Height2;
			ptr->V = ptr->U + 1;
			ptr->Pitch = Width2;
			break;
		case MFX_FOURCC_YV12:
			ptr->V = ptr->Y + Width2 * Height2;
			ptr->U = ptr->V + (Width2 >> 1) * (Height2 >> 1);
			ptr->Pitch = Width2;
			break;
		case MFX_FOURCC_UYVY:
			ptr->U = ptr->Y;
			ptr->Y = ptr->U + 1;
			ptr->V = ptr->U + 2;
			ptr->Pitch = 2 * Width2;
			break;
		case MFX_FOURCC_YUY2:
			ptr->U = ptr->Y + 1;
			ptr->V = ptr->Y + 3;
			ptr->Pitch = 2 * Width2;
			break;
		case MFX_FOURCC_RGB3:
			ptr->G = ptr->B + 1;
			ptr->R = ptr->B + 2;
			ptr->Pitch = 3 * Width2;
			break;
		case MFX_FOURCC_RGB4:
		case MFX_FOURCC_A2RGB10:
			ptr->G = ptr->B + 1;
			ptr->R = ptr->B + 2;
			ptr->A = ptr->B + 3;
			ptr->Pitch = 4 * Width2;
			break;
		case MFX_FOURCC_R16:
			ptr->Y16 = (mfxU16 *)ptr->B;
			ptr->Pitch = 2 * Width2;
			break;
		case MFX_FOURCC_P010:
			ptr->U = ptr->Y + Width2 * Height2 * 2;
			ptr->V = ptr->U + 2;
			ptr->Pitch = Width2 * 2;
			break;
		case MFX_FOURCC_P210:
			ptr->U = ptr->Y + Width2 * Height2 * 2;
			ptr->V = ptr->U + 2;
			ptr->Pitch = Width2 * 2;
			break;
		case MFX_FOURCC_AYUV:
			ptr->Y = ptr->B;
			ptr->U = ptr->Y + 1;
			ptr->V = ptr->Y + 2;
			ptr->A = ptr->Y + 3;
			ptr->Pitch = 4 * Width2;
			break;
		default:
			return MFX_ERR_UNSUPPORTED;
		}
		return MFX_ERR_NONE;
	}

	mfxStatus QSVMemeryAllocator::UnlockFrame(mfxMemId mid, mfxFrameData *ptr)
	{
		if (!m_pBufferAllocator)
			return MFX_ERR_NOT_INITIALIZED;
		if (!mid && ptr->Y)
			return MFX_ERR_NONE;
		mfxStatus sts = m_pBufferAllocator->Unlock(m_pBufferAllocator->pthis, mid);
		if (MFX_ERR_NONE != sts)
			return sts;
		if (NULL != ptr)
		{
			ptr->Pitch = 0;
			ptr->Y = 0;
			ptr->U = 0;
			ptr->V = 0;
			ptr->A = 0;
		}
		return MFX_ERR_NONE;
	}

	mfxStatus QSVMemeryAllocator::GetFrameHDL(mfxMemId mid, mfxHDL *handle)
	{
		return MFX_ERR_UNSUPPORTED;
	}

	mfxStatus QSVMemeryAllocator::CheckRequestType(mfxFrameAllocRequest *request)
	{
		mfxStatus sts = QSVAllocator::CheckRequestType(request);
		if (MFX_ERR_NONE != sts)
			return sts;

		if ((request->Type & MFX_MEMTYPE_SYSTEM_MEMORY) != 0)
			return MFX_ERR_NONE;
		else
			return MFX_ERR_UNSUPPORTED;
	}

	mfxStatus QSVMemeryAllocator::Allocate(mfxFrameAllocRequest *request, mfxFrameAllocResponse *response)
	{
		if (!m_pBufferAllocator)
			return MFX_ERR_NOT_INITIALIZED;
		mfxU32 Width2 = MSDK_ALIGN32(request->Info.Width);
		mfxU32 Height2 = MSDK_ALIGN32(request->Info.Height);
		mfxU32 nbytes;
		switch (request->Info.FourCC)
		{
		case MFX_FOURCC_YV12:
		case MFX_FOURCC_NV12:
			nbytes = Width2*Height2 + (Width2 >> 1)*(Height2 >> 1) + (Width2 >> 1)*(Height2 >> 1);
			break;
		case MFX_FOURCC_NV16:
			nbytes = Width2*Height2 + (Width2 >> 1)*(Height2)+(Width2 >> 1)*(Height2);
			break;
		case MFX_FOURCC_RGB3:
			nbytes = Width2*Height2 + Width2*Height2 + Width2*Height2;
			break;
		case MFX_FOURCC_RGB4:
		case MFX_FOURCC_AYUV:
			nbytes = Width2*Height2 + Width2*Height2 + Width2*Height2 + Width2*Height2;
			break;
		case MFX_FOURCC_UYVY:
		case MFX_FOURCC_YUY2:
			nbytes = Width2*Height2 + (Width2 >> 1)*(Height2)+(Width2 >> 1)*(Height2);
			break;
		case MFX_FOURCC_R16:
			nbytes = 2 * Width2*Height2;
			break;
		case MFX_FOURCC_P010:
			nbytes = Width2*Height2 + (Width2 >> 1)*(Height2 >> 1) + (Width2 >> 1)*(Height2 >> 1);
			nbytes *= 2;
			break;
		case MFX_FOURCC_A2RGB10:
			nbytes = Width2*Height2 * 4;
			break;
		case MFX_FOURCC_P210:
			nbytes = Width2*Height2 + (Width2 >> 1)*(Height2)+(Width2 >> 1)*(Height2);
			nbytes *= 2;
			break;
		default:
			return MFX_ERR_UNSUPPORTED;
		}
		mfxMemId* mids = new mfxMemId[request->NumFrameSuggested];
		if (!mids)
			return MFX_ERR_MEMORY_ALLOC;
		mfxU32 numAllocated = 0;
		for (numAllocated = 0; numAllocated < request->NumFrameSuggested; numAllocated++)
		{
			mfxStatus status = m_pBufferAllocator->Alloc(m_pBufferAllocator->pthis, nbytes + MSDK_ALIGN32(sizeof(sFrame)), request->Type, &(mids[numAllocated]));
			if (MFX_ERR_NONE != status)
				break;
			sFrame *fs;
			status = m_pBufferAllocator->Lock(m_pBufferAllocator->pthis, mids[numAllocated], (mfxU8 **)&fs);
			if (MFX_ERR_NONE != status)
				break;
			fs->id = ID_FRAME;
			fs->info = request->Info;
			m_pBufferAllocator->Unlock(m_pBufferAllocator->pthis, mids[numAllocated]);
		}
		if (numAllocated < request->NumFrameSuggested)
		{
			SAFE_DELETE_ARRAY(mids);
			return MFX_ERR_MEMORY_ALLOC;
		}
		response->NumFrameActual = (mfxU16)numAllocated;
		response->mids = mids;
		return MFX_ERR_NONE;
	}

	mfxStatus QSVMemeryAllocator::Deallocate(mfxFrameAllocResponse *response)
	{
		if (!response)
			return MFX_ERR_NULL_PTR;
		if (!m_pBufferAllocator)
			return MFX_ERR_NOT_INITIALIZED;
		mfxStatus status = MFX_ERR_NONE;
		if (response->mids)
		{
			for (mfxU32 i = 0; i < response->NumFrameActual; i++)
			{
				if (response->mids[i])
				{
					status = m_pBufferAllocator->Free(m_pBufferAllocator->pthis, response->mids[i]);
					if (MFX_ERR_NONE != status)
						return status;
				}
			}
		}
		SAFE_DELETE_ARRAY(response->mids);
		return status;
	}
}

