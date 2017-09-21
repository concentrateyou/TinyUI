#include "../stdafx.h"
#include "../Common/TinyString.h"
#include "../Render/TinyImage.h"
#include "TinyVisualCommon.h"
#include "TinyVisualManage.h"
#include "TinyVisualDocument.h"
#include "TinyVisualWindow.h"
#include "TinyVisualLabel.h"
#include "TinyVisualButton.h"
#include "TinyVisualCaption.h"
#include "TinyVisualButton.h"
#include "TinyVisualScrollBar.h"
#include "TinyVisualComboBox.h"
#include "TinyVisualTextBox.h"
#include "TinyVisualPanel.h"
#include "TinyVisualNative.h"

namespace TinyUI
{
	namespace Windowless
	{
		TinyVisualBuilder::TinyVisualBuilder()
		{

		}
		TinyVisualBuilder::~TinyVisualBuilder()
		{

		}
		BOOL TinyVisualBuilder::LoadFile(LPCSTR pzFile)
		{
			ASSERT(pzFile);
			return m_doc.LoadFile(pzFile, TIXML_ENCODING_UTF8);
		}
		BOOL TinyVisualBuilder::BuildDocument(TinyVisualDocument* document)
		{
			TiXmlElement *pXML = m_doc.RootElement();
			TinyVisual* spvis = document->GetParent(NULL);
			if (spvis == NULL)
			{
				if (pXML && !strcasecmp(pXML->Value(), TinyVisualTag::WINDOW.STR()))
				{
					spvis = document->Create(TinyVisualTag::WINDOW, NULL);
					BuildProperty(pXML, spvis);
					document->m_spvisWindow = spvis;
				}
				else
				{
					return FALSE;
				}
			}
			CreateInstace(pXML, spvis, document);
			spvis->OnInitialize();
			TinySize size = document->GetParent(NULL)->GetSize();
			RECT windowBounds;
			RECT centerBounds = { 0 };
			HMONITOR monitor = MonitorFromWindow(document->GetVisualHWND()->Handle(), MONITOR_DEFAULTTONEAREST);
			if (monitor != NULL)
			{
				MONITORINFO mi = { 0 };
				mi.cbSize = sizeof(mi);
				GetMonitorInfo(monitor, &mi);
				centerBounds = mi.rcWork;
			}
			windowBounds.left = centerBounds.left + (centerBounds.right - centerBounds.left - size.cx) / 2;
			windowBounds.right = windowBounds.left + size.cx;
			windowBounds.top = centerBounds.top + (centerBounds.bottom - centerBounds.top - size.cy) / 2;
			windowBounds.bottom = windowBounds.top + size.cy;
			return SetWindowPos(document->GetVisualHWND()->Handle(), 0, windowBounds.left, windowBounds.top,
				windowBounds.right - windowBounds.left,
				windowBounds.bottom - windowBounds.top,
				SWP_NOACTIVATE | SWP_NOZORDER);
		}
		void TinyVisualBuilder::CreateInstace(const TiXmlNode* pXMLNode, TinyVisual* spvisParent, TinyVisualDocument* document)
		{
			ASSERT(document && pXMLNode);
			TinyVisual* spvis = NULL;
			for (const TiXmlNode* pXMLChildNode = pXMLNode->FirstChild(); pXMLChildNode; pXMLChildNode = pXMLChildNode->NextSibling())
			{
				if (pXMLChildNode->Type() == TiXmlNode::TINYXML_ELEMENT)
				{
					spvis = document->Create(pXMLChildNode->Value(), spvisParent);
					if (spvis != NULL)
					{
						if (BuildProperty(static_cast<const TiXmlElement*>(pXMLChildNode), spvis))
						{
							CreateInstace(pXMLChildNode, spvis, document);
							spvis->OnInitialize();//初始化完成
						}
					}
				}
			}
		}

		BOOL TinyVisualBuilder::BuildProperty(const TiXmlElement* pXMLNode, TinyVisual* spvis)
		{
			if (!pXMLNode)
				return FALSE;

			const TiXmlAttribute* pFA = pXMLNode->FirstAttribute();
			const TiXmlAttribute* pLA = pXMLNode->LastAttribute();
			while (pFA != pLA)
			{
				string value = UTF8ToASCII(pFA->Value());
				if (spvis->SetProperty(pFA->Name(), value.c_str()))
				{
					spvis->m_propertys.Add(pFA->Name(), value.c_str());
				}
				pFA = pFA->Next();
			}
			string value = UTF8ToASCII(pFA->Value());
			if (spvis->SetProperty(pFA->Name(), UTF8ToASCII(pFA->Value()).c_str()))
			{
				spvis->m_propertys.Add(pFA->Name(), value.c_str());
			}
			return TRUE;
		}
		TinySize TinyVisualBuilder::GetSize(const TinyString& str)
		{
			TinyArray<TinyString> sps;
			str.Split(',', sps);
			if (sps.GetSize() == 2)
			{
				return (TinySize(atoi(sps[0].STR()), atoi(sps[1].STR())));
			}
			return TinySize();
		}
		TinyPoint TinyVisualBuilder::GetPosition(const TinyString& str)
		{
			TinyArray<TinyString> sps;
			str.Split(',', sps);
			if (sps.GetSize() == 2)
			{
				return (TinyPoint(atoi(sps[0].STR()), atoi(sps[1].STR())));
			}
			return TinyPoint();
		}
		TinyRectangle TinyVisualBuilder::GetRectangle(const TinyString& str)
		{
			TinyArray<TinyString> sps;
			str.Split(',', sps);
			if (sps.GetSize() == 4)
			{
				return (TinyRectangle(atoi(sps[0].STR()), atoi(sps[1].STR()), atoi(sps[2].STR()), atoi(sps[3].STR())));
			}
			return TinyRectangle();
		}
		BOOL TinyVisualBuilder::GetBool(const TinyString& str)
		{
			return str.Compare("true") == 0;
		}
		INT	TinyVisualBuilder::GetInt32(const TinyString& str)
		{
			return atoi(str.CSTR());
		}
		UINT TinyVisualBuilder::GetAlign(const TinyString& str)
		{
			UINT sFlag = DT_LEFT | DT_SINGLELINE;
			TinyArray<TinyString> sps;
			str.Split('|', sps);
			for (INT i = 0;i < sps.GetSize();i++)
			{
				if (strcasecmp(sps[i].STR(), "left") == 0)
					sFlag |= DT_LEFT;
				if (strcasecmp(sps[i].STR(), "top") == 0)
					sFlag |= DT_TOP;
				if (strcasecmp(sps[i].STR(), "right") == 0)
					sFlag |= DT_RIGHT;
				if (strcasecmp(sps[i].STR(), "bottom") == 0)
					sFlag |= DT_BOTTOM;
				if (strcasecmp(sps[i].STR(), "center") == 0)
					sFlag |= DT_CENTER;
				if (strcasecmp(sps[i].STR(), "vcenter") == 0)
					sFlag |= DT_VCENTER;
			}
			return sFlag;
		}
		Alignment TinyVisualBuilder::GetAlignment(const TinyString& str)
		{
			if (strcasecmp(str.STR(), "left") == 0)
				return Alignment::LEFT;
			if (strcasecmp(str.STR(), "top") == 0)
				return Alignment::TOP;
			if (strcasecmp(str.STR(), "right") == 0)
				return Alignment::RIGHT;
			if (strcasecmp(str.STR(), "bottom") == 0)
				return Alignment::BOTTOM;
			if (strcasecmp(str.STR(), "center") == 0)
				return Alignment::CENTER;
			return Alignment::LEFT;
		}
		COLORREF TinyVisualBuilder::GetColor(const TinyString& str)
		{
			TinyArray<TinyString> sps;
			str.Split(',', sps);
			if (sps.GetSize() == 3)
			{
				return RGB(atoi(sps[0].STR()), atoi(sps[1].STR()), atoi(sps[2].STR()));
			}
			return RGB(255, 255, 255);
		}
		//////////////////////////////////////////////////////////////////////////
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
			Register(TinyVisualTag::TEXTBOX, CLASS_NAME(TinyVisualTextBox));
			Register(TinyVisualTag::BUTTON, CLASS_NAME(TinyVisualButton));
			Register(TinyVisualTag::LABEL, CLASS_NAME(TinyVisualLabel));
			Register(TinyVisualTag::VSCROLLBAR, CLASS_NAME(TinyVisualVScrollBar));
			Register(TinyVisualTag::HSCROLLBAR, CLASS_NAME(TinyVisualHScrollBar));
			Register(TinyVisualTag::SYSCAPTION, CLASS_NAME(TinyVisualCaption));
			Register(TinyVisualTag::COMBOBOX, CLASS_NAME(TinyVisualComboBox));
			Register(TinyVisualTag::OPTION, CLASS_NAME(TinyVisualOption));
			Register(TinyVisualTag::PANEL, CLASS_NAME(TinyVisualPanel));
			Register(TinyVisualTag::TAB, CLASS_NAME(TinyVisualTag));
			Register(TinyVisualTag::NATIVEWINDOW, CLASS_NAME(TinyVisualNative));
			Register(TinyVisualTag::LIST, CLASS_NAME(TinyVisualList));
			Register(TinyVisualTag::LISTITEM, CLASS_NAME(TinyVisualListItem));
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
			return m_dynamicImages.Add(image);
		}
		TinyImage* TinyVisualResource::Add(const TinyString& szName, const TinyString& szFile)
		{
			TinyImage** value = m_staticImages.GetValue(szName);
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
					ASSERT(m_staticImages.Add(szName, image));
					return image;
				}
			}
			return NULL;
		}
		void TinyVisualResource::Remove(const TinyString& szName)
		{
			TinyImage** value = m_staticImages.GetValue(szName);
			if (value != NULL)
			{
				(*value)->Close();
			}
			SAFE_DELETE(*value);
			m_staticImages.Remove(szName);
		}
		void TinyVisualResource::RemoveAll()
		{
			ITERATOR pos = m_staticImages.First();
			while (pos != NULL)
			{
				TinyImage** value = m_staticImages.GetValueAt(pos);
				if (value != NULL)
				{
					(*value)->Close();
				}
				SAFE_DELETE(*value);
				pos = m_staticImages.Next(pos);
			}
			m_staticImages.RemoveAll();

			for (INT i = 0;i < m_dynamicImages.GetSize();i++)
			{
				m_dynamicImages[i]->Close();
				SAFE_DELETE(m_dynamicImages[i]);
			}
			m_dynamicImages.RemoveAll();
		}
		TinyImage* TinyVisualResource::operator[](const TinyString& szName)
		{
			TinyImage** value = m_staticImages.GetValue(szName);
			return *value;
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
