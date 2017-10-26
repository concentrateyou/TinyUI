#pragma once
#include "Common/TinyCommon.h"
#include "Samples/common/inc/nvEncodeAPI.h"
#include "Samples/common/inc/dynlink_cuda.h"
#include "Samples/common/inc/NvHWEncoder.h"
#include <d3d9.h>
#include <d3d10_1.h>
#include <d3d11.h>

namespace NVIDIA
{
#define BITSTREAM_BUFFER_SIZE 2 * 1024 * 1024

#define MAX_ENCODE_QUEUE 32
#define FRAME_QUEUE 240
#define NUM_OF_MVHINTS_PER_BLOCK8x8   4
#define NUM_OF_MVHINTS_PER_BLOCK8x16  2
#define NUM_OF_MVHINTS_PER_BLOCK16x8  2
#define NUM_OF_MVHINTS_PER_BLOCK16x16 1

	typedef enum
	{
		NV_ENC_DX9 = 0,
		NV_ENC_DX11 = 1,
		NV_ENC_CUDA = 2,
		NV_ENC_DX10 = 3,
	} NvEncodeDeviceType;

	template<class T>
	class CNvQueue
	{
		T** m_pBuffer;
		UINT m_uSize;
		UINT m_uPendingCount;
		UINT m_uAvailableIdx;
		UINT m_uPendingndex;
	public:
		CNvQueue() :m_pBuffer(NULL),
			m_uSize(0),
			m_uPendingCount(0),
			m_uAvailableIdx(0),
			m_uPendingndex(0)
		{
		}

		~CNvQueue()
		{
			SAFE_DELETE_ARRAY(m_pBuffer);
		}

		BOOL Initialize(T *pItems, UINT uSize)
		{
			m_uSize = uSize;
			m_uPendingCount = 0;
			m_uAvailableIdx = 0;
			m_uPendingndex = 0;
			m_pBuffer = new T *[m_uSize];
			for (UINT i = 0; i < m_uSize; i++)
			{
				m_pBuffer[i] = &pItems[i];
			}
			return TRUE;
		}


		T * GetAvailable()
		{
			T *pItem = NULL;
			if (m_uPendingCount == m_uSize)
			{
				return NULL;
			}
			pItem = m_pBuffer[m_uAvailableIdx];
			m_uAvailableIdx = (m_uAvailableIdx + 1) % m_uSize;
			m_uPendingCount += 1;
			return pItem;
		}

		T* GetPending()
		{
			if (m_uPendingCount == 0)
			{
				return NULL;
			}

			T *pItem = m_pBuffer[m_uPendingndex];
			m_uPendingndex = (m_uPendingndex + 1) % m_uSize;
			m_uPendingCount -= 1;
			return pItem;
		}
	};
}