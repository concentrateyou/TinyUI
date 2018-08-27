#pragma once
#include "AudioInputStream.h"
#include "AudioOutputStream.h"
#include "AudioParameters.h"
#include "AudioPacket.h"
#include <string>
using namespace std;

namespace MediaSDK
{
	class AudioManager
	{
		DISALLOW_COPY_AND_ASSIGN(AudioManager)
	public:
		AudioManager();
		~AudioManager();
		BOOL Initialize();
	public:
		void Add(AudioInputStream* stream);
		void Add(AudioOutputStream* stream);
		void Remove(AudioInputStream* stream);
		void Remove(AudioOutputStream* stream);
	public:
		static string			GetDefaultInputID();
		static string			GetDefaultOutputID();
		static string			GetCommunicationsInputID();
		static string			GetCommunicationsOutputID();
		static IMMDevice*		GetDefaultDevice(EDataFlow dataFlow, ERole role);
		static WAVEFORMATEX*	GetMixFormat(const string& deviceID);
	private:
		static string GetDeviceID(EDataFlow dataFlow, ERole role);
		void OnChange(LPCWSTR, DWORD);
	private:
		AudioClientListener				m_listener;
		TinyArray<AudioInputStream*>	m_inputs;
		TinyArray<AudioOutputStream*>	m_outputs;
	};
}



