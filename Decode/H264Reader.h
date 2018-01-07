#pragma once
#include "Common.h"
#include <vector>
using namespace std;
using namespace TinyUI;

namespace Decode
{
#define DEFAULT_H264_BUFFER_SIZE (1024*8)

	BOOL FindStartCode(BYTE* bits, INT size);
	typedef struct tagNALU
	{
		BYTE	Forbidden;
		BYTE	Reference;
		BYTE	Type;
		BYTE*	bits;
		LONG	size;
	}NALU;
	/// <summary>
	/// H264∂¡»°IO
	/// </summary>
	class H264Reader
	{
		DISALLOW_COPY_AND_ASSIGN(H264Reader)
	public:
		H264Reader();
		virtual ~H264Reader();
		BOOL	Open(LPCSTR pzFile);
		BOOL	ReadNALU(NALU& nalu);
		BOOL	Close();
	private:
		LONG				m_count;
		GrowableIOBuffer	m_io;
		TinyComPtr<IStream> m_stream;
	};

}
