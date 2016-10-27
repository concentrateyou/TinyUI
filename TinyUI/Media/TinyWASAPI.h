#pragma once
#include "../Common/TinyEvent.h"
#include "../Common/TinyString.h"
#include "../Common/TinyCallback.h"
#include <MMDeviceAPI.h>
#include <AudioClient.h>
#include <AudioPolicy.h>
#include <avrt.h>
#include <functiondiscoverykeys.h>

namespace TinyUI
{
	namespace Media
	{
		class TinyWASAPI
		{
			DISALLOW_COPY_AND_ASSIGN(TinyWASAPI);
		public:
			class Name
			{
			public:
				Name();
				Name(string&& name, string&& id);
				Name(const string& name, const string& id);
				~Name();
				const string& name() const;
				const string& id() const;
			private:
				string	m_name;
				string	m_id;
			};
		public:
			TinyWASAPI();
			~TinyWASAPI();
		public:
			static BOOL GetDevices(vector<Name>& names);
		};
	};
}
