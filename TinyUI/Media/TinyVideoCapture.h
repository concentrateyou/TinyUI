#pragma once
#include "TinyMedia.h"

namespace TinyUI
{
	namespace Media
	{
		class TinyVideoCapture
		{
		public:
			TinyVideoCapture();
			~TinyVideoCapture();
		public:
			BOOL Initialize();
			BOOL Uninitialize();
		public:
			static BOOL GetDeviceNames(TinyArray<TinyString>& names);
			static BOOL GetDeviceFilter(const string& name, IBaseFilter** ps);
			static void EnumFilter(Callback<BOOL(LPCSTR, IBaseFilter**)>& callback);
		private:
			TinyComPtr<IBaseFilter>		m_baseFilter;
			TinyComPtr<IGraphBuilder>	m_graphBuilder;
			TinyComPtr<IMediaControl>	m_mediaControl;
			TinyComPtr<IMediaEvent>		m_mediaEvent;
		};
	}
}


