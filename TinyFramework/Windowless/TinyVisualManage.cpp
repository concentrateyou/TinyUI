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
		//TinyVisualBuilder::TinyVisualBuilder()
		//{

		//}
		//TinyVisualBuilder::~TinyVisualBuilder()
		//{

		//}
		//BOOL TinyVisualBuilder::LoadFile(LPCSTR pzFile)
		//{
		//	ASSERT(pzFile);
		//	return m_doc.LoadFile(pzFile, TIXML_ENCODING_UTF8);
		//}
		//BOOL TinyVisualBuilder::BuildDocument(TinyVisualDocument* document)
		//{
		//	ASSERT(document);
		//	TiXmlElement *pXML = m_doc.RootElement();
		//	TinyVisual* spvis = document->GetParent(NULL);
		//	if (spvis == NULL)
		//	{
		//		if (pXML && !strcasecmp(pXML->Value(), TinyVisualTagConst::WINDOW.STR()))
		//		{
		//			spvis = document->Create(TinyVisualTagConst::WINDOW, NULL);//根节点元素
		//			BuildProperty(pXML, spvis);
		//			document->m_spvisWindow = spvis;
		//		}
		//		else
		//		{
		//			return FALSE;
		//		}
		//	}
		//	CreateInstace(pXML, spvis, document);
		//	CalculateLayout(spvis, document);
		//	spvis->OnInitialize();
		//	TinySize size = document->GetParent(NULL)->GetSize();
		//	RECT windowBounds;
		//	RECT centerBounds = { 0 };
		//	HMONITOR monitor = MonitorFromWindow(document->GetVisualHWND().Handle(), MONITOR_DEFAULTTONEAREST);
		//	if (monitor != NULL)
		//	{
		//		MONITORINFO mi = { 0 };
		//		mi.cbSize = sizeof(mi);
		//		GetMonitorInfo(monitor, &mi);
		//		centerBounds = mi.rcWork;
		//	}
		//	windowBounds.left = centerBounds.left + (centerBounds.right - centerBounds.left - size.cx) / 2;
		//	windowBounds.right = windowBounds.left + size.cx;
		//	windowBounds.top = centerBounds.top + (centerBounds.bottom - centerBounds.top - size.cy) / 2;
		//	windowBounds.bottom = windowBounds.top + size.cy;
		//	return SetWindowPos(document->GetVisualHWND().Handle(), 0, windowBounds.left, windowBounds.top,
		//		windowBounds.right - windowBounds.left,
		//		windowBounds.bottom - windowBounds.top,
		//		SWP_NOACTIVATE | SWP_NOZORDER);
		//}
		//void TinyVisualBuilder::CreateInstace(const TiXmlNode* pXMLNode, TinyVisual* spvisParent, TinyVisualDocument* document)
		//{
		//	ASSERT(document && pXMLNode);
		//	TinyVisual* spvis = NULL;
		//	for (const TiXmlNode* pXMLChildNode = pXMLNode->FirstChild(); pXMLChildNode; pXMLChildNode = pXMLChildNode->NextSibling())
		//	{
		//		if (pXMLChildNode->Type() == TiXmlNode::TINYXML_ELEMENT)
		//		{
		//			spvis = document->Create(pXMLChildNode->Value(), spvisParent);
		//			if (spvis != NULL)
		//			{
		//				if (BuildProperty(static_cast<const TiXmlElement*>(pXMLChildNode), spvis))
		//				{
		//					CreateInstace(pXMLChildNode, spvis, document);
		//				}
		//			}
		//		}
		//	}
		//}

		//BOOL TinyVisualBuilder::CalculateLayout(TinyVisual* spvisParent, TinyVisualDocument* document)
		//{
		//	if (!document || !spvisParent)
		//		return FALSE;
		//	DWORD count = spvisParent->GetChildCount();
		//	if (count <= 0)
		//		return TRUE;
		//	TinyPoint posL;
		//	TinyPoint posR;
		//	TinySize sizeT = spvisParent->GetSize();
		//	posR.x = sizeT.cx;
		//	TinyVisual* spvis = document->GetVisual(spvisParent, CMD_CHILD);
		//	spvis = document->GetVisual(spvis, CMD_FIRST);
		//	while (spvis != NULL)
		//	{
		//		TinySize size = spvis->GetSize();
		//		size.cx = size.cx == 0 ? sizeT.cx : size.cx;
		//		size.cy = size.cy == 0 ? sizeT.cy : size.cy;
		//		TinyRectangle s = spvis->GetMargin();
		//		size.cx = size.cx - (s.left + s.right);
		//		size.cy = size.cy - (s.top + s.bottom);
		//		TinySize minsize = spvis->GetMinimumSize();
		//		if (!minsize.IsEmpty())
		//		{
		//			size.cx = size.cx < minsize.cx ? minsize.cx : size.cx;
		//			size.cy = size.cy < minsize.cy ? minsize.cy : size.cy;
		//		}
		//		TinySize maxsize = spvis->GetMaximumSize();
		//		if (!maxsize.IsEmpty())
		//		{
		//			size.cx = size.cx > maxsize.cx ? maxsize.cx : size.cx;
		//			size.cy = size.cy > maxsize.cy ? maxsize.cy : size.cy;
		//		}
		//		spvis->SetSize(size);
		//		////如果是布局控件
		//		//if (spvisParent->IsKindOf(RUNTIME_CLASS(TinyVisualPanel)))
		//		//{
		//		//	TinyVisualPanel* panel = static_cast<TinyVisualPanel*>(spvisParent);
		//		//	switch (panel->GetOrientation())
		//		//	{
		//		//	case Orientation::Horizontal:
		//		//	{
		//		//		/*Alignment alignment = panel->GetAlignment();
		//		//		switch (alignment)
		//		//		{
		//		//		case Alignment::RIGHT:
		//		//		{
		//		//			posR.x -= size.cx;
		//		//			posR.x -= (s.left + s.right);
		//		//			posR.y = s.top;
		//		//			spvis->SetPosition(posR);
		//		//		}
		//		//		break;
		//		//		case Alignment::LEFT:
		//		//		{
		//		//			posL.x += size.cx;
		//		//			posL.y = s.top;
		//		//			spvis->SetPosition(posL);
		//		//		}
		//		//		break;
		//		//		}*/
		//		//	}
		//		//	break;
		//		//	}
		//		//}
		//		//CalculateLayout(spvis, document);
		//		spvis = document->GetVisual(spvis, CMD_NEXT);
		//	}
		//}

		//BOOL TinyVisualBuilder::BuildProperty(const TiXmlElement* pXMLNode, TinyVisual* spvis)
		//{
		//	if (!pXMLNode)
		//		return FALSE;

		//	const TiXmlAttribute* pFA = pXMLNode->FirstAttribute();
		//	const TiXmlAttribute* pLA = pXMLNode->LastAttribute();
		//	while (pFA != pLA)
		//	{
		//		string value = UTF8ToASCII(pFA->Value());
		//		if (spvis->SetProperty(pFA->Name(), value.c_str()))
		//		{
		//			spvis->m_propertys.InsertLast(TinyVisualProperty(pFA->Name(), value.c_str()));
		//		}
		//		pFA = pFA->Next();
		//	}
		//	string value = UTF8ToASCII(pFA->Value());
		//	if (spvis->SetProperty(pFA->Name(), UTF8ToASCII(pFA->Value()).c_str()))
		//	{
		//		spvis->m_propertys.InsertLast(TinyVisualProperty(pFA->Name(), value.c_str()));
		//	}
		//	return TRUE;
		//}
		//TinySize TinyVisualBuilder::GetSize(const TinyString& str)
		//{
		//	TinyArray<TinyString> sps;
		//	str.Split(',', sps);
		//	if (sps.GetSize() == 2)
		//	{
		//		return (TinySize(atoi(sps[0].STR()), atoi(sps[1].STR())));
		//	}
		//	return TinySize();
		//}
		//TinyPoint TinyVisualBuilder::GetPosition(const TinyString& str)
		//{
		//	TinyArray<TinyString> sps;
		//	str.Split(',', sps);
		//	if (sps.GetSize() == 2)
		//	{
		//		return (TinyPoint(atoi(sps[0].STR()), atoi(sps[1].STR())));
		//	}
		//	return TinyPoint();
		//}
		//TinyRectangle TinyVisualBuilder::GetRectangle(const TinyString& str)
		//{
		//	TinyArray<TinyString> sps;
		//	str.Split(',', sps);
		//	if (sps.GetSize() == 4)
		//	{
		//		return (TinyRectangle(atoi(sps[0].STR()), atoi(sps[1].STR()), atoi(sps[2].STR()), atoi(sps[3].STR())));
		//	}
		//	return TinyRectangle();
		//}
		//BOOL TinyVisualBuilder::GetBool(const TinyString& str)
		//{
		//	return str.Compare("true") == 0;
		//}
		//INT	TinyVisualBuilder::GetInt32(const TinyString& str)
		//{
		//	return atoi(str.CSTR());
		//}
		//UINT TinyVisualBuilder::GetAlign(const TinyString& str)
		//{
		//	UINT sFlag = DT_LEFT | DT_SINGLELINE;
		//	TinyArray<TinyString> sps;
		//	str.Split('|', sps);
		//	for (INT i = 0; i < sps.GetSize(); i++)
		//	{
		//		if (strcasecmp(sps[i].STR(), "left") == 0)
		//			sFlag |= DT_LEFT;
		//		if (strcasecmp(sps[i].STR(), "top") == 0)
		//			sFlag |= DT_TOP;
		//		if (strcasecmp(sps[i].STR(), "right") == 0)
		//			sFlag |= DT_RIGHT;
		//		if (strcasecmp(sps[i].STR(), "bottom") == 0)
		//			sFlag |= DT_BOTTOM;
		//		if (strcasecmp(sps[i].STR(), "center") == 0)
		//			sFlag |= DT_CENTER;
		//		if (strcasecmp(sps[i].STR(), "vcenter") == 0)
		//			sFlag |= DT_VCENTER;
		//	}
		//	return sFlag;
		//}
		//HorizontalAlignment TinyVisualBuilder::GetHorizontalAlignment(const TinyString& str)
		//{
		//	if (strcasecmp(str.STR(), "left") == 0)
		//		return HorizontalAlignment::LEFT;
		//	if (strcasecmp(str.STR(), "right") == 0)
		//		return HorizontalAlignment::RIGHT;
		//	if (strcasecmp(str.STR(), "center") == 0)
		//		return HorizontalAlignment::CENTER;
		//	if (strcasecmp(str.STR(), "stretch") == 0)
		//		return HorizontalAlignment::STRETCH;
		//	return HorizontalAlignment::NONE;
		//}
		//VerticalAlignment TinyVisualBuilder::GetVerticalAlignment(const TinyString& str)
		//{
		//	if (strcasecmp(str.STR(), "top") == 0)
		//		return VerticalAlignment::TOP;
		//	if (strcasecmp(str.STR(), "bottom") == 0)
		//		return VerticalAlignment::BOTTOM;
		//	if (strcasecmp(str.STR(), "center") == 0)
		//		return VerticalAlignment::CENTER;
		//	if (strcasecmp(str.STR(), "stretch") == 0)
		//		return VerticalAlignment::STRETCH;
		//	return VerticalAlignment::NONE;
		//}
		//COLORREF TinyVisualBuilder::GetColor(const TinyString& str)
		//{
		//	TinyArray<TinyString> sps;
		//	str.Split(',', sps);
		//	if (sps.GetSize() == 3)
		//	{
		//		return RGB(atoi(sps[0].STR()), atoi(sps[1].STR()), atoi(sps[2].STR()));
		//	}
		//	return RGB(255, 255, 255);
		//}
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
			Register(TinyVisualTagConst::WINDOW, CLASS_NAME(TinyVisualWindow));
			Register(TinyVisualTagConst::TEXTBOX, CLASS_NAME(TinyVisualTextBox));
			Register(TinyVisualTagConst::BUTTON, CLASS_NAME(TinyVisualButton));
			Register(TinyVisualTagConst::LABEL, CLASS_NAME(TinyVisualLabel));
			Register(TinyVisualTagConst::VSCROLLBAR, CLASS_NAME(TinyVisualVScrollBar));
			Register(TinyVisualTagConst::HSCROLLBAR, CLASS_NAME(TinyVisualHScrollBar));
			Register(TinyVisualTagConst::SYSCAPTION, CLASS_NAME(TinyVisualCaption));
			Register(TinyVisualTagConst::COMBOBOX, CLASS_NAME(TinyVisualComboBox));
			Register(TinyVisualTagConst::OPTION, CLASS_NAME(TinyVisualOption));
			Register(TinyVisualTagConst::PANEL, CLASS_NAME(TinyVisualPanel));
			Register(TinyVisualTagConst::TAB, CLASS_NAME(TinyVisualTagConst));
			Register(TinyVisualTagConst::NATIVEWINDOW, CLASS_NAME(TinyVisualNative));
			Register(TinyVisualTagConst::LIST, CLASS_NAME(TinyVisualList));
			Register(TinyVisualTagConst::LISTITEM, CLASS_NAME(TinyVisualListItem));
			Register(TinyVisualTagConst::ANIMATION, CLASS_NAME(TinyVisualAnimation));
			Register(TinyVisualTagConst::PANEL, CLASS_NAME(TinyVisualPanel));
			Register(TinyVisualTagConst::CONTEXTMENU, CLASS_NAME(TinyVisualContextMenu));
			Register(TinyVisualTagConst::MENUITEM, CLASS_NAME(TinyVisualMenuItem));
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
				if (pXMLNode && !strcasecmp(pXMLNode->Value(), TinyVisualTagConst::CONTEXT.STR()))
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
				if (!strcasecmp(pXMLChildNode->Value(), TinyVisualTagConst::IMAGE.STR()))
				{
					BuildImage(pXMLChildNode);
				}
			}
		}
		void TinyVisualResource::BuildImage(const TiXmlNode* pXMLNode)
		{
			for (const TiXmlNode* pXMLChildNode = pXMLNode->FirstChild(); pXMLChildNode; pXMLChildNode = pXMLChildNode->NextSibling())
			{
				if (!strcasecmp(pXMLChildNode->Value(), TinyVisualTagConst::ADD.STR()))
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
