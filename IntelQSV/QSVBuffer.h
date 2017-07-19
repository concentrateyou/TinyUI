#pragma once
#include "QSVAllocator.h"

namespace QSV
{
	class QSVBuffering;
	struct QSVFrameSurface
	{
		mfxFrameSurface1 frame;
		mfxU16 submit;
		mfxU16 render_lock;
		QSVFrameSurface* prev;
		QSVFrameSurface* next;
	};
	struct QSVOutputSurface
	{
		QSVFrameSurface* surface;
		mfxSyncPoint syncp;
		QSVOutputSurface* next;
	};
	class QSVFreeSurfacesPool
	{
		DISALLOW_COPY_AND_ASSIGN(QSVFreeSurfacesPool)
			friend class QSVBuffering;
	public:
		QSVFreeSurfacesPool(TinyLock* lock)
			:m_pSurfaces(NULL),
			m_pLock(lock)
		{
		}

		~QSVFreeSurfacesPool()
		{
			m_pSurfaces = NULL;
		}
		inline void AddSurface(QSVFrameSurface* surface)
		{
			TinyAutoLock lock(*m_pLock);
			AddSurfaceUnsafe(surface);
		}

		inline QSVFrameSurface* GetSurface()
		{
			TinyAutoLock lock(*m_pLock);
			return GetSurfaceUnsafe();
		}

	private:
		inline void AddSurfaceUnsafe(QSVFrameSurface* surface)
		{
			QSVFrameSurface* head;
			head = m_pSurfaces;
			m_pSurfaces = surface;
			m_pSurfaces->next = head;
		}
		inline QSVFrameSurface* GetSurfaceUnsafe()
		{
			QSVFrameSurface* surface = NULL;
			if (m_pSurfaces)
			{
				surface = m_pSurfaces;
				m_pSurfaces = m_pSurfaces->next;
				surface->prev = surface->next = NULL;
			}
			return surface;
		}

	protected:
		QSVFrameSurface* m_pSurfaces;
		TinyLock* m_pLock;
	};

	class QSVUsedSurfacesPool
	{
		DISALLOW_COPY_AND_ASSIGN(QSVUsedSurfacesPool)
			friend class QSVBuffering;
	public:
		QSVUsedSurfacesPool(TinyLock* lock)
			:m_pSurfacesHead(NULL),
			m_pSurfacesTail(NULL),
			m_pLock(lock)
		{
		}

		~QSVUsedSurfacesPool()
		{
			m_pSurfacesHead = NULL;
			m_pSurfacesTail = NULL;
		}

		inline void AddSurface(QSVFrameSurface* surface)
		{
			TinyAutoLock lock(*m_pLock);
			AddSurfaceUnsafe(surface);
		}


		inline void DetachSurface(QSVFrameSurface* surface)
		{
			TinyAutoLock lock(*m_pLock);
			DetachSurfaceUnsafe(surface);
		}

	private:
		inline void DetachSurfaceUnsafe(QSVFrameSurface* surface)
		{
			QSVFrameSurface *prev = surface->prev;
			QSVFrameSurface *next = surface->next;
			if (prev)
			{
				prev->next = next;
			}
			else
			{
				m_pSurfacesHead = next;
			}
			if (next)
			{
				next->prev = prev;
			}
			else
			{
				m_pSurfacesTail = prev;
			}
			surface->prev = surface->next = NULL;
		}
		inline void AddSurfaceUnsafe(QSVFrameSurface* surface)
		{
			surface->prev = m_pSurfacesTail;
			surface->next = NULL;
			if (m_pSurfacesTail)
			{
				m_pSurfacesTail->next = surface;
				m_pSurfacesTail = m_pSurfacesTail->next;
			}
			else
			{
				m_pSurfacesHead = m_pSurfacesTail = surface;
			}
		}

	protected:
		QSVFrameSurface* m_pSurfacesHead;
		QSVFrameSurface* m_pSurfacesTail;
		TinyLock* m_pLock;
	};

	class QSVOutputSurfacesPool
	{
		DISALLOW_COPY_AND_ASSIGN(QSVOutputSurfacesPool)
		friend class QSVBuffering;
	public:
		QSVOutputSurfacesPool(TinyLock* lock) :m_pSurfacesHead(NULL),
			m_pSurfacesTail(NULL),
			m_SurfacesCount(0),
			m_pLock(lock) {}

		~QSVOutputSurfacesPool() 
		{
			m_pSurfacesHead = NULL;
			m_pSurfacesTail = NULL;
		}

		inline void AddSurface(QSVOutputSurface* surface) 
		{
			TinyAutoLock lock(*m_pLock);
			AddSurfaceUnsafe(surface);
		}
		inline QSVOutputSurface* GetSurface()
		{
			TinyAutoLock lock(*m_pLock);
			return GetSurfaceUnsafe();
		}

		inline mfxU32 GetSurfaceCount()
		{
			return m_SurfacesCount;
		}
	private:
		inline void AddSurfaceUnsafe(QSVOutputSurface* surface)
		{
			surface->next = NULL;

			if (m_pSurfacesTail)
			{
				m_pSurfacesTail->next = surface;
				m_pSurfacesTail = m_pSurfacesTail->next;
			}
			else 
			{
				m_pSurfacesHead = m_pSurfacesTail = surface;
			}
			++m_SurfacesCount;
		}
		inline QSVOutputSurface* GetSurfaceUnsafe()
		{
			QSVOutputSurface* surface = NULL;

			if (m_pSurfacesHead) 
			{
				surface = m_pSurfacesHead;
				m_pSurfacesHead = m_pSurfacesHead->next;
				if (!m_pSurfacesHead) 
				{
					m_pSurfacesTail = NULL;
				}
				--m_SurfacesCount;
				surface->next = NULL;
			}
			return surface;
		}

	protected:
		QSVOutputSurface*      m_pSurfacesHead; 
		QSVOutputSurface*      m_pSurfacesTail;
		TinyLock*              m_pLock;
		mfxU32                 m_SurfacesCount;
	};

	class QSVBuffering
	{
		DISALLOW_COPY_AND_ASSIGN(QSVBuffering)
	public:
		QSVBuffering();
		virtual ~QSVBuffering();
	protected:
		mfxStatus AllocBuffers(mfxU32 surfaceNumber);
		mfxStatus AllocVppBuffers(mfxU32 vppSurfaceNumber);
		void AllocOutputBuffer();
		void FreeBuffers();
		void ResetBuffers();
		void ResetVppBuffers();
		void SyncFrameSurfaces();
		void SyncVppFrameSurfaces();
		inline QSVFrameSurface* FindUsedSurface(mfxFrameSurface1* frame)
		{
			return (QSVFrameSurface*)(frame);
		}
		inline void AddFreeOutputSurfaceUnsafe(QSVOutputSurface* surface)
		{
			QSVOutputSurface* head = m_pFreeOutputSurfaces;
			m_pFreeOutputSurfaces = surface;
			m_pFreeOutputSurfaces->next = head;
		}
		inline void AddFreeOutputSurface(QSVOutputSurface* surface) 
		{
			TinyAutoLock lock(m_lock);
			AddFreeOutputSurfaceUnsafe(surface);
		}

		inline QSVOutputSurface* GetFreeOutputSurfaceUnsafe()
		{
			QSVOutputSurface* surface = NULL;

			if (!m_pFreeOutputSurfaces)
			{
				m_lock.Unlock();
				AllocOutputBuffer();
				m_lock.Lock();
			}
			if (m_pFreeOutputSurfaces) 
			{
				surface = m_pFreeOutputSurfaces;
				m_pFreeOutputSurfaces = m_pFreeOutputSurfaces->next;
				surface->next = NULL;
			}
			return surface;
		}
		inline QSVOutputSurface* GetFreeOutputSurface()
		{
			TinyAutoLock lock(m_lock);
			return GetFreeOutputSurfaceUnsafe();
		}

		inline void ReturnSurfaceToBuffers(QSVOutputSurface* output_surface)
		{
			_InterlockedDecrement16((volatile SHORT*)&output_surface->surface->render_lock);
			output_surface->surface = NULL;
			output_surface->syncp = NULL;
			AddFreeOutputSurface(output_surface);
		}

	protected: 
		mfxU32                 m_SurfacesNumber;
		mfxU32                 m_OutputSurfacesNumber;
		QSVFrameSurface*       m_pSurfaces;
		QSVFrameSurface*       m_pVppSurfaces;
		TinyLock               m_lock;
		QSVFreeSurfacesPool    m_FreeSurfacesPool;
		QSVFreeSurfacesPool    m_FreeVppSurfacesPool;
		QSVUsedSurfacesPool    m_UsedSurfacesPool;
		QSVUsedSurfacesPool    m_UsedVppSurfacesPool;
		QSVOutputSurface*      m_pFreeOutputSurfaces;
		QSVOutputSurfacesPool  m_OutputSurfacesPool;
		QSVOutputSurfacesPool  m_DeliveredSurfacesPool;
	};
}

