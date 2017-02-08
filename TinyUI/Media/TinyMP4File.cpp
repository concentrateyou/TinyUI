#include "TinyMP4File.h"
#include <propvarutil.h>

namespace TinyUI
{
	namespace Media
	{
		TinyMP4File::TinyMP4File()
		{

		}
		TinyMP4File::~TinyMP4File()
		{

		}
		BOOL TinyMP4File::Create(LPTSTR pzFile)
		{
			if (!pzFile)
				return FALSE;
			wstring str = StringToWString(pzFile);
			TinyComPtr<IMFAttributes> attr;
			HRESULT hRes = MFCreateAttributes(&attr, 1);
			if (FAILED(hRes))
				return FALSE;
			hRes = attr->SetUINT32(MF_READWRITE_ENABLE_HARDWARE_TRANSFORMS, 1);
			if (FAILED(hRes))
				return FALSE;
			hRes = attr->SetUINT32(MF_SINK_WRITER_DISABLE_THROTTLING, 1);
			if (FAILED(hRes))
				return FALSE;
			hRes = MFCreateSinkWriterFromURL(str.c_str(), NULL, attr, &m_writer);
			if (FAILED(hRes))
				return FALSE;
			TinyComPtr<IMFMediaType> mediaType;
			hRes = MFCreateMediaType(&mediaType);
			if (FAILED(hRes))
				return FALSE;
			hRes = mediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
			if (FAILED(hRes))
				return FALSE;
			return TRUE;
		}
		BOOL TinyMP4File::Close()
		{
			if (m_writer != NULL)
			{
				m_writer->Flush(0);
				m_writer->Finalize();
				m_writer.Release();
			}
			return TRUE;
		}
	}
}