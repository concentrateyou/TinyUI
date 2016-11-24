#include "../stdafx.h"
#include "TinyInternet.h"

namespace TinyUI
{
	namespace Network
	{
		extern SELECTANY const TCHAR URLhttp[] = _T("http://");

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
			TinyPtrMap& map = TinyInternet::m_sessionMap;
			map.Add((UINT_PTR)m_hNET, (UINT_PTR)this);
			return TRUE;
		}
		HINTERNET TinyInternet::Detach()
		{
			HINTERNET hNET = m_hNET;
			if (hNET != NULL)
			{
				TinyPtrMap& map = TinyInternet::m_sessionMap;
				map.Remove((UINT_PTR)hNET);
			}
			m_hNET = NULL;
			return hNET;
		}
		TinyInternet* TinyInternet::Lookup(HINTERNET hNET)
		{
			TinyPtrMap& map = TinyInternet::m_sessionMap;
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
		//////////////////////////////////////////////////////////////////////////
		TinyInternetStream::TinyInternetStream(HINTERNET hRequest, TinyInternetConnection* pConnection, LPCTSTR pstrObject,
			LPCTSTR pstrServer, LPCTSTR pstrVerb, DWORD_PTR dwContext)
			:m_strVerb(pstrVerb),
			m_strObject(pstrObject),
			m_strServer(pstrServer),
			m_pConnection(pConnection)
		{
			Attach(hRequest);
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
	}
}