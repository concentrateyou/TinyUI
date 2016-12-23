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
	//////////////////////////////////////////////////////////////////////////
	MFVideoCapture::MFVideoCapture()
		:m_bCapturing(FALSE)
	{
		MFT_REGISTER_TYPE_INFO inputFilter = { MFMediaType_Video, MFVideoFormat_MJPG };
		MFT_REGISTER_TYPE_INFO outputFilter = { MFMediaType_Video, MFVideoFormat_ARGB32 };
		IMFActivate** ppActivate = NULL;
		UINT32 numDecodersMJPG = 0;
		HRESULT hRes = MFTEnumEx(MFT_CATEGORY_VIDEO_DECODER, MFT_ENUM_FLAG_ALL, &inputFilter, &outputFilter, &ppActivate, &numDecodersMJPG);
		INT a = 0;
		if (FAILED(hRes))
		{

		}

	}
	MFVideoCapture::~MFVideoCapture()
	{
		Uninitialize();
	}
	void MFVideoCapture::OnFrameReceive(BYTE* bits, LONG size, FLOAT ts, LPVOID lpParameter)
	{
		ASSERT(m_reader);
		TinyAutoLock lock(*this);
		if (m_bCapturing)
		{
			if (!m_receiveCB.IsNull())
			{
				m_receiveCB(bits, size, ts, lpParameter);
			}
			HRESULT hRes = m_reader->ReadSample(static_cast<DWORD>(MF_SOURCE_READER_FIRST_VIDEO_STREAM), 0, NULL, NULL, NULL, NULL);
			if (FAILED(hRes))
			{
				OnError(hRes);
			}
		}
	}
	void MFVideoCapture::OnError(HRESULT hRes)
	{

	}
	BOOL MFVideoCapture::Initialize(const Name& name)
	{
		if (!GetDeviceSource(name, &m_source))
			return FALSE;
		return TRUE;
	}
	BOOL MFVideoCapture::Initialize(const Name& name, Callback<void(BYTE*, LONG, FLOAT, LPVOID)>&& receiveCB)
	{
		if (!GetDeviceSource(name, &m_source))
			return FALSE;
		m_receiveCB = std::move(receiveCB);
		return TRUE;
	}
	void MFVideoCapture::Uninitialize()
	{
		DeAllocate();
		m_reader.Release();
		m_source.Release();
	}
	BOOL MFVideoCapture::Allocate(const MFVideoCaptureParam& param)
	{
		ASSERT(m_source);
		TinyComPtr<IMFAttributes> attributes;
		HRESULT hRes = MFCreateAttributes(&attributes, 1);
		if (FAILED(hRes))
			return FALSE;
		m_readerCB = new MFReaderCallback(this);
		hRes = attributes->SetUnknown(MF_SOURCE_READER_ASYNC_CALLBACK, m_readerCB);
		if (FAILED(hRes))
			return FALSE;
		hRes = MFCreateSourceReaderFromMediaSource(m_source, attributes, &m_reader);
		if (FAILED(hRes))
			return FALSE;
		TinyComPtr<IMFMediaType> mdeiaType;
		hRes = m_reader->GetNativeMediaType(static_cast<DWORD>(MF_SOURCE_READER_FIRST_VIDEO_STREAM), param.GetStreamIndex(), &mdeiaType);
		if (FAILED(hRes))
			return FALSE;
		hRes = m_reader->SetCurrentMediaType(static_cast<DWORD>(MF_SOURCE_READER_FIRST_VIDEO_STREAM), NULL, mdeiaType);
		if (FAILED(hRes))
			return FALSE;
		hRes = m_reader->ReadSample(static_cast<DWORD>(MF_SOURCE_READER_FIRST_VIDEO_STREAM), 0, NULL, NULL, NULL, NULL);
		if (FAILED(hRes))
			return FALSE;
		m_bCapturing = TRUE;
		return TRUE;
	}
	void MFVideoCapture::DeAllocate()
	{
		ASSERT(m_source && m_reader);
		m_reader->Flush(static_cast<DWORD>(MF_SOURCE_READER_ALL_STREAMS));
		m_bCapturing = FALSE;
	}
	BOOL MFVideoCapture::GetDevices(vector<Name>& names)
	{
		TinyComPtr<IMFAttributes> attributes;
		HRESULT hRes = MFCreateAttributes(&attributes, 1);
		if (FAILED(hRes))
			return FALSE;
		hRes = attributes->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE, MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);
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
				SAFE_RELEASE(devices[i]);
				continue;
			}
			string name = WStringToString(wstring(val, size));
			CoTaskMemFree(val);
			hRes = devices[i]->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK, &val, &size);
			if (FAILED(val))
			{
				CoTaskMemFree(val);
				SAFE_RELEASE(devices[i]);
				continue;
			}
			string id = WStringToString(wstring(val, size));
			CoTaskMemFree(val);
			names.push_back(Name(std::move(name), std::move(id)));
			SAFE_RELEASE(devices[i]);
		}
		CoTaskMemFree(devices);
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
			hRes = reader->GetNativeMediaType(static_cast<DWORD>(MF_SOURCE_READER_FIRST_VIDEO_STREAM), dwIndex, &mediaType);
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
		return TRUE;
	}
	BOOL MFVideoCapture::GetDeviceSource(const MFVideoCapture::Name& device, IMFMediaSource** source)
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
		hRes = MFCreateDeviceSource(attributes, source);
		if (FAILED(hRes))
			return FALSE;
		return TRUE;
	}
	BOOL MFVideoCapture::AllowTransform(VideoPixelFormat* src, VideoPixelFormat* dst)
	{
		return TRUE;
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
