#include "../stdafx.h"
#include "TinyInternet.h"
#include <minmax.h>

namespace TinyUI
{
	namespace Network
	{
		extern SELECTANY const TCHAR URLhttp[] = _T("http://");

		TinyPointerMap TinyInternet::m_sessionMap;

		TinyInternet::TinyInternet()
			:m_hNET(NULL)
		{

		}
		TinyInternet::~TinyInternet()
		{
			Close();
		}
		TinyInternet::operator HINTERNET() const
		{
			return this == NULL ? NULL : m_hNET;
		}
		HINTERNET TinyInternet::Handle() const
		{
			return this == NULL ? NULL : m_hNET;
		}
		BOOL TinyInternet::operator == (const TinyInternet& obj) const
		{
			return obj.m_hNET == m_hNET;
		}
		BOOL TinyInternet::operator != (const TinyInternet& obj) const
		{
			return obj.m_hNET != m_hNET;
		}
		BOOL TinyInternet::Attach(HINTERNET hNET)
		{
			if (hNET == NULL)
				return FALSE;
			m_hNET = hNET;
			TinyPointerMap& map = TinyInternet::m_sessionMap;
			map.Add((UINT_PTR)m_hNET, (UINT_PTR)this);
			return TRUE;
		}
		HINTERNET TinyInternet::Detach()
		{
			HINTERNET hNET = m_hNET;
			if (hNET != NULL)
			{
				TinyPointerMap& map = TinyInternet::m_sessionMap;
				map.Remove((UINT_PTR)hNET);
			}
			m_hNET = NULL;
			return hNET;
		}
		TinyInternet* TinyInternet::Lookup(HINTERNET hNET)
		{
			TinyPointerMap& map = TinyInternet::m_sessionMap;
			UINT_PTR val = 0;
			if (!map.Lookup((UINT_PTR)hNET, val))
				return NULL;
			return reinterpret_cast<TinyInternet*>(val);
		}
		void TinyInternet::Close()
		{
			if (m_hNET != NULL)
			{
				InternetCloseHandle(m_hNET);
				m_hNET = NULL;
			}
		}
		BOOL TinyInternet::QueryOption(DWORD dwOption, LPVOID lpBuffer, LPDWORD lpdwBufLen) const
		{
			ASSERT(m_hNET);
			return InternetQueryOption(m_hNET, dwOption, lpBuffer, lpdwBufLen);
		}
		BOOL TinyInternet::QueryOption(DWORD dwOption, DWORD& dwValue) const
		{
			ASSERT(m_hNET);
			DWORD dwLen = sizeof(DWORD);
			return InternetQueryOption(m_hNET, dwOption, &dwValue, &dwLen);
		}
		BOOL TinyInternet::SetOption(DWORD dwOption, LPVOID lpBuffer, DWORD dwBufferLength)
		{
			ASSERT(m_hNET);
			return InternetSetOption(m_hNET, dwOption, lpBuffer, dwBufferLength);
		}
		BOOL TinyInternet::SetOption(DWORD dwOption, DWORD dwValue)
		{
			ASSERT(m_hNET);
			return InternetSetOption(m_hNET, dwOption, &dwValue, sizeof(dwValue));
		}
		BOOL TinyInternet::ParseURL(LPCSTR lpszUrl, DWORD& dwServiceType, TinyString& strServer, TinyString& strObject, INTERNET_PORT& nPort, TinyString& strUsername, TinyString& strPassword)
		{
			URL_COMPONENTS urlComponents;
			memset(&urlComponents, 0, sizeof(URL_COMPONENTS));
			urlComponents.dwStructSize = sizeof(URL_COMPONENTS);
			strServer.Resize(INTERNET_MAX_HOST_NAME_LENGTH + 1);
			strObject.Resize(INTERNET_MAX_PATH_LENGTH + 1);
			strUsername.Resize(INTERNET_MAX_USER_NAME_LENGTH + 1);
			strPassword.Resize(INTERNET_MAX_PASSWORD_LENGTH + 1);
			urlComponents.dwHostNameLength = INTERNET_MAX_HOST_NAME_LENGTH;
			urlComponents.lpszHostName = strServer.STR();
			urlComponents.dwUrlPathLength = INTERNET_MAX_PATH_LENGTH;
			urlComponents.lpszUrlPath = strObject.STR();
			urlComponents.dwUserNameLength = INTERNET_MAX_USER_NAME_LENGTH;
			urlComponents.lpszUserName = strUsername.STR();
			urlComponents.dwPasswordLength = INTERNET_MAX_PASSWORD_LENGTH;
			urlComponents.lpszPassword = strPassword.STR();
			LPTSTR pstrCanonicalizedURL;
			TCHAR szCanonicalizedURL[INTERNET_MAX_URL_LENGTH];
			DWORD dwNeededLength = INTERNET_MAX_URL_LENGTH;
			BOOL bRetVal;
			BOOL bMustFree = FALSE;
			DWORD dwCanonicalizeFlags = ICU_NO_ENCODE | ICU_NO_META | ICU_ENCODE_SPACES_ONLY | ICU_BROWSER_MODE;
			DWORD dwCrackFlags = 0;
			BOOL bUnescape = FALSE;
			bRetVal = InternetCanonicalizeUrl(lpszUrl, szCanonicalizedURL, &dwNeededLength, dwCanonicalizeFlags);
			if (!bRetVal)
			{
				if (::GetLastError() != ERROR_INSUFFICIENT_BUFFER)
					return FALSE;
				pstrCanonicalizedURL = new TCHAR[dwNeededLength];
				if (pstrCanonicalizedURL == NULL)
					return FALSE;
				bMustFree = TRUE;
				bRetVal = InternetCanonicalizeUrl(lpszUrl, pstrCanonicalizedURL, &dwNeededLength, dwCanonicalizeFlags);
				if (!bRetVal)
				{
					delete[] pstrCanonicalizedURL;
					return FALSE;
				}
			}
			else
			{
				pstrCanonicalizedURL = szCanonicalizedURL;
			}
			bRetVal = InternetCrackUrl(pstrCanonicalizedURL, 0, dwCrackFlags, &urlComponents);

			if (bUnescape)
			{
				if (strlen(urlComponents.lpszUrlPath) >= INTERNET_MAX_URL_LENGTH || FAILED(UrlUnescape(urlComponents.lpszUrlPath, NULL, NULL, URL_UNESCAPE_INPLACE | URL_DONT_UNESCAPE_EXTRA_INFO)))
				{
					if (bMustFree)
						delete[] pstrCanonicalizedURL;
					return FALSE;
				}
				urlComponents.dwUrlPathLength = static_cast<DWORD>(strlen(urlComponents.lpszUrlPath));
			}
			if (bMustFree)
			{
				delete[] pstrCanonicalizedURL;
			}
			if (!bRetVal)
			{
				dwServiceType = -1;
			}
			else
			{
				nPort = urlComponents.nPort;
				dwServiceType = urlComponents.nScheme;
			}
			return bRetVal;
		}
		//////////////////////////////////////////////////////////////////////////
		TinyInternetSession::TinyInternetSession(LPCTSTR pstrAgent, DWORD_PTR dwContext, DWORD dwAccessType, LPCTSTR pstrProxyName, LPCTSTR pstrProxyBypass, DWORD dwFlags)
			:m_dwContext(0),
			m_pCallback(NULL),
			m_bEnable(FALSE)
		{
			m_dwContext = dwContext;
			Attach(InternetOpen(pstrAgent, dwAccessType, pstrProxyName, pstrProxyBypass, dwFlags));
		}
		TinyInternetSession::~TinyInternetSession()
		{

		}
		DWORD_PTR TinyInternetSession::GetContext() const
		{
			return m_dwContext;
		}
		TinyHTTPConnection* TinyInternetSession::GetHttpConnection(LPCTSTR pstrServer, INTERNET_PORT nPort, LPCTSTR pstrUserName, LPCTSTR pstrPassword)
		{
			TinyHTTPConnection* pResult = new TinyHTTPConnection(this, pstrServer, nPort, pstrUserName, pstrPassword, m_dwContext);
			return pResult;
		}
		BOOL TinyInternetSession::EnableStatusCallback(BOOL bEnable)
		{
			ASSERT(m_hNET);
			if (bEnable)
			{
				INTERNET_STATUS_CALLBACK pCallback = InternetSetStatusCallback(m_hNET, TinyInternetSession::InertnetStatusCallback);
				if (pCallback != INTERNET_INVALID_STATUS_CALLBACK)
				{
					m_pCallback = pCallback;
					m_bEnable = bEnable;
					return TRUE;
				}
				return FALSE;
			}
			else
			{
				InternetSetStatusCallback(m_hNET, NULL);
				m_bEnable = bEnable;
			}
			return TRUE;
		}
		void WINAPI TinyInternetSession::InertnetStatusCallback(HINTERNET hInternet, DWORD_PTR dwContext, DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength)
		{

		}
		void TinyInternetSession::OnStatusCallback(DWORD_PTR dwContext, DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength)
		{
			if (m_pCallback != NULL)
			{
				(*m_pCallback)(m_hNET, dwContext, dwInternetStatus, lpvStatusInformation, dwStatusInformationLength);
			}
		}
		BOOL TinyInternetSession::SetCookie(LPCTSTR pstrUrl, LPCTSTR pstrCookieName, LPCTSTR pstrCookieData)
		{
			return InternetSetCookie(pstrUrl, pstrCookieName, pstrCookieData);
		}
		BOOL TinyInternetSession::GetCookie(LPCTSTR pstrUrl, LPCTSTR pstrCookieName, LPTSTR pstrCookieData, DWORD dwBufLen)
		{
			return InternetGetCookie(pstrUrl, pstrCookieName, pstrCookieData, &dwBufLen);
		}
		DWORD TinyInternetSession::GetCookieLength(LPCTSTR pstrUrl, LPCTSTR pstrCookieName)
		{
			DWORD dwRet;
			if (!InternetGetCookie(pstrUrl, pstrCookieName, NULL, &dwRet))
				dwRet = 0;
			return dwRet;
		}
		BOOL TinyInternetSession::GetCookie(LPCTSTR pstrUrl, LPCTSTR pstrCookieName, TinyString& strCookieData)
		{
			DWORD dwLen = GetCookieLength(pstrUrl, pstrCookieName);
			TinyString str(dwLen + 1);
			BOOL bRetVal = InternetGetCookie(pstrUrl, pstrCookieName, str.STR(), &dwLen);
			if (!bRetVal)
				strCookieData = str;
			return bRetVal;
		}
		//////////////////////////////////////////////////////////////////////////
		TinyInternetConnection::TinyInternetConnection(TinyInternetSession* pSession, LPCTSTR pstrServer, INTERNET_PORT nPort, DWORD_PTR dwContext)
			:m_pSession(pSession),
			m_server(pstrServer),
			m_port(nPort),
			m_dwContext(dwContext)
		{

		}
		TinyInternetConnection::~TinyInternetConnection()
		{

		}
		DWORD_PTR TinyInternetConnection::GetContext() const
		{
			return m_dwContext;
		}
		TinyInternetSession* TinyInternetConnection::GetSession() const
		{
			return m_pSession;
		};
		TinyString TinyInternetConnection::GetServer() const
		{
			return m_server;
		}
		//////////////////////////////////////////////////////////////////////////
		TinyInternetStream::TinyInternetStream(HINTERNET hRequest, TinyInternetConnection* pConnection, LPCTSTR pstrObject, LPCTSTR pstrServer, LPCTSTR pstrVerb, DWORD_PTR dwContext)
			:m_strVerb(pstrVerb),
			m_strObject(pstrObject),
			m_strServer(pstrServer),
			m_pConnection(pConnection),
			m_dwContext(dwContext),
			m_nWriteBufferSize(0),
			m_nWriteBufferPos(0),
			m_nReadBufferSize(0),
			m_nReadBufferPos(0),
			m_nReadBufferBytes(0),
			m_pbReadBuffer(NULL),
			m_pbWriteBuffer(NULL)
		{
			Attach(hRequest);
		}
		BOOL TinyInternetStream::SetWriteBufferSize(UINT nWriteSize)
		{
			BOOL bRes = TRUE;
			if (nWriteSize != m_nWriteBufferSize)
			{
				if (m_nWriteBufferPos > nWriteSize)
				{
					Flush();
				}
				if (nWriteSize == 0)
				{
					delete[] m_pbWriteBuffer;
					m_pbWriteBuffer = NULL;
				}
				else if (m_pbWriteBuffer == NULL)
				{
					m_pbWriteBuffer = new BYTE[nWriteSize];
					m_nWriteBufferPos = 0;
				}
				else
				{
					LPBYTE pbTemp = m_pbWriteBuffer;
					m_pbWriteBuffer = new BYTE[nWriteSize];
					if (m_nWriteBufferPos <= nWriteSize)
					{
						memcpy_s(m_pbWriteBuffer, nWriteSize, pbTemp, m_nWriteBufferPos);
					}
					delete[] pbTemp;
				}
				m_nWriteBufferSize = nWriteSize;
			}
			return bRes;
		}
		BOOL TinyInternetStream::SetReadBufferSize(UINT nReadSize)
		{
			BOOL bRes = TRUE;
			if (nReadSize != -1 && nReadSize != m_nReadBufferSize)
			{
				if (m_nReadBufferPos > nReadSize)
					bRes = FALSE;
				else
				{
					if (nReadSize == 0)
					{
						delete[] m_pbReadBuffer;
						m_pbReadBuffer = NULL;
					}
					else if (m_pbReadBuffer == NULL)
					{
						m_pbReadBuffer = new BYTE[nReadSize];
						m_nReadBufferPos = nReadSize;
					}
					else
					{
						DWORD dwMoved = m_nReadBufferSize - m_nReadBufferPos;
						LPBYTE pbTemp = m_pbReadBuffer;
						m_pbReadBuffer = new BYTE[nReadSize];
						if (dwMoved > 0 && dwMoved <= nReadSize)
						{
							memcpy_s(m_pbReadBuffer, nReadSize, pbTemp + m_nReadBufferPos, dwMoved);
							m_nReadBufferPos = 0;
							m_nReadBufferBytes = dwMoved;
						}
						else
						{
							m_nReadBufferBytes = 0;
							m_nReadBufferPos = nReadSize;
						}
						delete[] pbTemp;
					}
					m_nReadBufferSize = nReadSize;
				}
			}
			return bRes;
		}
		LONGLONG TinyInternetStream::Seek(LONGLONG lOff, DWORD dwMoveMethod)
		{
			ASSERT(m_hNET);
			ASSERT((lOff >= LONG_MIN) || (lOff <= LONG_MAX));
			return InternetSetFilePointer(m_hNET, LONG(lOff), NULL, dwMoveMethod, m_dwContext);
		}

		DWORD TinyInternetStream::Read(void* pData, DWORD cbData)
		{
			ASSERT(m_hNET);
			DWORD dwBytes = -1;
			if (m_pbReadBuffer == NULL)
			{
				if (!InternetReadFile(m_hNET, (LPVOID)pData, cbData, &dwBytes))
					throw exception("InternetReadFile Fail");
				return dwBytes;
			}
			LPBYTE lpbBuf = (LPBYTE)pData;
			if (cbData >= m_nReadBufferSize)
			{
				DWORD dwMoved = max(0, (long)m_nReadBufferBytes - (long)m_nReadBufferPos);
				if (dwMoved <= cbData)
				{
					memcpy_s(pData, cbData, m_pbReadBuffer + m_nReadBufferPos, dwMoved);
				}
				else
				{
					return 0;
				}
				m_nReadBufferPos = m_nReadBufferSize;
				if (!InternetReadFile(m_hNET, lpbBuf + dwMoved, cbData - dwMoved, &dwBytes))
					throw exception("InternetReadFile Fail");
				dwBytes += dwMoved;
			}
			else
			{
				if (m_nReadBufferPos + cbData >= m_nReadBufferBytes)
				{
					DWORD dwMoved = max(0, (long)m_nReadBufferBytes - (long)m_nReadBufferPos);
					if (dwMoved <= cbData)
					{
						memcpy_s(lpbBuf, cbData, m_pbReadBuffer + m_nReadBufferPos, dwMoved);
					}
					else
					{
						return 0;
					}
					DWORD dwRead;
					if (!InternetReadFile(m_hNET, m_pbReadBuffer, m_nReadBufferSize, &dwRead))
						throw exception("InternetReadFile Fail");
					m_nReadBufferBytes = dwRead;
					dwRead = min(cbData - dwMoved, m_nReadBufferBytes);
					memcpy_s(lpbBuf + dwMoved, cbData - dwMoved, m_pbReadBuffer, dwRead);
					m_nReadBufferPos = dwRead;
					dwBytes = dwMoved + dwRead;
				}
				else
				{
					memcpy_s(lpbBuf, cbData, m_pbReadBuffer + m_nReadBufferPos, cbData);
					m_nReadBufferPos += cbData;
					dwBytes = cbData;
				}
			}
			return dwBytes;
		}

		DWORD TinyInternetStream::Write(const void* pData, DWORD cbData)
		{
			ASSERT(m_hNET);
			DWORD dwBytes;
			if (m_pbWriteBuffer == NULL)
			{
				if (!InternetWriteFile(m_hNET, pData, cbData, &dwBytes))
					throw exception("InternetWriteFile Fail");
				if (dwBytes != cbData)
					throw exception("InternetWriteFile Fail");
			}
			else
			{
				if ((m_nWriteBufferPos + cbData) >= m_nWriteBufferSize)
				{
					if (!InternetWriteFile(m_hNET, m_pbWriteBuffer, m_nWriteBufferPos, &dwBytes))
						throw exception("InternetWriteFile Fail");
					m_nWriteBufferPos = 0;
				}
				if (cbData >= m_nWriteBufferSize)
				{
					if (!InternetWriteFile(m_hNET, (LPVOID)pData, cbData, &dwBytes))
						throw exception("InternetWriteFile Fail");
				}
				else
				{
					if (m_nWriteBufferPos + cbData <= m_nWriteBufferSize)
					{
						memcpy_s(m_pbWriteBuffer + m_nWriteBufferPos, m_nWriteBufferSize - m_nWriteBufferPos, pData, cbData);
						m_nWriteBufferPos += cbData;
					}
				}
			}
			return dwBytes;
		}

		BOOL TinyInternetStream::Flush()
		{
			ASSERT(m_hNET);
			if (m_pbWriteBuffer != NULL && m_nWriteBufferPos > 0)
			{
				DWORD dwBytes;
				if (!InternetWriteFile(m_hNET, m_pbWriteBuffer,
					m_nWriteBufferPos, &dwBytes))
				{
					return FALSE;
				}
				if (dwBytes != m_nWriteBufferPos)
					return FALSE;
				m_nWriteBufferPos = 0;
				return TRUE;
			}
			return FALSE;
		}

		LONGLONG TinyInternetStream::GetSize() const
		{
			DWORD dwSize = 0;
			if (m_hNET != NULL)
			{
				if (!InternetQueryDataAvailable(m_hNET, &dwSize, 0, 0))
				{
					dwSize = 0;
				}
			}
			return dwSize;
		}

		TinyInternetStream::~TinyInternetStream()
		{

		}
		TinyString	TinyInternetStream::GetObject() const
		{
			return m_strObject;
		}
		TinyString	TinyInternetStream::GetServer() const
		{
			return m_strServer;
		}
		TinyString	TinyInternetStream::GetVerb() const
		{
			return m_strVerb;
		}
		TinyString TinyInternetStream::GetFileURL() const
		{
			TinyString str(URLhttp);
			if (m_pConnection != NULL)
			{
				str += m_strServer;
				INT_PTR nLen = m_strObject.GetSize();
				if (nLen > 0)
				{
					if (m_strObject[0] != '/' && m_strObject[0] != '\\')
						str += '/';
					str += m_strObject;
				}
			}
			return str;
		}
		BOOL TinyInternetStream::ErrorDialog(HWND hWND, DWORD dwError, DWORD dwFlags, LPVOID* lppvData)
		{
			return InternetErrorDlg(hWND, m_hNET, dwError, dwFlags, lppvData);
		}
		//////////////////////////////////////////////////////////////////////////
		TinyHTTPStream::TinyHTTPStream(HINTERNET hRequest, TinyHTTPConnection* pConnection, LPCTSTR pstrObject, LPCTSTR pstrServer, LPCTSTR pstrVerb, DWORD_PTR dwContext)
			:TinyInternetStream(hRequest, pConnection, pstrObject, pstrServer, pstrVerb, dwContext)
		{

		}
		TinyHTTPStream::~TinyHTTPStream()
		{

		}
		BOOL TinyHTTPStream::AddRequestHeaders(LPCTSTR pstrHeaders, DWORD dwFlags, INT dwHeadersLen)
		{
			ASSERT(m_hNET);
			ASSERT(dwHeadersLen == 0 || pstrHeaders != NULL);
			if (dwHeadersLen == -1)
			{
				if (pstrHeaders == NULL)
				{
					dwHeadersLen = 0;
				}
				else
				{
					dwHeadersLen = static_cast<DWORD>(_tcslen(pstrHeaders));
				}
			}
			return HttpAddRequestHeaders(m_hNET, pstrHeaders, dwHeadersLen, dwFlags);
		}
		BOOL TinyHTTPStream::AddRequestHeaders(TinyString& str, DWORD dwFlags)
		{
			return AddRequestHeaders((LPCTSTR)str.STR(), dwFlags, (INT)str.GetSize());
		}
		BOOL TinyHTTPStream::SendRequest(LPCTSTR pstrHeaders, DWORD dwHeadersLen, LPVOID lpOptional, DWORD dwOptionalLen)
		{
			ASSERT(m_hNET);
			ASSERT(dwOptionalLen == 0 || lpOptional != NULL);
			ASSERT(dwHeadersLen == 0 || pstrHeaders != NULL);
			return HttpSendRequest(m_hNET, pstrHeaders, dwHeadersLen, lpOptional, dwOptionalLen);
		}
		BOOL TinyHTTPStream::SendRequest(TinyString& strHeaders, LPVOID lpOptional, DWORD dwOptionalLen)
		{
			return SendRequest((LPCTSTR)strHeaders.STR(), strHeaders.GetSize(), lpOptional, dwOptionalLen);
		}
		BOOL TinyHTTPStream::SendRequestEx(DWORD dwTotalLen, DWORD dwFlags, DWORD_PTR dwContext)
		{
			ASSERT(m_hNET);
			INTERNET_BUFFERS buffer;
			memset(&buffer, 0, sizeof(buffer));
			buffer.dwStructSize = sizeof(buffer);
			buffer.dwBufferTotal = dwTotalLen;
			return SendRequestEx(&buffer, NULL, dwFlags, dwContext);
		}
		BOOL TinyHTTPStream::SendRequestEx(LPINTERNET_BUFFERS lpBuffIn, LPINTERNET_BUFFERS lpBuffOut, DWORD dwFlags, DWORD_PTR dwContext)
		{
			ASSERT(m_hNET);
			return HttpSendRequestEx(m_hNET, lpBuffIn, lpBuffOut, dwFlags, dwContext);
		}
		BOOL TinyHTTPStream::EndRequest(DWORD dwFlags, LPINTERNET_BUFFERS lpBuffIn, DWORD_PTR dwContext)
		{
			ASSERT(m_hNET);
			return HttpEndRequest(m_hNET, lpBuffIn, dwFlags, dwContext);
		}
		BOOL TinyHTTPStream::QueryInfo(DWORD dwInfoLevel, LPVOID lpvBuffer, LPDWORD lpdwBufferLength, LPDWORD lpdwIndex) const
		{
			ASSERT(m_hNET);
			return HttpQueryInfo(m_hNET, dwInfoLevel, lpvBuffer, lpdwBufferLength, lpdwIndex);
		}
		BOOL TinyHTTPStream::QueryInfo(DWORD dwInfoLevel, TinyString& str, LPDWORD lpdwIndex) const
		{
			ASSERT(m_hNET);
			ASSERT(dwInfoLevel <= HTTP_QUERY_MAX);
			DWORD dwLen = 0;
			HttpQueryInfo(m_hNET, dwInfoLevel, NULL, &dwLen, 0);
			str = TinyString(dwLen);
			return HttpQueryInfo(m_hNET, dwInfoLevel, str.STR(), &dwLen, lpdwIndex);
		}
		BOOL TinyHTTPStream::QueryInfo(DWORD dwInfoLevel, SYSTEMTIME* pSysTime, LPDWORD lpdwIndex) const
		{
			dwInfoLevel |= HTTP_QUERY_FLAG_SYSTEMTIME;
			DWORD dwTimeSize = sizeof(SYSTEMTIME);
			return QueryInfo(dwInfoLevel, pSysTime, &dwTimeSize, lpdwIndex);
		}
		BOOL TinyHTTPStream::QueryInfo(DWORD dwInfoLevel, DWORD& dwResult, LPDWORD lpdwIndex) const
		{
			dwInfoLevel |= HTTP_QUERY_FLAG_NUMBER;
			DWORD dwDWSize = sizeof(DWORD);
			return QueryInfo(dwInfoLevel, &dwResult, &dwDWSize, lpdwIndex);
		}
		BOOL TinyHTTPStream::QueryInfoStatusCode(DWORD& dwStatusCode) const
		{
			ASSERT(m_hNET);
			TCHAR szBuffer[80];
			DWORD dwLen = _countof(szBuffer);
			BOOL bRes;
			bRes = HttpQueryInfo(m_hNET, HTTP_QUERY_STATUS_CODE, szBuffer, &dwLen, NULL);
			if (bRes)
				dwStatusCode = (DWORD)_ttol(szBuffer);
			return bRes;
		}
		//////////////////////////////////////////////////////////////////////////
		const LPCTSTR TinyHTTPConnection::szHtmlVerbs[] = {
			_T("POST"),
			_T("GET"),
			_T("HEAD"),
			_T("PUT"),
			_T("LINK"),
			_T("DELETE"),
			_T("UNLINK"),
		};
		TinyHTTPConnection::TinyHTTPConnection(TinyInternetSession* pSession, HINTERNET hConnected, LPCTSTR pstrServer, DWORD_PTR dwContext)
			:TinyInternetConnection(pSession, pstrServer, INTERNET_INVALID_PORT_NUMBER, dwContext)
		{
			m_hNET = hConnected;
		}
		TinyHTTPConnection::TinyHTTPConnection(TinyInternetSession* pSession, LPCTSTR pstrServer, INTERNET_PORT nPort, LPCTSTR pstrUserName, LPCTSTR pstrPassword, DWORD_PTR dwContext)
			: TinyInternetConnection(pSession, pstrServer, nPort, dwContext)
		{
			m_hNET = InternetConnect((HINTERNET)*pSession, pstrServer, nPort, pstrUserName, pstrPassword, INTERNET_SERVICE_HTTP, 0, dwContext);
		}
		TinyHTTPConnection::~TinyHTTPConnection()
		{

		}
		TinyHTTPStream* TinyHTTPConnection::OpenRequest(LPCTSTR pstrVerb,
			LPCTSTR pstrObjectName, LPCTSTR pstrReferer, DWORD_PTR dwContext,
			LPCTSTR* ppstrAcceptTypes, LPCTSTR pstrVersion, DWORD dwFlags)
		{
			ASSERT(m_hNET);
			ASSERT((dwFlags & INTERNET_FLAG_ASYNC) == 0);
			if (pstrVersion == NULL)
				pstrVersion = HTTP_VERSION;
			HINTERNET hRequest;
			hRequest = HttpOpenRequest(m_hNET, pstrVerb, pstrObjectName, pstrVersion, pstrReferer, ppstrAcceptTypes, dwFlags, dwContext);
			TinyHTTPStream* stream = new TinyHTTPStream(hRequest, this, pstrObjectName, GetServer().CSTR(), pstrVerb, dwContext);
			return stream;
		}

		TinyHTTPStream* TinyHTTPConnection::OpenRequest(INT nVerb,
			LPCTSTR pstrObjectName, LPCTSTR pstrReferer, DWORD_PTR dwContext,
			LPCTSTR* ppstrAcceptTypes,
			LPCTSTR pstrVersion, DWORD dwFlags)
		{
			ASSERT(m_hNET);
			ASSERT((dwFlags & INTERNET_FLAG_ASYNC) == 0);
			ASSERT(nVerb >= _HTTP_VERB_MIN && nVerb <= _HTTP_VERB_MAX);
			LPCTSTR pstrVerb;
			if (nVerb >= _HTTP_VERB_MIN && nVerb <= _HTTP_VERB_MAX)
				pstrVerb = szHtmlVerbs[nVerb];
			else
				pstrVerb = _T("");
			return OpenRequest(pstrVerb, pstrObjectName, pstrReferer, dwContext, ppstrAcceptTypes, pstrVersion, dwFlags);
		}

	}
}