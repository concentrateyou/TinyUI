#pragma once
#include "../Common/TinyEvent.h"
#include "../Common/TinyString.h"
#include "../Common/TinyCallback.h"
#include <mmsystem.h>
#include <dshow.h>
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "strmiids.lib")

namespace TinyUI
{
	namespace Media
	{
		/// <summary>
		///  ”∆µ≤∂ªÒ¿‡
		/// </summary>
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


