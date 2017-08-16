#include "stdafx.h"
#include "MAudioCapture.h"
#include "Media/TinyWASAPIAudio.h"

namespace MShow
{
	MAudioCapture::MAudioCapture()
	{
	}

	MAudioCapture::~MAudioCapture()
	{
	}
	WAVEFORMATEX MAudioCapture::GetFormat()
	{
		return m_param.GetFormat();
	}
	BOOL MAudioCapture::Initialize()
	{
		vector<AudioCapture::Name> names;
		if (!AudioCapture::GetDevices(names))
			return FALSE;
		BOOL bIsMA = FALSE;
		for (INT i = 0;i < names.size();i++)
		{
			if (!Media::TinyWASAPIAudio::IsMicrophoneArray(names[i].name(), bIsMA))
				return FALSE;
			LOG(INFO) << "MAudioCapture IsMicrophoneArray Name:" << names[i].name() << " IsMA:" << bIsMA << "\n";
			if (!bIsMA)
			{
				if (!Media::TinyWASAPIAudio::IsMicrophone(names[i].name(), bIsMA))
					return FALSE;
				LOG(INFO) << "MAudioCapture IsMicrophone Name:" << names[i].name() << " IsMA:" << bIsMA << "\n";
			}
			if (!bIsMA)
				return FALSE;
			vector<AudioCaptureParam> params;
			if (!AudioCapture::GetDeviceParams(names[i], params))
				return FALSE;
			for (INT j = 0;j < params.size();j++)
			{
				WAVEFORMATEX waveFMT = params[j].GetFormat();
				if (waveFMT.nChannels == 2 &&
					waveFMT.nSamplesPerSec == 44100
					&& waveFMT.wBitsPerSample == 16)
				{
					m_name = names[i];
					m_param = params[j];
					LOG(INFO) << "MAudioCapture Initialize:" << names[i].name() << "\n";
					return TRUE;
				}
			}
		}
		LOG(INFO) << "MAudioCapture Initialize Fail\n";
		return FALSE;
	}

	BOOL MAudioCapture::Open(Callback<void(BYTE*, LONG, FLOAT, LPVOID)>&& callback)
	{
		this->Close();
		if (!m_capture.Initialize(m_name, std::move(callback)))
			return FALSE;
		if (!m_capture.Allocate(m_param))
			return FALSE;
		return TRUE;
	}
	void MAudioCapture::Close()
	{
		m_capture.Deallocate();
		m_capture.Uninitialize();
	}

	BOOL MAudioCapture::Start()
	{
		return m_capture.Start();
	}

	BOOL MAudioCapture::Stop()
	{
		return m_capture.Stop();
	}

}


