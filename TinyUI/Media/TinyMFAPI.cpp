#include "../stdafx.h"
#include "TinyMFAPI.h"

namespace TinyUI
{
	namespace Media
	{
		HRESULT TinyMFAPI::GetAudioDecoder(const GUID& subtype, IMFTransform **ppDecoder)
		{
			HRESULT hRes = S_OK;
			UINT32 count = 0;
			CLSID *ppCLSIDs = NULL;
			MFT_REGISTER_TYPE_INFO info = { 0 };
			info.guidMajorType = MFMediaType_Audio;
			info.guidSubtype = subtype;
			hRes = MFTEnum(
				MFT_CATEGORY_AUDIO_DECODER,
				0,
				&info,
				NULL,
				NULL,
				&ppCLSIDs,
				&count
			);
			if (SUCCEEDED(hRes) && count == 0)
			{
				hRes = MF_E_TOPO_CODEC_NOT_FOUND;
			}
			if (SUCCEEDED(hRes))
			{
				hRes = CoCreateInstance(ppCLSIDs[0], NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(ppDecoder));
			}
			CoTaskMemFree(ppCLSIDs);
			return hRes;
		}
		HRESULT TinyMFAPI::GetAudioEncoder(const GUID& subtype, IMFTransform **ppEncoder)
		{
			HRESULT hRes = S_OK;
			UINT32 count = 0;
			CLSID *ppCLSIDs = NULL;
			MFT_REGISTER_TYPE_INFO info = { 0 };
			info.guidMajorType = MFMediaType_Audio;
			info.guidSubtype = subtype;
			hRes = MFTEnum(
				MFT_CATEGORY_AUDIO_ENCODER,
				0,
				&info,
				NULL,
				NULL,
				&ppCLSIDs,
				&count
			);
			if (SUCCEEDED(hRes) && count == 0)
			{
				hRes = MF_E_TOPO_CODEC_NOT_FOUND;
			}
			if (SUCCEEDED(hRes))
			{
				hRes = CoCreateInstance(ppCLSIDs[0], NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(ppEncoder));
			}
			CoTaskMemFree(ppCLSIDs);
			return hRes;
		}
		HRESULT TinyMFAPI::GetVideoDecoder(const GUID& subtype, IMFTransform **ppDecoder)
		{
			HRESULT hRes = S_OK;
			UINT32 count = 0;
			CLSID *ppCLSIDs = NULL;
			MFT_REGISTER_TYPE_INFO info = { 0 };
			info.guidMajorType = MFMediaType_Video;
			info.guidSubtype = subtype;
			hRes = MFTEnum(
				MFT_CATEGORY_VIDEO_DECODER,
				0,
				&info,
				NULL,
				NULL,
				&ppCLSIDs,
				&count
			);
			if (SUCCEEDED(hRes) && count == 0)
			{
				hRes = MF_E_TOPO_CODEC_NOT_FOUND;
			}
			if (SUCCEEDED(hRes))
			{
				hRes = CoCreateInstance(ppCLSIDs[0], NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(ppDecoder));
			}
			CoTaskMemFree(ppCLSIDs);
			return hRes;
		}
		HRESULT TinyMFAPI::GetVideoEncoder(const GUID& subtype, IMFTransform **ppEncoder)
		{
			HRESULT hRes = S_OK;
			UINT32 count = 0;
			CLSID *ppCLSIDs = NULL;
			MFT_REGISTER_TYPE_INFO info = { 0 };
			info.guidMajorType = MFMediaType_Video;
			info.guidSubtype = subtype;
			hRes = MFTEnum(
				MFT_CATEGORY_VIDEO_ENCODER,
				0,
				&info,
				NULL,
				NULL,
				&ppCLSIDs,
				&count
			);
			if (SUCCEEDED(hRes) && count == 0)
			{
				hRes = MF_E_TOPO_CODEC_NOT_FOUND;
			}
			if (SUCCEEDED(hRes))
			{
				hRes = CoCreateInstance(ppCLSIDs[0], NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(ppEncoder));
			}
			CoTaskMemFree(ppCLSIDs);
			return hRes;
		}
	};
}
