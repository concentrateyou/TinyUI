#include "../stdafx.h"
#include "../Common/TinyString.h"
#include "../Render/TinyImage.h"
#include "TinyVisualCommon.h"
#include "TinyVisualManage.h"
#include "TinyVisualDocument.h"
#include "TinyVisualWindowless.h"
#include "TinyVisualWindow.h"

namespace TinyFramework
{
	namespace Windowless
	{
		TinyVisualResource::TinyVisualResource()
		{
			CHAR module[MAX_PATH];
			GetModuleFileName(NULL, module, MAX_PATH);
			m_szPath = module;
			string::size_type backslash = m_szPath.rfind('\\', m_szPath.size());
			if (backslash != string::npos)
			{
				m_szPath.erase(backslash + 1);
			}
			Register(TinyVisualTag::WINDOW, CLASS_NAME(TinyVisualWindow));
			Register(TinyVisualTag::BUTTON, CLASS_NAME(TinyVisualButton));
			Register(TinyVisualTag::LABEL, CLASS_NAME(TinyVisualLabel));
			Register(TinyVisualTag::HSCROLLBAR, CLASS_NAME(TinyVisualHScrollBar));
			Register(TinyVisualTag::VSCROLLBAR, CLASS_NAME(TinyVisualVScrollBar));
			Register(TinyVisualTag::PANEL, CLASS_NAME(TinyVisualPanel));
			Register(TinyVisualTag::MENUITEM, CLASS_NAME(TinyVisualMenuItem));
		}
		TinyVisualResource::~TinyVisualResource()
		{
			RemoveAll();
		}
		TinyVisualResource& TinyVisualResource::GetInstance()
		{
			static TinyVisualResource instance;
			return instance;
		}
		BOOL TinyVisualResource::Load(LPCSTR pzFile)
		{
			string szFile = StringPrintf("%s\%s", m_szPath.c_str(), pzFile);
			if (m_doc.LoadFile(szFile.c_str(), TIXML_ENCODING_UTF8))
			{
				TiXmlElement *pXMLNode = m_doc.RootElement();
				if (pXMLNode && !strcasecmp(pXMLNode->Value(), TinyVisualTag::CONTEXT.STR()))
				{
					BuildContext(pXMLNode);
				}
				return TRUE;
			}
			return FALSE;
		}
		string TinyVisualResource::GetDefaultPath()
		{
			return m_szPath;
		}
		BOOL TinyVisualResource::Add(TinyImage* image)
		{
			TinyAutoLock lock(m_lock);
			return m_images1.Add(image);
		}
		TinyImage* TinyVisualResource::Add(const TinyString& szName, const TinyString& szFile)
		{
			TinyAutoLock lock(m_lock);
			TinyImage** value = m_images2.GetValue(szName);
			if (value != NULL)
			{
				return *value;
			}
			else
			{
				string szNewFile = StringPrintf("%s\%s", m_szPath.c_str(), szFile.CSTR());
				if (!PathIsURL(szNewFile.c_str()))
				{
					ASSERT(PathFileExists(szNewFile.c_str()));
				}
				TinyImage* image = new TinyImage();
				if (image != NULL && image->Open(szNewFile.c_str()))
				{
					m_images2.Add(szName, image);
					return image;
				}
			}
			return NULL;
		}
		void TinyVisualResource::Remove(const TinyString& szName)
		{
			TinyAutoLock lock(m_lock);
			TinyImage** value = m_images2.GetValue(szName);
			if (value != NULL)
			{
				(*value)->Close();
			}
			SAFE_DELETE(*value);
			m_images2.Remove(szName);
		}
		void TinyVisualResource::Remove(TinyImage* image)
		{
			TinyAutoLock lock(m_lock);
			for (INT i = 0; i < m_images1.GetSize(); i++)
			{
				if (m_images1[i] == image)
				{
					m_images1.Remove(image);
					image->Close();
					SAFE_DELETE(image);
					break;
				}
			}
		}
		void TinyVisualResource::RemoveAll()
		{
			TinyAutoLock lock(m_lock);
			ITERATOR pos = m_images2.First();
			while (pos != NULL)
			{
				TinyImage** value = m_images2.GetValueAt(pos);
				if (value != NULL)
				{
					(*value)->Close();
				}
				SAFE_DELETE(*value);
				pos = m_images2.Next(pos);
			}
			m_images2.RemoveAll();

			for (INT i = 0; i < m_images1.GetSize(); i++)
			{
				m_images1[i]->Close();
				SAFE_DELETE(m_images1[i]);
			}
			m_images1.RemoveAll();
		}
		TinyImage* TinyVisualResource::operator[](const TinyString& szName)
		{
			TinyAutoLock lock(m_lock);
			TinyImage** value = m_images2.GetValue(szName);
			if (value != NULL)
				return *value;
			return NULL;
		}
		BOOL TinyVisualResource::Register(const TinyString& tag, const TinyString& value)
		{
			return m_types.Add(tag, value) != NULL;
		}
		void TinyVisualResource::Unregister(const TinyString& tag)
		{
			m_types.Remove(tag);
		}
		TinyString TinyVisualResource::GetClassName(const TinyString& tag)
		{
			TinyString* val = m_types.GetValue(tag);
			return val != NULL ? *val : TinyString();
		}
		void TinyVisualResource::BuildContext(const TiXmlNode* pXMLNode)
		{
			for (const TiXmlNode* pXMLChildNode = pXMLNode->FirstChild(); pXMLChildNode; pXMLChildNode = pXMLChildNode->NextSibling())
			{
				if (!strcasecmp(pXMLChildNode->Value(), TinyVisualTag::IMAGE.STR()))
				{
					BuildImage(pXMLChildNode);
				}
			}
		}
		void TinyVisualResource::BuildImage(const TiXmlNode* pXMLNode)
		{
			for (const TiXmlNode* pXMLChildNode = pXMLNode->FirstChild(); pXMLChildNode; pXMLChildNode = pXMLChildNode->NextSibling())
			{
				if (!strcasecmp(pXMLChildNode->Value(), TinyVisualTag::ADD.STR()))
				{
					const TiXmlElement* pXML = static_cast<const TiXmlElement*>(pXMLChildNode);
					const TiXmlAttribute* pFA = pXML->FirstAttribute();
					const TiXmlAttribute* pLA = pXML->LastAttribute();
					string szName;
					string szValue;
					while (pFA != pLA)
					{
						if (!strcasecmp(pFA->Name(), "name"))
						{
							szName = UTF8ToASCII(pFA->Value());
						}
						if (!strcasecmp(pFA->Name(), "value"))
						{
							szValue = UTF8ToASCII(pFA->Value());
						}
						pFA = pFA->Next();
					}
					if (!strcasecmp(pFA->Name(), "name"))
					{
						szName = UTF8ToASCII(pFA->Value());
					}
					if (!strcasecmp(pFA->Name(), "value"))
					{
						szValue = UTF8ToASCII(pFA->Value());
					}
					if (!szName.empty() && !szValue.empty())
					{
						TinyVisualResource::GetInstance().Add(szName.c_str(), szValue.c_str());
					}
				}
			}
		}
	};
}
