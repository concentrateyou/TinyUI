#pragma once
#include "QSVD3D.h"
#include "IO/TinyFixedAlloc.h"
using namespace TinyFramework;
using namespace TinyFramework::IO;

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
	struct QSVD3D11AllocatorParams : mfxAllocatorParams
	{
		ID3D11Device*	pDevice;
		BOOL			bUseSingleTexture;
		DWORD			uncompressedResourceMiscFlags;
		QSVD3D11AllocatorParams()
			: pDevice(NULL),
			bUseSingleTexture(FALSE),
			uncompressedResourceMiscFlags(0)
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
		TinyComPtr<IDirect3DDeviceManager9>			m_manager;
		TinyComPtr<IDirectXVideoDecoderService>		m_decoderService;
		TinyComPtr<IDirectXVideoProcessorService>	m_processorService;
	};
	//////////////////////////////////////////////////////////////////////////
	class QSVD3D11Allocator : public QSVAllocator
	{
		struct TextureResource
		{
			std::vector<mfxMemId> outerMids;
			std::vector<ID3D11Texture2D*> textures;
			std::vector<ID3D11Texture2D*> stagingTexture;
			BOOL   bAlloc;
			TextureResource()
				: bAlloc(TRUE)
			{
			}
			static BOOL IsAllocated(TextureResource & that)
			{
				return that.bAlloc;
			}
			ID3D11Texture2D* GetTexture(mfxMemId id)
			{
				if (outerMids.empty())
					return NULL;

				return textures[((uintptr_t)id - (uintptr_t)outerMids.front()) % textures.size()];
			}
			UINT GetSubResource(mfxMemId id)
			{
				if (outerMids.empty())
					return NULL;
				return (UINT)(((uintptr_t)id - (uintptr_t)outerMids.front()) / textures.size());
			}
			void Release()
			{
				for (size_t i = 0; i < textures.size(); i++)
				{
					textures[i]->Release();
				}
				textures.clear();
				for (size_t i = 0; i < stagingTexture.size(); i++)
				{
					stagingTexture[i]->Release();
				}
				stagingTexture.clear();
				bAlloc = false;
			}
		};
		class TextureSubResource
		{
			TextureResource * m_pTarget;
			ID3D11Texture2D * m_pTexture;
			ID3D11Texture2D * m_pStaging;
			UINT m_subResource;
		public:
			TextureSubResource(TextureResource * pTarget = NULL, mfxMemId id = 0)
				: m_pTarget(pTarget)
				, m_pTexture()
				, m_subResource()
				, m_pStaging(NULL)
			{
				if (NULL != m_pTarget && !m_pTarget->outerMids.empty())
				{
					ptrdiff_t idx = (uintptr_t)MFXReadWriteMid(id).raw() - (uintptr_t)m_pTarget->outerMids.front();
					m_pTexture = m_pTarget->textures[idx % m_pTarget->textures.size()];
					m_subResource = (UINT)(idx / m_pTarget->textures.size());
					m_pStaging = m_pTarget->stagingTexture.empty() ? NULL : m_pTarget->stagingTexture[idx];
				}
			}
			ID3D11Texture2D* GetStaging()const
			{
				return m_pStaging;
			}
			ID3D11Texture2D* GetTexture()const
			{
				return m_pTexture;
			}
			UINT GetSubResource()const
			{
				return m_subResource;
			}
			void Release()
			{
				if (NULL != m_pTarget)
				{
					m_pTarget->Release();
				}
			}
		};
		class MFXReadWriteMid
		{
			static const uintptr_t bits_offset = std::numeric_limits<uintptr_t>::digits - 1;
			static const uintptr_t clear_mask = ~((uintptr_t)1 << bits_offset);
		public:
			enum
			{
				not_set = 0,
				reuse = 1,
				read = 2,
				write = 4,
			};
			MFXReadWriteMid(mfxMemId mid, mfxU8 flag = not_set)
			{
				m_midReport = (mfxMemId)((uintptr_t)&m_mid | ((uintptr_t)1 << bits_offset));
				if (0 != ((uintptr_t)mid >> bits_offset))
				{
					mfxMedIdEx * pMemIdExt = reinterpret_cast<mfxMedIdEx *>((uintptr_t)mid & clear_mask);
					m_mid.pId = pMemIdExt->pId;
					if (reuse == flag)
					{
						m_mid.flag = pMemIdExt->flag;
					}
					else
					{
						m_mid.flag = flag;
					}
				}
				else
				{
					m_mid.pId = mid;
					if (reuse == flag)
						m_mid.flag = not_set;
					else
						m_mid.flag = flag;
				}

			}
			BOOL IsRead() const
			{
				return 0 != (m_mid.flag & read) || !m_mid.flag;
			}
			BOOL IsWrite() const
			{
				return 0 != (m_mid.flag & write) || !m_mid.flag;
			}

			mfxMemId raw() const
			{
				return m_mid.pId;
			}
			operator mfxMemId() const
			{
				return m_midReport;
			}

		private:
			struct mfxMedIdEx
			{
				mfxMemId	pId;
				mfxU8		flag;//¶ÁÐ´±êÖ¾Î»
			};

			mfxMedIdEx m_mid;
			mfxMemId   m_midReport;
		};
		DISALLOW_COPY_AND_ASSIGN(QSVD3D11Allocator)
	public:
		QSVD3D11Allocator();
		virtual ~QSVD3D11Allocator();
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
		TextureSubResource GetResourceFromMID(mfxMemId mid);
		static  DXGI_FORMAT ConverColortFormat(mfxU32 fourcc);
	private:
		typedef std::list <TextureResource>::iterator referenceType;
		std::list <TextureResource>		m_resourcesByRequest;
		std::vector<referenceType>		m_memIDMap;
		QSVD3D11AllocatorParams			m_initParams;
		TinyComPtr<ID3D11DeviceContext> m_deviceContext;
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



