#include "stdafx.h"
#include "QSVBuffer.h"

namespace QSV
{
	QSVBuffering::QSVBuffering() :
		m_SurfacesNumber(0),
		m_OutputSurfacesNumber(0),
		m_pSurfaces(NULL),
		m_pVppSurfaces(NULL),
		m_FreeSurfacesPool(&m_lock),
		m_FreeVppSurfacesPool(&m_lock),
		m_UsedSurfacesPool(&m_lock),
		m_UsedVppSurfacesPool(&m_lock),
		m_pFreeOutputSurfaces(NULL),
		m_OutputSurfacesPool(&m_lock),
		m_DeliveredSurfacesPool(&m_lock)
	{
	}

	QSVBuffering::~QSVBuffering()
	{
	}

	mfxStatus QSVBuffering::AllocBuffers(mfxU32 SurfaceNumber)
	{
		if (!SurfaceNumber)
			return MFX_ERR_MEMORY_ALLOC;
		if (!m_OutputSurfacesNumber)
		{
			m_OutputSurfacesNumber = SurfaceNumber;
		}
		m_SurfacesNumber = SurfaceNumber;
		m_pSurfaces = (QSVFrameSurface*)calloc(m_SurfacesNumber, sizeof(QSVFrameSurface));
		if (!m_pSurfaces)
			return MFX_ERR_MEMORY_ALLOC;
		QSVOutputSurface* p = NULL;
		QSVOutputSurface* tail = NULL;
		m_pFreeOutputSurfaces = (QSVOutputSurface*)calloc(1, sizeof(QSVOutputSurface));
		if (!m_pFreeOutputSurfaces) return MFX_ERR_MEMORY_ALLOC;
		tail = m_pFreeOutputSurfaces;
		for (mfxU32 i = 1; i < m_OutputSurfacesNumber; ++i)
		{
			p = (QSVOutputSurface*)calloc(1, sizeof(QSVOutputSurface));
			if (!p) return MFX_ERR_MEMORY_ALLOC;
			tail->next = p;
			tail = p;
		}
		ResetBuffers();
		return MFX_ERR_NONE;
	}

	mfxStatus QSVBuffering::AllocVppBuffers(mfxU32 VppSurfaceNumber)
	{
		m_OutputSurfacesNumber = VppSurfaceNumber;
		m_pVppSurfaces = (QSVFrameSurface*)calloc(m_OutputSurfacesNumber, sizeof(QSVFrameSurface));
		if (!m_pVppSurfaces) 
			return MFX_ERR_MEMORY_ALLOC;
		ResetVppBuffers();
		return MFX_ERR_NONE;
	}

	void QSVBuffering::AllocOutputBuffer()
	{
		TinyAutoLock lock(m_lock);
		m_pFreeOutputSurfaces = (QSVOutputSurface*)calloc(1, sizeof(QSVOutputSurface));
	}

	static void FreeList(QSVOutputSurface*& head)
	{
		QSVOutputSurface* next;
		while (head)
		{
			next = head->next;
			free(head);
			head = next;
		}
	}

	void QSVBuffering::FreeBuffers()
	{
		SAFE_FREE(m_pSurfaces);
		SAFE_FREE(m_pVppSurfaces);
		FreeList(m_pFreeOutputSurfaces);
		FreeList(m_OutputSurfacesPool.m_pSurfacesHead);
		FreeList(m_DeliveredSurfacesPool.m_pSurfacesHead);
		m_UsedSurfacesPool.m_pSurfacesHead = NULL;
		m_UsedSurfacesPool.m_pSurfacesTail = NULL;
		m_UsedVppSurfacesPool.m_pSurfacesHead = NULL;
		m_UsedVppSurfacesPool.m_pSurfacesTail = NULL;
		m_FreeSurfacesPool.m_pSurfaces = NULL;
		m_FreeVppSurfacesPool.m_pSurfaces = NULL;
	}

	void QSVBuffering::ResetBuffers()
	{
		QSVFrameSurface* pFreeSurf = m_FreeSurfacesPool.m_pSurfaces = m_pSurfaces;
		for (mfxU32 i = 0; i < m_SurfacesNumber; ++i)
		{
			if (i < (m_SurfacesNumber - 1))
			{
				pFreeSurf[i].next = &(pFreeSurf[i + 1]);
				pFreeSurf[i + 1].prev = &(pFreeSurf[i]);
			}
		}
	}

	void QSVBuffering::ResetVppBuffers()
	{
		QSVFrameSurface* pFreeVppSurf = m_FreeVppSurfacesPool.m_pSurfaces = m_pVppSurfaces;
		for (mfxU32 i = 0; i < m_OutputSurfacesNumber; ++i)
		{
			if (i < (m_OutputSurfacesNumber - 1))
			{
				pFreeVppSurf[i].next = &(pFreeVppSurf[i + 1]);
				pFreeVppSurf[i + 1].prev = &(pFreeVppSurf[i]);
			}
		}
	}

	void QSVBuffering::SyncFrameSurfaces()
	{
		TinyAutoLock lock(m_lock);
		QSVFrameSurface *prev;
		QSVFrameSurface *next;
		prev = next = NULL;
		QSVFrameSurface *cur = m_UsedSurfacesPool.m_pSurfacesHead;

		while (cur)
		{
			if (cur->frame.Data.Locked || cur->render_lock)
			{
				cur = cur->next;
			}
			else
			{
				m_UsedSurfacesPool.DetachSurfaceUnsafe(cur);
				m_FreeSurfacesPool.AddSurfaceUnsafe(cur);
				cur = next;
			}
		}
	}

	void QSVBuffering::SyncVppFrameSurfaces()
	{
		TinyAutoLock lock(m_lock);
		QSVFrameSurface *prev;
		QSVFrameSurface *next;
		prev = next = NULL;
		QSVFrameSurface *cur = m_UsedVppSurfacesPool.m_pSurfacesHead;

		while (cur)
		{
			if (cur->frame.Data.Locked || cur->render_lock)
			{
				cur = cur->next;
			}
			else
			{
				m_UsedVppSurfacesPool.DetachSurfaceUnsafe(cur);
				m_FreeVppSurfacesPool.AddSurfaceUnsafe(cur);
				cur = next;
			}
		}
	}

}