#pragma once
#include "TinySocket.h"

namespace TinyUI
{
	namespace Network
	{
		class Component
		{
		public:
			Component();
			Component(INT b, INT l);
			INT end() const;
			BOOL empty() const;
			void reset();
			BOOL operator==(const Component& other) const;
		public:
			INT begin;
			INT size;
		};
		/// <summary>
		/// URL½âÎö
		/// </summary>
		class TinyURL
		{
		public:
			enum ComponentType
			{
				SCHEME,
				USERNAME,
				PASSWORD,
				HOST,
				PORT,
				PATH,
				QUERY,
				REF,
				FULLPATH
			};
		public:
			TinyURL();
			BOOL ParseURL(const CHAR* val, INT size);
			string GetComponent(ComponentType type) const;
		private:
			string	m_szSCHEME;
			string	m_szUSERNAME;
			string	m_szPASSWORD;
			string	m_szHOST;
			string	m_szPORT;
			string	m_szPATH;
			string	m_szQUERY;
			string	m_szREF;
			string	m_szFULLPATH;
		};
	}
}

