#pragma once
#include "TinyVisual.h"
#include "../IO/TinyIO.h"
#include "../XML/TinyXMLParse.h"
#include "../Common/TinyCollection.h"
#include "TinyVisualCommon.h"
#include "TinyVisualWindow.h"

namespace TinyUI
{
	namespace Windowless
	{

		class TinyVisualDocument;
		/// <summary>
		/// 可视化元素管理
		/// </summary>
		class TinyVisualBuilder : public TinyObject
		{
			DECLARE_DYNAMIC(TinyVisualBuilder)
		public:
			TinyVisualBuilder();
			~TinyVisualBuilder();
			BOOL LoadFile(LPCSTR pzFile);
			BOOL BuildDocument(TinyVisualDocument* ps);
			static void CreateInstace(const TiXmlNode* pXMLNode, TinyVisual* spvisParent, TinyVisualDocument* ps);
		private:
			static void BuildProperty(TinyMap<TinyString, TinyString> &map, TinyVisual* spvis);
			static BOOL GetAttributeMap(const TiXmlElement* pXMLNode, TinyMap<TinyString, TinyString>& map);
			static TinySize	 GetSize(const TinyString* ps);
			static TinyPoint GetPosition(const TinyString* ps);
			static BOOL	GetBool(const TinyString* str);
		private:
			TiXmlDocument	m_doc;
		};
	};
}



