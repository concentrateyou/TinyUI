#include "../stdafx.h"
#include "../Common/TinyString.h"
#include "TinyVisualManage.h"
#include "TinyVisualWindow.h"
#include "TinyVisualCaption.h"
#include "TinyVisualTree.h"

namespace TinyUI
{
	namespace Windowless
	{
		const CHAR* TinyVisualPoperty::NAME = TEXT("name");
		const CHAR* TinyVisualPoperty::TEXT = TEXT("text");
		const CHAR* TinyVisualPoperty::TOOLTIP = TEXT("tooltip");
		const CHAR* TinyVisualPoperty::MAXSIZE = TEXT("maxsize");
		const CHAR* TinyVisualPoperty::MINSIZE = TEXT("minsize");
		const CHAR* TinyVisualPoperty::SIZE = TEXT("size");
		const CHAR* TinyVisualPoperty::VISIBLE = TEXT("visible");
		const CHAR* TinyVisualPoperty::ENABLE = TEXT("enable");
		//////////////////////////////////////////////////////////////////////////
		const CHAR* TinyVisualTag::WINDOW = TEXT("window");
		const CHAR* TinyVisualTag::CAPTION = TEXT("caption");
		const CHAR* TinyVisualTag::SYSBUTTON = TEXT("sysbutton");
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
			if (pXML && !strcasecmp(pXML->Value(), TinyVisualTag::WINDOW))
			{
				TinyMap<TinyString, TinyString> map;
				GetAttributeMap(pXML, map);
				TinySize size = GetSize(map.GetValue("size"));
				spvis->SetSize(size);
				TinySize maxsize = GetSize(map.GetValue("maxsize"));
				spvis->SetMaximumSize(maxsize);
				TinySize minsize = GetSize(map.GetValue("minsize"));
				spvis->SetMinimumSize(minsize);
			}
			return CreateInstace(pXML, spvis, pvisualTree) != NULL;
		}
		TinyVisual* TinyVisualParse::CreateInstace(const TiXmlNode* pXMLNode, TinyVisual* spvisParent, TinyVisualTree* pvisualTree)
		{
			TinyVisual* spvis = NULL;
			for (const TiXmlNode* pXMLChildNode = pXMLNode->FirstChild(); pXMLChildNode; pXMLChildNode = pXMLChildNode->NextSibling())
			{
				if (pXMLChildNode->Type() == TiXmlNode::TINYXML_ELEMENT &&
					!strcasecmp(pXMLChildNode->Value(), TinyVisualTag::CAPTION))
				{
					TinyMap<TinyString, TinyString> map;
					GetAttributeMap(static_cast<const TiXmlElement*>(pXMLChildNode), map);
					spvis = new TinyVisualCaption(spvisParent);
					pvisualTree->LinkVisual(spvis, PVISUAL_BOTTOM, &spvisParent->m_spvisChild);
				}
				else if (pXMLChildNode->Type() == TiXmlNode::TINYXML_ELEMENT &&
					!strcasecmp(pXMLChildNode->Value(), TinyVisualTag::SYSBUTTON))
				{
					TinyMap<TinyString, TinyString> map;
					GetAttributeMap(static_cast<const TiXmlElement*>(pXMLChildNode), map);
					spvis = new TinySysButton(spvisParent);
					pvisualTree->LinkVisual(spvis, PVISUAL_BOTTOM, &spvisParent->m_spvisChild);
				}
				return CreateInstace(pXMLChildNode, spvis, pvisualTree);
			}
			return spvis;
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
	};
}
