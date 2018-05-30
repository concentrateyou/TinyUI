#include "../stdafx.h"
#include "TinyGZip.h"

namespace TinyUI
{
	namespace Network
	{
		const BYTE TinyGZip::magic[] = { 0x1f, 0x8b };

		TinyGZip::TinyGZip()
			:m_state(IN_HEADER_ID1),
			m_flags(0),
			m_size(0)
		{

		}
		TinyGZip::~TinyGZip()
		{

		}
		BOOL TinyGZip::IsGZip(CHAR* bits, LONG size)
		{
			const BYTE* pos = reinterpret_cast<const BYTE*>(bits);
			const BYTE* const end = pos + size;
			while (pos < end)
			{
				switch (m_state)
				{
				case IN_HEADER_ID1:
					if (*pos != magic[0])
						return FALSE;
					pos++;
					m_state++;
					break;
				case IN_HEADER_ID2:
					if (*pos != magic[1])
						return FALSE;
					pos++;
					m_state++;
					break;
				case IN_HEADER_CM:
					if (*pos != Z_DEFLATED)
						return FALSE;
					pos++;
					m_state++;
					break;
				case IN_HEADER_FLG:
					m_flags = (*pos) & (FLAG_FHCRC | FLAG_FEXTRA | FLAG_FNAME | FLAG_FCOMMENT);
					pos++;
					m_state++;
					break;
				case IN_HEADER_MTIME_BYTE_0:
					pos++;
					m_state++;
					break;
				case IN_HEADER_MTIME_BYTE_1:
					pos++;
					m_state++;
					break;
				case IN_HEADER_MTIME_BYTE_2:
					pos++;
					m_state++;
					break;
				case IN_HEADER_MTIME_BYTE_3:
					pos++;
					m_state++;
					break;
				case IN_HEADER_XFL:
					pos++;
					m_state++;
					break;
				case IN_HEADER_OS:
					pos++;
					m_state++;
					break;
				case IN_XLEN_BYTE_0:
					if (!(m_flags & FLAG_FEXTRA))
					{
						m_state = IN_FNAME;
						break;
					}
					m_size = *pos;
					pos++;
					m_state++;
					break;
				case IN_XLEN_BYTE_1:
					m_size += *pos << 8;
					pos++;
					m_state++;
				case IN_FEXTRA:
				{
					const uint16_t num_extra_bytes = static_cast<uint16_t>(min(static_cast<ptrdiff_t>(m_size), (end - pos)));
					pos += num_extra_bytes;
					m_size -= num_extra_bytes;
					if (m_size == 0)
					{
						m_state = IN_FNAME;
						m_flags &= ~FLAG_FEXTRA;
					}
					break;
				}
				case IN_FNAME:
					if (!(m_flags & FLAG_FNAME))
					{
						m_state = IN_FCOMMENT;
						break;
					}
					pos = reinterpret_cast<const uint8_t*>(memchr(pos, '\0', (end - pos)));
					if (pos != NULL)
					{
						pos++;
						m_flags &= ~FLAG_FNAME;
						m_state = IN_FCOMMENT;
					}
					else
					{
						pos = end;
					}
					break;

				case IN_FCOMMENT:
					if (!(m_flags & FLAG_FCOMMENT))
					{
						m_state = IN_FHCRC_BYTE_0;
						break;
					}
					pos = reinterpret_cast<const uint8_t*>(memchr(pos, '\0', (end - pos)));
					if (pos != NULL)
					{
						pos++;
						m_flags &= ~FLAG_FCOMMENT;
						m_state = IN_FHCRC_BYTE_0;
					}
					else
					{
						pos = end;
					}
					break;

				case IN_FHCRC_BYTE_0:
					if (!(m_flags & FLAG_FHCRC))
					{
						m_state = IN_DONE;
						break;
					}
					pos++;
					m_state++;
					break;
				case IN_FHCRC_BYTE_1:
					pos++;
					m_flags &= ~FLAG_FHCRC;
					m_state++;
					break;
				case IN_DONE:
					return TRUE;
				}
			}
			if ((m_state > IN_HEADER_OS) && (m_flags == 0))
			{
				return TRUE;
			}
			return FALSE;
		}
		BOOL TinyGZip::GZipCompress(CHAR* bits, LONG size, TinyBufferArray<CHAR>& bufferIO)
		{
			bufferIO.Clear();
			BYTE buffer[GZIP_ZLIB_CHUNK];
			z_stream zs;
			ZeroMemory(&zs, sizeof(zs));
			zs.zalloc = Z_NULL;
			zs.zfree = Z_NULL;
			zs.opaque = Z_NULL;
			if (deflateInit2(&zs, Z_DEFAULT_COMPRESSION, Z_DEFLATED, GZIP_ZLIB_WINDOWSIZE | GZIP_ENCODING, 8, Z_DEFAULT_STRATEGY) != Z_OK)
				return FALSE;
			zs.next_in = (BYTE*)bits;
			zs.avail_in = size;
			do
			{
				zs.avail_out = GZIP_ZLIB_CHUNK;
				zs.next_out = buffer;
				if (deflate(&zs, Z_FINISH) == Z_STREAM_ERROR)
				{
					return false;
				}
				bufferIO.Add((CHAR*)buffer, (GZIP_ZLIB_CHUNK - zs.avail_out));
			} while (zs.avail_out == 0);
			if (inflateEnd(&zs) != Z_OK)
				return FALSE;
			return TRUE;
		}
		BOOL TinyGZip::GZipDecompress(CHAR* bits, LONG size, TinyBufferArray<CHAR>& bufferIO)
		{
			bufferIO.Clear();
			BYTE buffer[GZIP_ZLIB_CHUNK];
			z_stream zs;
			ZeroMemory(&zs, sizeof(zs));
			zs.zalloc = Z_NULL;
			zs.zfree = Z_NULL;
			zs.opaque = Z_NULL;
			zs.avail_in = 0;
			zs.next_in = Z_NULL;
			if (inflateInit2(&zs, 16 + MAX_WBITS) != Z_OK)
				return FALSE;
			zs.next_in = (BYTE*)buffer;
			zs.avail_in = static_cast<INT>(size);
			do
			{
				zs.avail_out = GZIP_ZLIB_CHUNK;
				zs.next_out = buffer;
				INT iRes = inflate(&zs, Z_NO_FLUSH);
				switch (iRes)
				{
				case Z_NEED_DICT:
				case Z_DATA_ERROR:
				case Z_MEM_ERROR:
					inflateEnd(&zs);
					return FALSE;
				}
				bufferIO.Add((CHAR*)buffer, (GZIP_ZLIB_CHUNK - zs.avail_out));
			} while (zs.avail_out == 0);
			if (inflateEnd(&zs) != Z_OK)
				return FALSE;
			return TRUE;
		}
	}
}