#include "../stdafx.h"
#include "TinyMedia.h"

namespace TinyUI
{
	namespace Media
	{
		TinyScopedAvrt::TinyScopedAvrt(LPCSTR pzTaskName)
			:m_hMM(NULL),
			m_dwTaskIndex(0)
		{
			m_hMM = AvSetMmThreadCharacteristics(pzTaskName, &m_dwTaskIndex);
		}
		TinyScopedAvrt::~TinyScopedAvrt()
		{
			if (m_hMM != NULL)
			{
				AvRevertMmThreadCharacteristics(m_hMM);
				m_hMM = NULL;
			}
		}
		TinyScopedAvrt::operator HANDLE() const
		{
			return m_hMM;
		}
		BOOL TinyScopedAvrt::SetPriority(AVRT_PRIORITY priority)
		{
			ASSERT(m_hMM);
			return AvSetMmThreadPriority(m_hMM, priority);
		}
		//////////////////////////////////////////////////////////////////////////
		AudioObserver::AudioObserver()
		{

		}
		AudioObserver::~AudioObserver()
		{

		}
	};
}
