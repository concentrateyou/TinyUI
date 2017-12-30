#pragma once
#include "TinySocket.h"
extern "C"
{
#include "zlib.h"
}
#include <algorithm>
using namespace std;

namespace TinyUI
{
	namespace Network
	{
#define GZIP_ZLIB_WINDOWSIZE 15
#define GZIP_ENCODING		 16
#define GZIP_ZLIB_CFACTOR    9
#define GZIP_ZLIB_BSIZE      8096
#define GZIP_ZLIB_CHUNK		16384
		/// <summary>
		/// GZIP½âÎö https://tools.ietf.org/html/rfc1952
		/// </summary>
		class TinyGZip
		{
			DISALLOW_COPY_AND_ASSIGN(TinyGZip)
		public:
			TinyGZip();
			~TinyGZip();
			BOOL IsGZip(CHAR* bits, LONG size);
			BOOL GZipCompress(CHAR* bits, LONG size, TinyBufferArray<CHAR>& bufferIO);
			BOOL GZipDecompress(CHAR* bits, LONG size, TinyBufferArray<CHAR>& bufferIO);
		private:
			enum
			{
				FLAG_FTEXT = 0x01,
				FLAG_FHCRC = 0x02,
				FLAG_FEXTRA = 0x04,
				FLAG_FNAME = 0x08,
				FLAG_FCOMMENT = 0x10,
				FLAG_RESERVED = 0xE0,
			};

			enum GZIP_MEMBER_HEADER
			{
				IN_HEADER_ID1,
				IN_HEADER_ID2,
				IN_HEADER_CM,
				IN_HEADER_FLG,
				IN_HEADER_MTIME_BYTE_0,
				IN_HEADER_MTIME_BYTE_1,
				IN_HEADER_MTIME_BYTE_2,
				IN_HEADER_MTIME_BYTE_3,
				IN_HEADER_XFL,
				IN_HEADER_OS,
				IN_XLEN_BYTE_0,
				IN_XLEN_BYTE_1,
				IN_FEXTRA,
				IN_FNAME,
				IN_FCOMMENT,
				IN_FHCRC_BYTE_0,
				IN_FHCRC_BYTE_1,
				IN_DONE,
			};
			static const BYTE magic[];
			INT		m_state;
			SHORT	m_size;
			BYTE	m_flags;
		};
	}
}

