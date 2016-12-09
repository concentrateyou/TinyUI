#pragma once
#include "TinyMedia.h"

namespace TinyUI
{
	namespace Media
	{
		class TinyWASAPIAudio
		{
			DISALLOW_COPY_AND_ASSIGN(TinyWASAPIAudio)
		public:
			class Name
			{
			public:
				Name();
				Name(string&& id, string&& name);
				~Name();
				const string& name() const;
				const string& id() const;
			private:
				string		m_name;
				string		m_id;
			};
		public:
			TinyWASAPIAudio();
			virtual ~TinyWASAPIAudio();
			virtual	void Initialize(DWORD dwFlag, AUDCLNT_SHAREMODE shareMode, Callback<void(BYTE*, LONG, LPVOID)>&& callback);
		public:
			static BOOL GetDevices(vector<Name>& names);
			static BOOL GetDeviceFormats(const Name& name, AUDCLNT_SHAREMODE shareMode, vector<WAVEFORMATEX>& s);
		protected:
			AUDCLNT_SHAREMODE					m_shareMode;
			DWORD								m_dwStreamFlag;
			Callback<void(BYTE*, LONG, LPVOID)>	m_callback;
		};
	}
}
