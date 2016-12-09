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
		public:
			static BOOL GetDevices(vector<Name>& names);
			static BOOL GetDeviceFormats(const Name& name, AUDCLNT_SHAREMODE shareMode, vector<WAVEFORMATEX>& s);
		};
	}
}
