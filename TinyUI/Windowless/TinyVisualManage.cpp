#include "../stdafx.h"
#include "../Common/TinyString.h"
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
#include "TinyVisualRichText.h"
#include "TinyVisualHBoxLayout.h"
#include "TinyVisualVBoxLayout.h"
#include "TinyVisualPanel.h"

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
					spvis = document->Create<TinyVisualWindow>(NULL);
					BuildProperty(pXML, spvis);
					document->m_spvisWindow = spvis;
				}
				else
				{
					return FALSE;
				}
			}
			CreateInstace(pXML, spvis, document);
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
					spvisParent->m_dwCount++;
					if (!strcasecmp(pXMLChildNode->Value(), TinyVisualTag::SYSCAPTION.STR()))
					{
						spvis = document->Create<TinyVisualCaption>(spvisParent);
					}
					if (!strcasecmp(pXMLChildNode->Value(), TinyVisualTag::HBOXLAYOUT.STR()))
					{
						spvis = document->Create<TinyVisualHBoxLayout>(spvisParent);
					}
					if (!strcasecmp(pXMLChildNode->Value(), TinyVisualTag::VBOXLAYOUT.STR()))
					{
						spvis = document->Create<TinyVisualVBoxLayout>(spvisParent);
					}
					if (!strcasecmp(pXMLChildNode->Value(), TinyVisualTag::LABEL.STR()))
					{
						spvis = document->Create<TinyVisualLabel>(spvisParent);
					}
					if (!strcasecmp(pXMLChildNode->Value(), TinyVisualTag::BUTTON.STR()))
					{
						spvis = document->Create<TinyVisualButton>(spvisParent);
					}
					if (!strcasecmp(pXMLChildNode->Value(), TinyVisualTag::COMBOBOX.STR()))
					{
						spvis = document->Create<TinyVisualComboBox>(spvisParent);
					}
					if (!strcasecmp(pXMLChildNode->Value(), TinyVisualTag::PANEL.STR()))
					{
						spvis = document->Create<TinyVisualPanel>(spvisParent);
					}
					if (spvis != NULL)
					{
						TinyMap<TinyString, TinyString> map;
						BuildProperty(static_cast<const TiXmlElement*>(pXMLChildNode), spvis);
						if (spvis->IsLayout())
						{
							CreateInstace(pXMLChildNode, spvis, document);
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
				spvis->SetProperty(pFA->Name(), UTF8ToASCII(pFA->Value()).c_str());
				pFA = pFA->Next();
			}
			spvis->SetProperty(pFA->Name(), UTF8ToASCII(pFA->Value()).c_str());
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
		BOOL TinyVisualBuilder::GetBool(const TinyString& str)
		{
			return str.Compare("true") == 0;
		}
		UINT TinyVisualBuilder::GetAlign(const TinyString& str)
		{
			UINT sFlag = DT_LEFT| DT_SINGLELINE;
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
	};
}
