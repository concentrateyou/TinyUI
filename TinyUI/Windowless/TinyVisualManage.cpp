#include "../stdafx.h"
#include "../Common/TinyString.h"
#include "TinyVisualCommon.h"
#include "TinyVisualManage.h"
#include "TinyVisualDocument.h"
#include "TinyVisualWindow.h"
#include "TinyVisualLabel.h"
#include "TinyVisualButton.h"
#include "TinyVisualVLayout.h"
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
					::SetWindowPos(document->GetVisualHWND()->Handle(),
						NULL,
						pos.x,
						pos.y,
						size.cx,
						size.cy,
						SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
				}
				else
				{
					return FALSE;
				}
			}
			CreateInstace(pXML, spvis, document);
			return TRUE;
		}
		void TinyVisualBuilder::CreateInstace(const TiXmlNode* pXMLNode, TinyVisual* spvisParent, TinyVisualDocument* document)
		{
			TinyVisual* spvis = NULL;
			for (const TiXmlNode* pXMLChildNode = pXMLNode->FirstChild(); pXMLChildNode; pXMLChildNode = pXMLChildNode->NextSibling())
			{
				if (pXMLChildNode->Type() == TiXmlNode::TINYXML_ELEMENT &&
					!strcasecmp(pXMLChildNode->Value(), TinyVisualTag::HORIZONTALLAYOUT.STR()))
				{
					TinyMap<TinyString, TinyString> map;
					GetAttributeMap(static_cast<const TiXmlElement*>(pXMLChildNode), map);
					TinyPoint pos = GetPosition(map.GetValue(TinyVisualProperty::POSITION));
					TinySize size = GetSize(map.GetValue(TinyVisualProperty::SIZE));
					spvis = document->Create<TinyVisualHLayout>(pos.x, pos.y, size.cx, size.cy, spvisParent);
					BuildProperty(map, spvis);
					document->LinkVisual(spvis, PVISUAL_BOTTOM, &spvisParent->m_spvisChild);
					CreateInstace(pXMLChildNode, spvis, document);
					spvis->Resize();
				}
				else if (pXMLChildNode->Type() == TiXmlNode::TINYXML_ELEMENT &&
					!strcasecmp(pXMLChildNode->Value(), TinyVisualTag::VERTICALLAYOUT.STR()))
				{
					TinyMap<TinyString, TinyString> map;
					GetAttributeMap(static_cast<const TiXmlElement*>(pXMLChildNode), map);
					TinyPoint pos = GetPosition(map.GetValue(TinyVisualProperty::POSITION));
					TinySize size = GetSize(map.GetValue(TinyVisualProperty::SIZE));
					spvis = document->Create<TinyVisualVLayout>(pos.x, pos.y, size.cx, size.cy, spvisParent);
					BuildProperty(map, spvis);
					document->LinkVisual(spvis, PVISUAL_BOTTOM, &spvisParent->m_spvisChild);
					CreateInstace(pXMLChildNode, spvis, document);
					spvis->Resize();
				}
				else if (pXMLChildNode->Type() == TiXmlNode::TINYXML_ELEMENT &&
					!strcasecmp(pXMLChildNode->Value(), TinyVisualTag::BUTTON.STR()))
				{
					TinyMap<TinyString, TinyString> map;
					GetAttributeMap(static_cast<const TiXmlElement*>(pXMLChildNode), map);
					TinyPoint pos = GetPosition(map.GetValue(TinyVisualProperty::POSITION));
					TinySize size = GetSize(map.GetValue(TinyVisualProperty::SIZE));
					spvis = document->Create<TinyVisualButton>(pos.x, pos.y, size.cx, size.cy, spvisParent);
					BuildProperty(map, spvis);
					document->LinkVisual(spvis, PVISUAL_BOTTOM, &spvisParent->m_spvisChild);
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
		}
		BOOL TinyVisualBuilder::GetAttributeMap(const TiXmlElement* pXMLNode, TinyMap<TinyString, TinyString>& map)
		{
			if (!pXMLNode) return FALSE;
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
			if (!ps) return TinySize();
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
			if (!ps) return TinyPoint();
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
