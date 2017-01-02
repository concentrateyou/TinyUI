#include "../stdafx.h"
#include "../Common/TinyString.h"
#include "TinyVisualCommon.h"
#include "TinyVisualManage.h"
#include "TinyVisualDocument.h"
#include "TinyVisualWindow.h"
#include "TinyVisualLabel.h"
#include "TinyVisualButton.h"
#include "TinyVisualCaption.h"
#include "TinyVisualHLayout.h"
#include "TinyVisualButton.h"
#include "TinyVisualScrollBar.h"
#include "TinyVisualComboBox.h"
#include "TinyVisualRichText.h"

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
			return m_doc.LoadFile(pzFile);
		}
		BOOL TinyVisualBuilder::BuildDocument(TinyVisualDocument* document)
		{
			TiXmlElement *pXML = m_doc.RootElement();
			TinyVisual* spvis = document->GetParent(NULL);
			if (spvis == NULL)
			{
				if (pXML && !strcasecmp(pXML->Value(), TinyVisualTag::WINDOW.STR()))
				{
					TinyMap<TinyString, TinyString> map;
					GetAttributeMap(pXML, map);
					TinyPoint pos = GetPosition(map.GetValue(TinyVisualProperty::POSITION));
					TinySize size = GetSize(map.GetValue(TinyVisualProperty::SIZE));
					spvis = document->Create<TinyVisualWindow>(0, 0, size.cx, size.cy, NULL);
					document->m_spvisWindow = spvis;
					BuildProperty(map, spvis);
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
					TinyMap<TinyString, TinyString> map;
					GetAttributeMap(static_cast<const TiXmlElement*>(pXMLChildNode), map);
					TinyPoint pos = GetPosition(map.GetValue(TinyVisualProperty::POSITION));
					TinySize size = GetSize(map.GetValue(TinyVisualProperty::SIZE));
					if (!strcasecmp(pXMLChildNode->Value(), TinyVisualTag::SYSCAPTION.STR()))
					{
						spvis = document->Create<TinyVisualCaption>(pos.x, pos.y, size.cx, size.cy, spvisParent);
					}
					if (!strcasecmp(pXMLChildNode->Value(), TinyVisualTag::BUTTON.STR()))
					{
						spvis = document->Create<TinyVisualButton>(pos.x, pos.y, size.cx, size.cy, spvisParent);
					}
					BuildProperty(map, spvis);
					if (spvis->IsLayout())
					{
						CreateInstace(pXMLChildNode, spvis, document);
					}
				}
			}
		}
		void TinyVisualBuilder::BuildProperty(TinyMap<TinyString, TinyString> &map, TinyVisual* spvis)
		{
			if (map.Contain(TinyVisualProperty::MAXSIZE))
			{
				TinySize maxsize = GetSize(map.GetValue(TinyVisualProperty::MAXSIZE));
				spvis->SetMaximumSize(maxsize);
			}
			if (map.Contain(TinyVisualProperty::MINSIZE))
			{
				TinySize minsize = GetSize(map.GetValue(TinyVisualProperty::MINSIZE));
				spvis->SetMinimumSize(minsize);
			}
			if (map.Contain(TinyVisualProperty::NAME))
			{
				TinyString* ps = map.GetValue(TinyVisualProperty::NAME);
				spvis->SetName(ps->STR());
			}
			if (map.Contain(TinyVisualProperty::TOOLTIP))
			{
				TinyString* ps = map.GetValue(TinyVisualProperty::TOOLTIP);
				spvis->SetToolTip(ps->STR());
			}
			if (map.Contain(TinyString(TinyVisualProperty::TEXT)))
			{
				TinyString* ps = map.GetValue(TinyVisualProperty::TEXT);
				spvis->SetText(ps->STR());
			}
			if (map.Contain(TinyString(TinyVisualProperty::ENABLE)))
			{
				TinyString* ps = map.GetValue(TinyVisualProperty::ENABLE);
				spvis->SetEnable(GetBool(ps));
			}
			if (map.Contain(TinyString(TinyVisualProperty::VISIBLE)))
			{
				TinyString* ps = map.GetValue(TinyVisualProperty::VISIBLE);
				spvis->SetVisible(GetBool(ps));
			}
			if (map.Contain(TinyString(TinyVisualProperty::IMAGENORMAL)))
			{
				TinyString* ps = map.GetValue(TinyVisualProperty::IMAGENORMAL);
				spvis->SetStyleImage(NORMAL, ps->STR());
			}
			if (map.Contain(TinyString(TinyVisualProperty::IMAGEHIGHLIGHT)))
			{
				TinyString* ps = map.GetValue(TinyVisualProperty::IMAGEHIGHLIGHT);
				spvis->SetStyleImage(HIGHLIGHT, ps->STR());
			}
			if (map.Contain(TinyString(TinyVisualProperty::IMAGEDOWN)))
			{
				TinyString* ps = map.GetValue(TinyVisualProperty::IMAGEDOWN);
				spvis->SetStyleImage(DOWN, ps->STR());
			}
			//if (map.Contain(TinyString(TinyVisualProperty::HORIZONTALALIGNMENT)))
			//{
			//	TinyString* ps = map.GetValue(TinyVisualProperty::HORIZONTALALIGNMENT);
			//	HorizontalAlignment ha = HORIZONTAL_LEFT;
			//	if (*ps == TinyVisualConst::LEFT)
			//		ha = HORIZONTAL_LEFT;
			//	if (*ps == TinyVisualConst::RIGHT)
			//		ha = HORIZONTAL_RIGHT;
			//	if (*ps == TinyVisualConst::CENTER)
			//		ha = HORIZONTAL_CENTER;
			//	if (*ps == TinyVisualConst::STRETCH)
			//		ha = HORIZONTAL_STRETCH;
			//	if (spvis->RetrieveTag() == TinyVisualTag::VERTICALLAYOUT)
			//	{
			//		TinyVisualVLayout* vis = static_cast<TinyVisualVLayout*>(spvis);
			//		vis->SetHorizontalAlignment(ha);
			//	}
			//	if (spvis->RetrieveTag() == TinyVisualTag::HORIZONTALLAYOUT)
			//	{
			//		TinyVisualHLayout* vis = static_cast<TinyVisualHLayout*>(spvis);
			//		vis->SetHorizontalAlignment(ha);
			//	}
			//}
			//if (map.Contain(TinyString(TinyVisualProperty::VERTICALALIGNMENT)))
			//{
			//	TinyString* ps = map.GetValue(TinyVisualProperty::VERTICALALIGNMENT);
			//	VerticalAlignment va = VERTICAL_TOP;
			//	if (*ps == TinyVisualConst::TOP)
			//		va = VERTICAL_TOP;
			//	if (*ps == TinyVisualConst::BOTTOM)
			//		va = VERTICAL_BOTTOM;
			//	if (*ps == TinyVisualConst::CENTER)
			//		va = VERTICAL_CENTER;
			//	if (*ps == TinyVisualConst::STRETCH)
			//		va = VERTICAL_STRETCH;
			//	if (spvis->RetrieveTag() == TinyVisualTag::VERTICALLAYOUT)
			//	{
			//		TinyVisualVLayout* vis = static_cast<TinyVisualVLayout*>(spvis);
			//		vis->SetVerticalAlignment(va);
			//	}
			//	if (spvis->RetrieveTag() == TinyVisualTag::HORIZONTALLAYOUT)
			//	{
			//		TinyVisualHLayout* vis = static_cast<TinyVisualHLayout*>(spvis);
			//		vis->SetVerticalAlignment(va);
			//	}
			//}
			spvis->m_map = std::move(map);
		}
		BOOL TinyVisualBuilder::GetAttributeMap(const TiXmlElement* pXMLNode, TinyMap<TinyString, TinyString>& map)
		{
			if (!pXMLNode)
				return FALSE;
			const TiXmlAttribute* pFA = pXMLNode->FirstAttribute();
			const TiXmlAttribute* pLA = pXMLNode->LastAttribute();
			while (pFA != pLA)
			{
				map.Add(pFA->Name(), pFA->Value());
				pFA = pFA->Next();
			}
			map.Add(pLA->Name(), pLA->Value());
			return TRUE;
		}
		TinySize TinyVisualBuilder::GetSize(const TinyString* ps)
		{
			if (!ps)
				return TinySize();
			TinyArray<TinyString> sps;
			ps->Split(',', sps);
			if (sps.GetSize() == 2)
			{
				return (TinySize(atoi(sps[0].STR()), atoi(sps[1].STR())));
			}
			return TinySize();
		}
		TinyPoint TinyVisualBuilder::GetPosition(const TinyString* ps)
		{
			if (!ps)
				return TinyPoint();
			TinyArray<TinyString> sps;
			ps->Split(',', sps);
			if (sps.GetSize() == 2)
			{
				return (TinyPoint(atoi(sps[0].STR()), atoi(sps[1].STR())));
			}
			return TinyPoint();
		}
		BOOL TinyVisualBuilder::GetBool(const TinyString* str)
		{
			return str->Compare("true") == 0;
		}
	};
}
