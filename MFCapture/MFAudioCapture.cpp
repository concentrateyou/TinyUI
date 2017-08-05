#include "stdafx.h"
#include "MFAudioCapture.h"

namespace MF
{
	MFAudioCapture::Name::Name()
	{

	}
	MFAudioCapture::Name::Name(string&& name, string&& id)
		:m_name(std::move(name)),
		m_id(std::move(id))
	{

	}
	MFAudioCapture::Name::Name(const string& name, const string& id)
		: m_name(name),
		m_id(id)
	{

	}
	MFAudioCapture::Name::~Name()
	{

	}
	const string& MFAudioCapture::Name::name() const
	{
		return m_name;
	}
	const string& MFAudioCapture::Name::id() const
	{
		return m_id;
	}
	//////////////////////////////////////////////////////////////////////////
	MFAudioCapture::MFAudioCapture()
		:m_bCapturing(FALSE)
	{


	}
	MFAudioCapture::~MFAudioCapture()
	{
		Uninitialize();
	}
	void MFAudioCapture::OnFrameReceive(BYTE* bits, LONG size, FLOAT ts, LPVOID lpParameter)
	{
		TinyAutoLock lock(*this);
		if (m_bCapturing)
		{
			if (!m_receiveCB.IsNull())
			{
				m_receiveCB(bits, size, ts, lpParameter);
			}
			if (m_reader)
			{
				m_reader->ReadSample(static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), 0, NULL, NULL, NULL, NULL);
			}
		}
	}
	BOOL MFAudioCapture::Initialize(const Name& name)
	{
		if (!GetDeviceSource(name, &m_source))
			return FALSE;
		return TRUE;
	}
	BOOL MFAudioCapture::Initialize(const Name& name, Callback<void(BYTE*, LONG, FLOAT, LPVOID)>&& receiveCB)
	{
		if (!GetDeviceSource(name, &m_source))
			return FALSE;
		m_receiveCB = std::move(receiveCB);
		return TRUE;
	}
	void MFAudioCapture::Uninitialize()
	{
		Deallocate();
		m_reader.Release();
		if (m_source != NULL)
			m_source->Shutdown();
		m_source.Release();
	}
	BOOL MFAudioCapture::Allocate(const MFAudioCaptureParam& param)
	{
		ASSERT(m_source);
		TinyComPtr<IMFAttributes> attributes;
		HRESULT hRes = MFCreateAttributes(&attributes, 1);
		if (hRes != S_OK)
			return FALSE;
		m_readerCB = new MFAudioReaderCallback(this);
		hRes = attributes->SetUnknown(MF_SOURCE_READER_ASYNC_CALLBACK, m_readerCB);
		if (hRes != S_OK)
			return FALSE;
		hRes = MFCreateSourceReaderFromMediaSource(m_source, attributes, &m_reader);
		if (hRes != S_OK)
			return FALSE;
		TinyComPtr<IMFMediaType> mdeiaType;
		hRes = m_reader->GetNativeMediaType(static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), param.GetStreamIndex(), &mdeiaType);
		if (hRes != S_OK)
			return FALSE;
		hRes = m_reader->SetCurrentMediaType(static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), NULL, mdeiaType);
		if (hRes != S_OK)
			return FALSE;
		hRes = m_reader->ReadSample(static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), 0, NULL, NULL, NULL, NULL);
		if (hRes != S_OK)
			return FALSE;
		m_bCapturing = TRUE;
		return TRUE;
	}
	void MFAudioCapture::Deallocate()
	{
		if (m_reader)
			m_reader->Flush(static_cast<DWORD>(MF_SOURCE_READER_ALL_STREAMS));
		m_bCapturing = FALSE;
	}
	BOOL MFAudioCapture::GetDevices(vector<Name>& names)
	{
		TinyComPtr<IMFAttributes> attributes;
		HRESULT hRes = MFCreateAttributes(&attributes, 1);
		if (hRes != S_OK)
			return FALSE;
		hRes = attributes->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE, MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_GUID);
		if (hRes != S_OK)
			return FALSE;
		IMFActivate** activates = NULL;
		UINT32 count = 0;
		hRes = MFEnumDeviceSources(attributes, &activates, &count);
		if (hRes != S_OK)
			return FALSE;
		for (UINT32 i = 0; i < count; ++i)
		{
			WCHAR* val = NULL;
			UINT32 size = 0;
			hRes = activates[i]->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME, &val, &size);
			if (FAILED(val))
			{
				CoTaskMemFree(val);
				SAFE_RELEASE(activates[i]);
				continue;
			}
			string name = WStringToString(wstring(val, size));
			CoTaskMemFree(val);
			hRes = activates[i]->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_SYMBOLIC_LINK, &val, &size);
			if (FAILED(val))
			{
				CoTaskMemFree(val);
				SAFE_RELEASE(activates[i]);
				continue;
			}
			string id = WStringToString(wstring(val, size));
			CoTaskMemFree(val);
			names.push_back(Name(std::move(name), std::move(id)));
			SAFE_RELEASE(activates[i]);
		}
		CoTaskMemFree(activates);
		return TRUE;
	}
	BOOL MFAudioCapture::GetDeviceParams(const MFAudioCapture::Name& device, vector<MFAudioCaptureParam>& params)
	{
		TinyComPtr<IMFAttributes> attributes;
		HRESULT hRes = MFCreateAttributes(&attributes, 2);
		if (hRes != S_OK)
			return FALSE;
		hRes = attributes->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE, MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_GUID);
		if (hRes != S_OK)
			return FALSE;
		hRes = attributes->SetString(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_SYMBOLIC_LINK, StringToWString(device.id(), CP_UTF8).c_str());
		if (hRes != S_OK)
			return FALSE;
		TinyComPtr<IMFMediaSource> source;
		BOOL fSelect = FALSE;
		hRes = MFCreateDeviceSource(attributes, &source);//这个函数有内存泄漏
		if (hRes != S_OK)
			return FALSE;
		TinyComPtr<IMFSourceReader> reader;
		hRes = MFCreateSourceReaderFromMediaSource(source, NULL, &reader);
		if (hRes != S_OK)
			goto MFERROR;
		for (DWORD dwIndex = 0;;dwIndex++)
		{
			TinyComPtr<IMFMediaType> mediaType;
			hRes = reader->GetNativeMediaType(static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), dwIndex, &mediaType);
			if (hRes != S_OK)
				break;
			UINT32 cx, cy;
			hRes = MFGetAttributeSize(mediaType, MF_MT_FRAME_SIZE, &cx, &cy);
			if (hRes != S_OK)
				goto MFERROR;
			MFAudioCaptureParam param;
			param.SetSize(cx, cy);
			UINT32 numerator, denominator;
			hRes = MFGetAttributeRatio(mediaType, MF_MT_FRAME_RATE, &numerator, &denominator);
			if (hRes != S_OK)
				goto MFERROR;
			param.SetRate(denominator ? static_cast<FLOAT>(numerator) / denominator : 0.0F);
			GUID guid;
			hRes = mediaType->GetGUID(MF_MT_SUBTYPE, &guid);
			if (hRes != S_OK)
				goto MFERROR;
			VideoPixelFormat format;
			if (!GetFormat(guid, &format))
				goto MFERROR;
			param.SetFormat(format);
			params.push_back(param);
		}
	MFERROR:
		source->Shutdown();
		return SUCCEEDED(hRes);
	}
	BOOL MFAudioCapture::GetDeviceSource(const MFAudioCapture::Name& device, IMFMediaSource** source)
	{
		TinyComPtr<IMFAttributes> attributes;
		HRESULT hRes = MFCreateAttributes(&attributes, 2);
		if (hRes != S_OK)
			return FALSE;
		hRes = attributes->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE, MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_GUID);
		if (hRes != S_OK)
			return FALSE;
		hRes = attributes->SetString(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_SYMBOLIC_LINK, StringToWString(device.id(), CP_UTF8).c_str());
		if (hRes != S_OK)
			return FALSE;
		hRes = MFCreateDeviceSource(attributes, source);
		if (hRes != S_OK)
			return FALSE;
		return TRUE;
	}
	BOOL MFAudioCapture::GetFormat(const GUID& guid, VideoPixelFormat* format)
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
