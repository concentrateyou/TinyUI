#include "stdafx.h"
#include "WASAPIAudioInputStream.h"

namespace MediaSDK
{

	WASAPIAudioInputStream::WASAPIAudioInputStream()
	{

	}
	WASAPIAudioInputStream::~WASAPIAudioInputStream()
	{

	}

	BOOL WASAPIAudioInputStream::Initialize(const AudioParameters& params, const string& deviceID)
	{
		return FALSE;
	}

	BOOL WASAPIAudioInputStream::Open()
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	BOOL WASAPIAudioInputStream::Start(AudioOutputCallback* callback)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	BOOL WASAPIAudioInputStream::Stop()
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void WASAPIAudioInputStream::Close()
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	BOOL WASAPIAudioInputStream::SetVolume(DOUBLE volume)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	BOOL WASAPIAudioInputStream::GetVolume(DOUBLE* volume)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

}