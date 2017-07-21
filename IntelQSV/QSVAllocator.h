#pragma once
#include "QSVD3D.h"
#include "IO/TinyFixedAlloc.h"
using namespace TinyUI;
using namespace TinyUI::IO;

namespace QSV
{
	struct mfxFrameAllocResponseEx : mfxFrameAllocResponse
	{
		mfxU16 Width;
		mfxU16 Height;
		mfxU32 RefCount;
		mfxU16 Type;
		mfxFrameAllocResponseEx();
		mfxFrameAllocResponseEx(const mfxFrameAllocResponse & response, mfxU16 width, mfxU16 height, mfxU16 type);
		BOOL operator () (const mfxFrameAllocResponseEx &response) const;
	};
	struct mfxAllocatorParams
	{
		virtual ~mfxAllocatorParams() {};
	};
	struct QSVD3D9AllocatorParams : mfxAllocatorParams
	{
		DWORD						dwUsage;
		IDirect3DDeviceManager9*	pManager;
		QSVD3D9AllocatorParams()
			: pManager(NULL),
			dwUsage(0)
		{
		}
	};
	//////////////////////////////////////////////////////////////////////////
	class QSVBufferAllocatorBase : public mfxBufferAllocator
	{
	public:
		QSVBufferAllocatorBase();
		virtual ~QSVBufferAllocatorBase();
	public:
		virtual mfxStatus AllocBuffer(mfxU32 nbytes, mfxU16 type, mfxMemId *mid) = 0;
		virtual mfxStatus LockBuffer(mfxMemId mid, mfxU8 **ptr) = 0;
		virtual mfxStatus UnlockBuffer(mfxMemId mid) = 0;
		virtual mfxStatus FreeBuffer(mfxMemId mid) = 0;
	private:
		static mfxStatus MFX_CDECL  Alloc_(mfxHDL pthis, mfxU32 nbytes, mfxU16 type, mfxMemId *mid);
		static mfxStatus MFX_CDECL  Lock_(mfxHDL pthis, mfxMemId mid, mfxU8 **ptr);
		static mfxStatus MFX_CDECL  Unlock_(mfxHDL pthis, mfxMemId mid);
		static mfxStatus MFX_CDECL  Free_(mfxHDL pthis, mfxMemId mid);
	};
	//////////////////////////////////////////////////////////////////////////
	class QSVBufferAllocator : public QSVBufferAllocatorBase
	{
	public:
		QSVBufferAllocator();
		virtual ~QSVBufferAllocator();
	public:
		mfxStatus AllocBuffer(mfxU32 nbytes, mfxU16 type, mfxMemId *mid) OVERRIDE;
		mfxStatus LockBuffer(mfxMemId mid, mfxU8 **ptr) OVERRIDE;
		mfxStatus UnlockBuffer(mfxMemId mid) OVERRIDE;
		mfxStatus FreeBuffer(mfxMemId mid) OVERRIDE;
	};
	//////////////////////////////////////////////////////////////////////////
	class QSVAllocatorBase : public mfxFrameAllocator
	{
		DISALLOW_COPY_AND_ASSIGN(QSVAllocatorBase)
	public:
		QSVAllocatorBase();
		virtual ~QSVAllocatorBase();
	public:
		virtual mfxStatus Initialize(mfxAllocatorParams *pParams) = 0;
		virtual mfxStatus Close() = 0;
		virtual mfxStatus AllocFrames(mfxFrameAllocRequest *request, mfxFrameAllocResponse *response) = 0;
		virtual mfxStatus LockFrame(mfxMemId mid, mfxFrameData *ptr) = 0;
		virtual mfxStatus UnlockFrame(mfxMemId mid, mfxFrameData *ptr) = 0;
		virtual mfxStatus GetFrameHDL(mfxMemId mid, mfxHDL *handle) = 0;
		virtual mfxStatus FreeFrames(mfxFrameAllocResponse *response) = 0;
	protected:
		virtual mfxStatus CheckRequestType(mfxFrameAllocRequest *request) = 0;
		static const mfxU32 MEMTYPE_FROM_MASK = MFX_MEMTYPE_FROM_ENCODE | MFX_MEMTYPE_FROM_DECODE | MFX_MEMTYPE_FROM_VPPIN | MFX_MEMTYPE_FROM_VPPOUT | MFX_MEMTYPE_FROM_ENC | MFX_MEMTYPE_FROM_PAK;
	private:
		static mfxStatus MFX_CDECL  Alloc_(mfxHDL pthis, mfxFrameAllocRequest *request, mfxFrameAllocResponse *response);
		static mfxStatus MFX_CDECL  Lock_(mfxHDL pthis, mfxMemId mid, mfxFrameData *ptr);
		static mfxStatus MFX_CDECL  Unlock_(mfxHDL pthis, mfxMemId mid, mfxFrameData *ptr);
		static mfxStatus MFX_CDECL  GetHDL_(mfxHDL pthis, mfxMemId mid, mfxHDL *handle);
		static mfxStatus MFX_CDECL  Free_(mfxHDL pthis, mfxFrameAllocResponse *response);
	};
	//////////////////////////////////////////////////////////////////////////
	class QSVAllocator : public QSVAllocatorBase
	{
		DISALLOW_COPY_AND_ASSIGN(QSVAllocator)
	public:
		QSVAllocator();
		virtual ~QSVAllocator();
	public:
		mfxStatus Close() OVERRIDE;
		mfxStatus AllocFrames(mfxFrameAllocRequest *request, mfxFrameAllocResponse *response) OVERRIDE;
		mfxStatus FreeFrames(mfxFrameAllocResponse *response) OVERRIDE;
	protected:
		mfxStatus CheckRequestType(mfxFrameAllocRequest *request) OVERRIDE;
		virtual mfxStatus Allocate(mfxFrameAllocRequest *request, mfxFrameAllocResponse *response) = 0;
		virtual mfxStatus Deallocate(mfxFrameAllocResponse *response) = 0;
	private:
		TinyLinkList<mfxFrameAllocResponse>		m_responses;
		TinyLinkList<mfxFrameAllocResponseEx>	m_responsexs;
	};
	//////////////////////////////////////////////////////////////////////////
	class QSVD3D9Allocator : public QSVAllocator
	{
		DISALLOW_COPY_AND_ASSIGN(QSVD3D9Allocator)
	public:
		QSVD3D9Allocator();
		virtual ~QSVD3D9Allocator();
	public:
		mfxStatus Initialize(mfxAllocatorParams *pParams) OVERRIDE;
		mfxStatus Close() OVERRIDE;
		mfxStatus LockFrame(mfxMemId mid, mfxFrameData *ptr);
		mfxStatus GetFrameHDL(mfxMemId mid, mfxHDL *handle);
		mfxStatus UnlockFrame(mfxMemId mid, mfxFrameData *ptr);
	protected:
		mfxStatus CheckRequestType(mfxFrameAllocRequest *request) OVERRIDE;
		mfxStatus Allocate(mfxFrameAllocRequest *request, mfxFrameAllocResponse *response) OVERRIDE;
		mfxStatus Deallocate(mfxFrameAllocResponse *response) OVERRIDE;
	private:
		void DeallocateMids(mfxHDLPair** pair, INT size);
	private:
		DWORD										m_dwUsage;
		HANDLE										m_hDecoder;
		HANDLE										m_hProcessor;
		std::vector<mfxHDLPair**>					m_midsAllocated;
		TinyComPtr<IDirect3DDeviceManager9>			m_manager;
		TinyComPtr<IDirectXVideoDecoderService>		m_decoderService;
		TinyComPtr<IDirectXVideoProcessorService>	m_processorService;
	};
	//////////////////////////////////////////////////////////////////////////
	struct sBuffer
	{
		mfxU32      id;
		mfxU32      nbytes;
		mfxU16      type;
	};
	struct sFrame
	{
		mfxU32          id;
		mfxFrameInfo    info;
	};
	struct QSVMemeryAllocatorParams : mfxAllocatorParams
	{
		QSVMemeryAllocatorParams()
			: mfxAllocatorParams(),
			pBufferAllocator(NULL)
		{
		}
		QSVBufferAllocatorBase *pBufferAllocator;
	};
	class QSVMemeryAllocator : public QSVAllocator
	{
	public:
		QSVMemeryAllocator();
		virtual ~QSVMemeryAllocator();
	public:
		mfxStatus Initialize(mfxAllocatorParams *pParams) OVERRIDE;
		mfxStatus Close() OVERRIDE;
		mfxStatus LockFrame(mfxMemId mid, mfxFrameData *ptr) OVERRIDE;
		mfxStatus UnlockFrame(mfxMemId mid, mfxFrameData *ptr) OVERRIDE;
		mfxStatus GetFrameHDL(mfxMemId mid, mfxHDL *handle) OVERRIDE;
	protected:
		mfxStatus CheckRequestType(mfxFrameAllocRequest *request) OVERRIDE;
		mfxStatus Allocate(mfxFrameAllocRequest *request, mfxFrameAllocResponse *response) OVERRIDE;
		mfxStatus Deallocate(mfxFrameAllocResponse *response) OVERRIDE;
	protected:
		BOOL					m_bOwnBufferAllocator;
		QSVBufferAllocatorBase*	m_pBufferAllocator;
	};
	//////////////////////////////////////////////////////////////////////////
	class GeneralAllocator : public QSVAllocator
	{
	public:
		GeneralAllocator();
		virtual ~GeneralAllocator();
	public:
		mfxStatus Initialize(mfxAllocatorParams *pParams) OVERRIDE;
		mfxStatus Close() OVERRIDE;
	protected:
		mfxStatus LockFrame(mfxMemId mid, mfxFrameData *ptr) OVERRIDE;
		mfxStatus UnlockFrame(mfxMemId mid, mfxFrameData *ptr) OVERRIDE;
		mfxStatus GetFrameHDL(mfxMemId mid, mfxHDL *handle) OVERRIDE;
		mfxStatus Allocate(mfxFrameAllocRequest *request, mfxFrameAllocResponse *response) OVERRIDE;
		mfxStatus Deallocate(mfxFrameAllocResponse *response) OVERRIDE;
		void    StoreFrameMids(BOOL isD3DFrames, mfxFrameAllocResponse *response);
		BOOL    IsD3DMid(mfxHDL mid);
	protected:
		std::map<mfxHDL, BOOL>  m_Mids;
		TinyScopedPtr<QSVAllocator>			m_d3dAllocator;
		TinyScopedPtr<QSVMemeryAllocator>   m_memeryAllocator;
	};
}



