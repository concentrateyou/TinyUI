#include "../stdafx.h"
#include "../Common/TinyString.h"
#include "TinyVisualCommon.h"
#include "TinyVisualManage.h"
#include "TinyVisualWindow.h"
#include "TinyVisualCaption.h"
#include "TinyVisualTree.h"

namespace TinyUI
{
	namespace Windowless
	{
		TinyVisualFactory::TinyVisualFactory(TinyVisualTree* vtree)
			:m_vtree(vtree)
		{

		}
		TinyVisual* TinyVisualFactory::Create(INT x, INT y, INT cx, INT cy, TinyVisual* spvisParent, const TinyString& tag)
		{
			ASSERT(m_vtree);
			TinyVisual* spvis = NULL;
			if (tag.Compare(TinyVisualTag::CAPTION) == 0)
			{
				spvis = new TinyVisualCaption(spvisParent, m_vtree);
			}
			else if (tag.Compare(TinyVisualTag::SYSBUTTON) == 0)
			{
				spvis = new TinyVisualSysButton(spvisParent, m_vtree);
			}
			spvis->SetPosition(TinyPoint(x, y));
			spvis->SetSize(TinySize(cx, cy));
			m_vtree->SetParent(spvis, spvisParent);
			return spvis;
		}
		BOOL TinyVisualFactory::Destory(TinyVisual* spvis)
		{
			ASSERT(m_vtree);
			if (!spvis) return FALSE;
			TinyVisual* spvisNext = NULL;
			TinyVisual* spvisChild = NULL;
			spvisChild = spvis->m_spvisChild;
			while (spvisChild != NULL)
			{
				spvisNext = spvisChild->m_spvisNext;
				Destory(spvisChild);
				spvisChild = spvisNext;
			}
			if (spvis->m_spvisParent)
			{
				m_vtree->UnlinkVisual(spvis, &(spvis->m_spvisParent->m_spvisChild));
			}
			SAFE_DELETE(spvis);
			return TRUE;
		}
		//////////////////////////////////////////////////////////////////////////
		TinyVisualParse::TinyVisualParse()
		{

		}
		TinyVisualParse::~TinyVisualParse()
		{

		}
		BOOL TinyVisualParse::LoadFile(LPCSTR pzFile)
		{
			ASSERT(pzFile);
			return m_doc.LoadFile(pzFile);
		}
		BOOL TinyVisualParse::BuildVisualTree(TinyVisualTree* pvisualTree)
		{
			TinyVisual* spvis = pvisualTree->GetParent(NULL);
			if (!spvis) return FALSE;
			TiXmlElement *pXML = m_doc.RootElement();
			if (pXML && !strcasecmp(pXML->Value(), TinyVisualTag::WINDOW.STR()))
			{
				TinyMap<TinyString, TinyString> map;
				GetAttributeMap(pXML, map);
				BuildProperty(map, spvis);
			}
			CreateInstace(pXML, spvis, pvisualTree);
			pvisualTree->Dump();
			return TRUE;
		}
		void TinyVisualParse::CreateInstace(const TiXmlNode* pXMLNode, TinyVisual* spvisParent, TinyVisualTree* pvisualTree)
		{
			/*TinyVisual* spvis = NULL;
			for (const TiXmlNode* pXMLChildNode = pXMLNode->FirstChild(); pXMLChildNode; pXMLChildNode = pXMLChildNode->NextSibling())
			{
			if (pXMLChildNode->Type() == TiXmlNode::TINYXML_ELEMENT &&
			!strcasecmp(pXMLChildNode->Value(), TinyVisualTag::CAPTION.STR()))
			{
			TinyMap<TinyString, TinyString> map;
			GetAttributeMap(static_cast<const TiXmlElement*>(pXMLChildNode), map);
			spvis = new TinyVisualCaption(spvisParent);
			BuildProperty(map, spvis);
			pvisualTree->LinkVisual(spvis, PVISUAL_BOTTOM, &spvisParent->m_spvisChild);
			CreateInstace(pXMLChildNode, spvis, pvisualTree);
			spvis->Resize();
			}
			else if (pXMLChildNode->Type() == TiXmlNode::TINYXML_ELEMENT &&
			!strcasecmp(pXMLChildNode->Value(), TinyVisualTag::SYSBUTTON.STR()))
			{
			TinyMap<TinyString, TinyString> map;
			GetAttributeMap(static_cast<const TiXmlElement*>(pXMLChildNode), map);
			spvis = new TinyVisualSysButton(spvisParent);
			BuildProperty(map, spvis);
			pvisualTree->LinkVisual(spvis, PVISUAL_BOTTOM, &spvisParent->m_spvisChild);
			}
			}*/
		}
		void TinyVisualParse::BuildProperty(TinyMap<TinyString, TinyString> &map, TinyVisual* spvis)
		{
			if (map.Contain(TinyVisualProperty::SIZE))
			{
				TinySize size = GetSize(map.GetValue(TinyVisualProperty::SIZE));
				spvis->SetSize(size);
			}
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
		BOOL TinyVisualParse::GetAttributeMap(const TiXmlElement* pXMLNode, TinyMap<TinyString, TinyString>& map)
		{
			if (!pXMLNode) return FALSE;
			const TiXmlAttribute* pFA = pXMLNode->FirstAttribute();
			const TiXmlAttribute* pLA = pXMLNode->LastAttribute();
			while (pFA != pLA)
			{
				map.Add(pFA->Name(), pFA->Value());
				pFA = pFA->Next();
			}
			return TRUE;
		}
		TinySize TinyVisualParse::GetSize(const TinyString* ps)
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
		BOOL TinyVisualParse::GetBool(const TinyString* str)
		{
			return str->Compare("true") == 0;
		}
	};
}
