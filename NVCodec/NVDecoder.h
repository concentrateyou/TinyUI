#pragma once
#include "NVCodecCommon.h"

namespace NVCodec
{
	class NVDecoder
	{
		DISALLOW_COPY_AND_ASSIGN(NVDecoder)
	public:
		NVDecoder();
		virtual ~NVDecoder();
		BOOL Open(const TinySize& videoSize, const BYTE* bits, LONG size);
		BOOL Decode(Media::SampleTag& tag);
		void Close();
	private:
		//BOOL InitializeD3D9();
	private:
		BOOL							m_bInterop;
		INT								m_deviceID;
		CUVIDPARSERDISPINFO*			m_parserINFO;
		CUVIDDECODECREATEINFO			m_createINFO;
		CUVIDEOFORMAT					m_videoFMT;
		CUvideoctxlock					m_videolock;
		CUcontext						m_context;
		CUvideoparser					m_parser;
		CUvideodecoder					m_decoder;
		TinyComPtr<IDirect3D9>			m_d3d9;
		TinyComPtr<IDirect3DDevice9>	m_d3dd9;
	};
}


