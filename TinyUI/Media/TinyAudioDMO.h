#pragma once
#include "TinyMedia.h"
#include "TinyWave.h"
#include <dmort.h>

namespace TinyUI
{
	namespace Media
	{
		/// <summary>
		/// »ØÉùÏû³ý()
		/// </summary>
		class TinyAudioDMO
		{
			DISALLOW_COPY_AND_ASSIGN(TinyAudioDMO)
		public:
			TinyAudioDMO();
			~TinyAudioDMO();
		public:
			BOOL Initialize(const WAVEFORMATEX* pFMT);
			BOOL Open();
			BOOL Close();
		private:
			BOOL SetVtI4Property(IPropertyStore* ptrPS, REFPROPERTYKEY key, LONG value);
			BOOL SetBoolProperty(IPropertyStore* ptrPS, REFPROPERTYKEY key, VARIANT_BOOL value);
		private:
			TinyComPtr<IMediaObject>	m_dmo;
			TinyComPtr<IPropertyStore>	m_propertyStore;
			DMO_MEDIA_TYPE				m_mediaType;
		};
	};
}
