#pragma once
#include "TinyMFAPI.h"

namespace TinyUI
{
	namespace Media
	{
		/// <summary>
		/// �ز���
		/// </summary>
		class TinyResampler
		{
		public:
			TinyResampler();
			~TinyResampler();
		public:
			BOOL Initialize(const WAVEFORMATEX* pTransformFrom, const WAVEFORMATEX* pTransformTo);
			BOOL ProcessInput(BYTE* bits, LONG size);
		private:
			TinyComPtr<IMFTransform> m_transform;
		};
	};
}
