#include "stdafx.h"
#include "MPEG123Decode.h"

namespace Decode
{
	MPEG123Decode::MPEG123Decode()
		:m_handle(NULL)
	{
		mpg123_init();
	}


	MPEG123Decode::~MPEG123Decode()
	{
		mpg123_exit();
	}
	BOOL MPEG123Decode::Open(LPCSTR pzFile)
	{
		INT iRes = 0;
		m_handle = mpg123_new(NULL, &iRes);
		if (!m_handle)
			return FALSE;
		iRes = mpg123_param(m_handle, MPG123_VERBOSE, 2, 0);
		if (iRes == MPG123_ERR)
			goto _ERROR;
		iRes = mpg123_open_feed(m_handle);
		if (iRes == MPG123_ERR)
			goto _ERROR;

	_ERROR:
		mpg123_close(m_handle);
		mpg123_delete(m_handle);
		m_handle = NULL;
		return FALSE;
	}
	BOOL MPEG123Decode::Decode(BYTE* data, LONG size)
	{
		if (!m_handle)
			return FALSE;
		mpg123_decode(m_handle, data, size, );
		return TRUE;
	}
	BOOL MPEG123Decode::Close()
	{
		if (m_handle != NULL)
		{
			mpg123_close(m_handle);
			mpg123_delete(m_handle);
			m_handle = NULL;
		}
		return TRUE;
	}
}
