#pragma once
#include "TinyVisual.h"
#include "../IO/TinyIO.h"
#include "../XML/TinyXMLParse.h"
#include "../Common/TinyCollection.h"
#include "TinyVisualWindow.h"

namespace TinyUI
{
	namespace Windowless
	{
		class TinyVisualPoperty
		{
		public:
			//元素属性
			static const CHAR* NAME;
			static const CHAR* TEXT;
			static const CHAR* TOOLTIP;
			static const CHAR* MAXSIZE;
			static const CHAR* MINSIZE;
			static const CHAR* SIZE;
			static const CHAR* VISIBLE;
			static const CHAR* ENABLE;
		};
		class TinyVisualTag
		{
		public:
			static const CHAR* WINDOW;
			static const CHAR* CAPTION;
			static const CHAR* SYSBUTTON;
		};
		/// <summary>
		/// 可视化元素管理
		/// </summary>
		class TinyVisualParse : public TinyObject
		{
			DECLARE_DYNAMIC(TinyVisualParse)
		public:
			TinyVisualParse();
			~TinyVisualParse();
			BOOL LoadFile(LPCSTR pzFile);
			BOOL BuildVisualTree(TinyVisualTree* ps);
			static TinyVisual* CreateInstace(const TiXmlNode* pXMLNode, TinyVisual* spvisParent, TinyVisualTree* ps);
		private:
			static BOOL GetAttributeMap(const TiXmlElement* pXMLNode, TinyMap<TinyString, TinyString>& map);
			static TinySize	GetSize(const TinyString* ps);
		private:
			TiXmlDocument	m_doc;
		};
	};
}



