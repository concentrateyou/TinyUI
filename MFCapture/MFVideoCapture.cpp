#include "stdafx.h"
#include "MFVideoCapture.h"

namespace MF
{

	MFVideoCapture::Name::Name()
	{

	}
	MFVideoCapture::Name::Name(string&& name, string&& id)
		:m_name(std::move(name)),
		m_id(std::move(id))
	{

	}
	MFVideoCapture::Name::Name(const string& name, const string& id)
		: m_name(name),
		m_id(id)
	{

	}
	MFVideoCapture::Name::~Name()
	{

	}
	const string& MFVideoCapture::Name::name() const
	{
		return m_name;
	}
	const string& MFVideoCapture::Name::id() const
	{
		return m_id;
	}
	void MFVideoCapture::OnFrameReceive(BYTE* bits, LONG size, FLOAT ts, LPVOID lpParameter)
	{

	}
	BOOL MFVideoCapture::Initialize(const Name& name)
	{
		return TRUE;
	}
	BOOL MFVideoCapture::Initialize(const Name& name, Callback<void(BYTE*, LONG, FLOAT, LPVOID)>& receiveCB)
	{
		return TRUE;
	}
	void MFVideoCapture::Uninitialize()
	{

	}
	BOOL MFVideoCapture::Allocate()
	{
		TinyComPtr<IMFAttributes> attributes;
		HRESULT hRes = MFCreateAttributes(&attributes, 1);
		if (FAILED(hRes))
			return FALSE;
		m_readerCB = new MFReaderCallback(this);
		attributes->SetUnknown(MF_SOURCE_READER_ASYNC_CALLBACK, m_readerCB);
		return TRUE;
	}
	void MFVideoCapture::DeAllocate()
	{

	}
	BOOL MFVideoCapture::GetDevices(vector<Name>& names)
	{
		TinyComPtr<IMFAttributes> attributes;
		HRESULT hRes = MFCreateAttributes(&attributes, 1);
		if (FAILED(hRes))
			return FALSE;
		IMFActivate** devices = NULL;
		UINT32 count = 0;
		hRes = MFEnumDeviceSources(attributes, &devices, &count);
		if (FAILED(hRes))
			return FALSE;
		for (UINT32 i = 0; i < count; ++i)
		{
			WCHAR* val = NULL;
			UINT32 size = 0;
			hRes = devices[i]->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME, &val, &size);
			if (FAILED(val))
			{
				CoTaskMemFree(val);
				continue;
			}
			string name = WStringToString(wstring(val, size));
			CoTaskMemFree(val);
			hRes = devices[i]->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK, &val, &size);
			CoTaskMemFree(val);
			if (FAILED(val))
			{
				CoTaskMemFree(val);
				continue;
			}
			string id = WStringToString(wstring(val, size));
			CoTaskMemFree(val);
			names.push_back(Name(std::move(name), std::move(id)));
		}
		return TRUE;
	}
	BOOL MFVideoCapture::GetDeviceParams(const MFVideoCapture::Name& device, vector<MFVideoCaptureParam>& params)
	{
		TinyComPtr<IMFAttributes> attributes;
		HRESULT hRes = MFCreateAttributes(&attributes, 2);
		if (FAILED(hRes))
			return FALSE;
		hRes = attributes->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE, MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);
		if (FAILED(hRes))
			return FALSE;
		hRes = attributes->SetString(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK, StringToWString(device.id()).c_str());
		if (FAILED(hRes))
			return FALSE;
		TinyComPtr<IMFMediaSource> source;
		hRes = MFCreateDeviceSource(attributes, &source);
		if (FAILED(hRes))
			return FALSE;
		TinyComPtr<IMFSourceReader> reader;
		hRes = MFCreateSourceReaderFromMediaSource(source, NULL, &reader);
		if (FAILED(hRes))
			return FALSE;
		DWORD dwIndex = 0;
		TinyComPtr<IMFMediaType> mediaType;
		for (;;)
		{
			hRes = reader->GetNativeMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, dwIndex, &mediaType);
			if (FAILED(hRes))
				return FALSE;
			UINT32 cx, cy;
			hRes = MFGetAttributeSize(mediaType, MF_MT_FRAME_SIZE, &cx, &cy);
			if (FAILED(hRes))
			{
				++dwIndex;
				continue;
			}
			MFVideoCaptureParam param;
			param.SetSize(cx, cy);
			UINT32 numerator, denominator;
			hRes = MFGetAttributeRatio(mediaType, MF_MT_FRAME_RATE, &numerator, &denominator);
			if (FAILED(hRes))
			{
				++dwIndex;
				continue;
			}
			param.SetRate(denominator ? static_cast<FLOAT>(numerator) / denominator : 0.0F);
			GUID guid;
			hRes = mediaType->GetGUID(MF_MT_SUBTYPE, &guid);
			if (FAILED(hRes))
			{
				++dwIndex;
				continue;
			}
			VideoPixelFormat format;
			if (!GetFormat(guid, &format))
			{
				++dwIndex;
				continue;
			}
			param.SetFormat(format);
			params.push_back(param);
			++dwIndex;
		}
	}
	BOOL MFVideoCapture::GetFormat(const GUID& guid, VideoPixelFormat* format)
	{
		struct
		{
			const GUID& guid;
			const VideoPixelFormat format;
		}
		static const FormatMap[] =
		{
			{ MFVideoFormat_I420, PIXEL_FORMAT_I420 },
			{ MFVideoFormat_YUY2, PIXEL_FORMAT_YUY2 },
			{ MFVideoFormat_UYVY, PIXEL_FORMAT_UYVY },
			{ MFVideoFormat_RGB24, PIXEL_FORMAT_RGB24 },
			{ MFVideoFormat_ARGB32, PIXEL_FORMAT_ARGB },
			{ MFVideoFormat_MJPG, PIXEL_FORMAT_MJPEG },
			{ MFVideoFormat_YV12, PIXEL_FORMAT_YV12 },
		};

		for (INT i = 0; i < arraysize(FormatMap); ++i)
		{
			if (FormatMap[i].guid == guid)
			{
				*format = FormatMap[i].format;
				return TRUE;
			}
		}
		return FALSE;
	}
}
