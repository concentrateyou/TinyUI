#include "../stdafx.h"
#include "TinyMFAACEncode.h"

namespace TinyUI
{
	namespace Media
	{
		TinyMFAACEncode::TinyMFAACEncode()
		{

		}
		TinyMFAACEncode::~TinyMFAACEncode()
		{

		}
		BOOL TinyMFAACEncode::Initialize(const WAVEFORMATEX* pFMTI, const WAVEFORMATEX* pFMTO)
		{
			TinyComPtr<IUnknown> unknow;
			HRESULT hRes = unknow.CoCreateInstance(CLSID_AACMFTEncoder, NULL, CLSCTX_INPROC_SERVER);
			if (hRes != S_OK)
				return FALSE;
			hRes = unknow->QueryInterface(IID_PPV_ARGS(&m_transform));
			if (hRes != S_OK)
				return FALSE;
			hRes = MFCreateMediaType(&m_inputType);
			if (hRes != S_OK)
				return FALSE;
			hRes = MFCreateMediaType(&m_outputType);
			if (hRes != S_OK)
				return FALSE;
			hRes = m_transform->SetInputType(0, m_inputType, 0);
			if (hRes != S_OK)
				return FALSE;
			hRes = m_transform->SetOutputType(0, m_outputType, 0);
			if (hRes != S_OK)
				return FALSE;
			return TRUE;
		}
		BOOL TinyMFAACEncode::Open()
		{
			HRESULT hRes = m_transform->ProcessMessage(MFT_MESSAGE_NOTIFY_BEGIN_STREAMING, NULL);
			if (hRes != S_OK)
				return FALSE;
			hRes = m_transform->ProcessMessage(MFT_MESSAGE_NOTIFY_START_OF_STREAM, NULL);
			if (hRes != S_OK)
				return FALSE;
			return TRUE;
		}
		BOOL TinyMFAACEncode::Close()
		{
			HRESULT hRes = m_transform->ProcessMessage(MFT_MESSAGE_NOTIFY_END_STREAMING, NULL);
			if (hRes != S_OK)
				return FALSE;
			hRes = m_transform->ProcessMessage(MFT_MESSAGE_NOTIFY_END_OF_STREAM, NULL);
			if (hRes != S_OK)
				return FALSE;
			return TRUE;
		}
	};
}
