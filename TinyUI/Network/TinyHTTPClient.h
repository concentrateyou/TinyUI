#pragma once
#include "TinyNetwork.h"
#include "TinyHTTPCommon.h"
#include "TinyURL.h"
#include "TinyDNS.h"
#include <algorithm>
#include <string>
#include <map>

namespace TinyUI
{
	namespace Network
	{
#define DEFALUT_HTTP_HEADER_INITIAL_SIZE	4096
#define DEFALUT_HTTP_HEADER_MAX_SIZE		(4096 * 64)

		struct FreeDeleter
		{
			inline void operator()(void* ptr) const
			{
				free(ptr);
			}
		};
		class TinyHTTPClient;
		class IOBuffer : public TinyReference<IOBuffer>
		{
		public:
			IOBuffer();
			explicit IOBuffer(INT size);
			CHAR* data() const;
		protected:
			friend class TinyReference<IOBuffer>;
			explicit IOBuffer(CHAR* data);
			virtual ~IOBuffer();
			CHAR*	m_data;
		};
		class DrainableIOBuffer : public IOBuffer
		{
		public:
			DrainableIOBuffer(IOBuffer* base, INT size);
			virtual ~DrainableIOBuffer();
			void	SetOffset(INT offset);
			INT		GetOffset() const;
			INT		GetRemaining() const;
			INT		size() const;
		private:
			INT									m_offset;
			INT									m_size;
			TinyScopedReferencePtr<IOBuffer>	m_base;
		};
		/// <summary>
		/// 网络缓冲
		/// </summary>
		class NetworkIOBuffer : public IOBuffer
		{
		public:
			NetworkIOBuffer();
			void SetCapacity(INT capacity);
			INT capacity() const;
			void SetOffset(INT offset);
			INT offset() const;
			void SetReceive(INT receive);
			INT  receive() const;
			INT RemainingCapacity();
			CHAR* StartOfBuffer();
		protected:
			virtual ~NetworkIOBuffer();
			INT									m_receive;
			INT									m_capacity;
			INT									m_offset;
			std::unique_ptr<CHAR, FreeDeleter>	m_readIO;
		};
		/// <summary>
		/// 请求
		/// </summary>
		class HTTPRequest : public TinyHTTPAttribute
		{
			friend class TinyHTTPClient;
		protected:
			HTTPRequest();
			~HTTPRequest();
		public:
			void	SetBody(const string& body);
			void	SetBody(const CHAR* bits, LONG size);
			CHAR*	GetBody() const;
			LONG	GetBodySize() const;
			void	SetVerbs(const string& verb);
			string	GetVerbs() const;
		private:
			string				m_verbs;
			LONG				m_bodySize;
			TinyScopedPtr<CHAR>	m_body;
		};
		/// <summary>
		/// 应答
		/// </summary>
		class HTTPResponse : public TinyHTTPAttribute
		{
			friend class TinyHTTPClient;
		protected:
			HTTPResponse(TinyHTTPClient& client);
			~HTTPResponse();
		public:
			INT		GetStatusCode() const;
			string	GetVersion() const;
			string	GetGetStatusMsg() const;
			BOOL	ReadAsString(string& val);
			INT		ReadAsBinary(CHAR*& ps);
		private:
			BOOL	ReadLine(CHAR*& line1, CHAR*& line2);
			BOOL	FindLine(INT& size);
			BOOL	ParseTransferEncoding();
			BOOL	ParseAttribute(CHAR* ps1, CHAR* ps2);
			BOOL	ParseResponse();
			BOOL	ParseStatusLine(CHAR* ps1, CHAR* ps2);
		private:
			INT						m_statusCode;
			string					m_version;//版本
			string					m_statusMsg;//状态信息
			TinyHTTPClient&			m_client;
			TinyBufferArray<CHAR>	m_context;
			TinyScopedReferencePtr<NetworkIOBuffer> m_networkIO;
		};
		/// <summary>
		/// HTTP客户端
		/// </summary>
		class TinyHTTPClient
		{
		public:
			static const CHAR GET[];
			static const CHAR POST[];
			static const CHAR HTTP[];
			static const CHAR HTTP10[];
			static const CHAR HTTP11[];
			static const CHAR HTTP20[];
			static const CHAR HTTPS[];
			static const CHAR AcceptCharset[];
			static const CHAR AcceptEncoding[];
			static const CHAR AcceptLanguage[];
			static const CHAR Authorization[];
			static const CHAR CacheControl[];
			static const CHAR Connection[];
			static const CHAR ContentType[];
			static const CHAR Cookie[];
			static const CHAR ContentLength[];
			static const CHAR Host[];
			static const CHAR IfModifiedSince[];
			static const CHAR IfNoneMatch[];
			static const CHAR IfRange[];
			static const CHAR Origin[];
			static const CHAR Pragma[];
			static const CHAR ProxyAuthorization[];
			static const CHAR ProxyConnection[];
			static const CHAR Range[];
			static const CHAR Referer[];
			static const CHAR TransferEncoding[];
			static const CHAR TokenBinding[];
			static const CHAR UserAgent[];
			static const CHAR CRLF[];
			DISALLOW_COPY_AND_ASSIGN(TinyHTTPClient)
		public:
			TinyHTTPClient();
			~TinyHTTPClient();
			void			SetTimeout(INT timeout);
			void			SetErrorCallback(ErrorCallback&& callback);
			BOOL			IsEmpty() const;
			BOOL			Open(const string& szURL);
			void			Close();
			INT				Write(CHAR* bits, INT size);
			INT				Read(CHAR* bits, INT size);
			INT				ReadSome(CHAR* bits, INT size);
			HTTPRequest&	GetRequest();
			HTTPResponse&	GetResponse();
		private:
			void			BuildRequest();
		private:
			INT						m_timeout;
			TinyURL					m_szURL;
			TinySocket				m_socket;
			IPEndPoint				m_endpoint;
			HTTPRequest				m_request;
			HTTPResponse			m_reponse;
			TinyBufferArray<CHAR>	m_requestIO;
		};
	}
}

