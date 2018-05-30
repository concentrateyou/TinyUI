#include "../stdafx.h"
#include "TinyURL.h"

namespace TinyFramework
{
	namespace Network
	{

		Component MakeRange(INT begin, INT end)
		{
			return Component(begin, end - begin);
		}
		BOOL IsURLSlash(CHAR s)
		{
			return s == '/' || s == '\\';
		}
		BOOL IsAuthorityTerminator(CHAR s)
		{
			return IsURLSlash(s) || s == '?' || s == '#';
		}
		INT FindNextAuthorityTerminator(const CHAR* spec, INT offset, INT size)
		{
			for (INT i = offset; i < size; i++)
			{
				if (IsAuthorityTerminator(spec[i]))
					return i;
			}
			return size;
		}
		INT CountConsecutiveSlashes(const CHAR *str, INT offset, INT size)
		{
			INT count = 0;
			while (offset + count < size && IsURLSlash(str[offset + count]))
				++count;
			return count;
		}
		void TrimURL(const CHAR* pzURL, INT* begin, INT* size)
		{
			while (*begin < *size && pzURL[*begin] <= ' ')
				(*begin)++;
			while (*size > *begin && pzURL[*size - 1] <= ' ')
				(*size)--;
		}
		void ParseUser(const CHAR* pzURL, const Component& user, Component* username, Component* password)
		{
			INT offset = 0;
			while (offset < user.size && pzURL[user.begin + offset] != ':')
				offset++;
			if (offset < user.size)
			{
				*username = Component(user.begin, offset);
				*password = MakeRange(user.begin + offset + 1, user.begin + user.size);
			}
			else
			{
				*username = user;
				*password = Component();
			}
		}
		void ParseServer(const CHAR* pzURL, const Component& server, Component* pHOST, Component* pPORT)
		{
			if (server.size == 0)
			{
				pHOST->reset();
				pPORT->reset();
			}
			else
			{
				INT ipv6 = pzURL[server.begin] == '[' ? server.end() : -1;
				INT offset = -1;

				for (INT i = server.begin; i < server.end(); i++)
				{
					switch (pzURL[i])
					{
					case ']':
						ipv6 = i;
						break;
					case ':':
						offset = i;
						break;
					}
				}
				if (offset > ipv6)
				{
					*pHOST = MakeRange(server.begin, offset);
					if (pHOST->size == 0)
						pHOST->reset();
					*pPORT = MakeRange(offset + 1, server.end());
				}
				else
				{
					*pHOST = server;
					pPORT->reset();
				}
			}
		}
		BOOL ParseAuthority(const CHAR* pzURL, const Component& auth, Component* pUSERNAME, Component* pPWD, Component* pHOST, Component* pPORT)
		{
			if (auth.size == 0)
			{
				pUSERNAME->reset();
				pPWD->reset();
				pHOST->reset();
				pPORT->reset();
				return FALSE;
			}

			INT offset = auth.begin + auth.size - 1;
			while (offset > auth.begin && pzURL[offset] != '@')
				offset--;
			if (pzURL[offset] == '@')
			{
				ParseUser(pzURL, Component(auth.begin, offset - auth.begin), pUSERNAME, pPWD);
				ParseServer(pzURL, MakeRange(offset + 1, auth.begin + auth.size), pHOST, pPORT);
			}
			else
			{
				pUSERNAME->reset();
				pPWD->reset();
				ParseServer(pzURL, auth, pHOST, pPORT);
			}
			return TRUE;
		}
		void ParsePath(const CHAR* pzURL, const Component& path, Component* pPATH, Component* pQUERY, Component* pREF)
		{
			if (path.size == -1)
			{
				pPATH->reset();
				pQUERY->reset();
				pREF->reset();
			}
			else
			{
				INT path_end = path.begin + path.size;
				INT query_separator = -1;
				INT ref_separator = -1;
				for (int i = path.begin; i < path_end; i++)
				{
					switch (pzURL[i])
					{
					case '?':
						if (ref_separator < 0 && query_separator < 0)
							query_separator = i;
						break;
					case '#':
						if (ref_separator < 0)
							ref_separator = i;
						break;
					}
				}
				INT file_end, query_end;
				if (ref_separator >= 0)
				{
					file_end = query_end = ref_separator;
					*pREF = MakeRange(ref_separator + 1, path_end);
				}
				else
				{
					file_end = query_end = path_end;
					pREF->reset();
				}

				if (query_separator >= 0)
				{
					file_end = query_separator;
					*pQUERY = MakeRange(query_separator + 1, query_end);
				}
				else
				{
					pQUERY->reset();
				}

				if (file_end != path.begin)
					*pPATH = MakeRange(path.begin, file_end);
				else
					pPATH->reset();
			}
		}
		BOOL ParseScheme(const CHAR* pzURL, INT size, Component* scheme)
		{
			INT begin = 0;
			while (begin < size && pzURL[begin] <= ' ')
				begin++;
			if (begin == size)
				return FALSE;
			for (INT i = begin; i < size; i++)
			{
				if (pzURL[i] == ':')
				{
					*scheme = MakeRange(begin, i);
					return TRUE;
				}
			}
			return FALSE;
		}
		//////////////////////////////////////////////////////////////////////////
		Component::Component()
			:begin(-1),
			size(-1)
		{

		}
		Component::Component(INT b, INT s)
			: begin(b), size(s)
		{

		}
		INT Component::end() const
		{
			return begin + size;
		}
		void Component::reset()
		{
			begin = 0;
			size = -1;
		}
		BOOL Component::empty() const
		{
			return size == -1;
		}
		BOOL Component::operator==(const Component& other) const
		{
			return begin == other.begin && size == other.size;
		}
		//////////////////////////////////////////////////////////////////////////
		TinyURL::TinyURL()
		{

		}
		string TinyURL::GetComponent(ComponentType type) const
		{
			switch (type)
			{
			case SCHEME:
				return m_szSCHEME;
			case HOST:
				return m_szHOST;
			case PASSWORD:
				return m_szPASSWORD;
			case PATH:
				return m_szPATH;
			case PORT:
				return m_szPORT;
			case QUERY:
				return m_szQUERY;
			case REF:
				return m_szREF;
			case USERNAME:
				return m_szUSERNAME;
			case FULLPATH:
				return m_szFULLPATH;
			}
			return string();
		}
		BOOL TinyURL::ParseURL(const CHAR* pzURL, INT size)
		{
			INT begin = 0;
			TrimURL(pzURL, &begin, &size);
			Component scheme;
			INT offset = 0;
			if (ParseScheme(pzURL, size, &scheme))
			{
				offset = scheme.end() + 1;
			}
			else
			{
				scheme.reset();
				offset = begin;
			}
			INT slashes = CountConsecutiveSlashes(pzURL, offset, size);
			INT auth = FindNextAuthorityTerminator(pzURL, offset + slashes, size);
			Component authority = Component(offset + slashes, auth - (offset + slashes));
			Component username;
			Component password;
			Component host;
			Component port;
			if (ParseAuthority(pzURL, authority, &username, &password, &host, &port))
			{
				Component path;
				Component query;
				Component ref;
				authority = auth == size ? Component() : Component(auth, size - auth);		
				ParsePath(pzURL, authority, &path, &query, &ref);
				string val = pzURL;
				m_szFULLPATH = val.substr(auth);
				if (!scheme.empty())
				{
					m_szSCHEME = val.substr(scheme.begin, scheme.size);
				}
				if (!username.empty())
				{
					m_szUSERNAME = val.substr(username.begin, username.size);
				}
				if (!password.empty())
				{
					m_szPASSWORD = val.substr(password.begin, password.size);
				}
				if (!host.empty())
				{
					m_szHOST = val.substr(host.begin, host.size);
				}
				if (!port.empty())
				{
					m_szPORT = val.substr(port.begin, port.size);
				}
				if (!path.empty())
				{
					m_szPATH = val.substr(path.begin, path.size);
				}
				if (!query.empty())
				{
					m_szQUERY = val.substr(query.begin, query.size);
				}
				if (!ref.empty())
				{
					m_szREF = val.substr(ref.begin, ref.size);
				}
				return TRUE;
			}
			return FALSE;
		}
	}
}