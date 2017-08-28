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
		class TinyVisualBuilder 
		{
		public:
			TinyVisualBuilder();
			~TinyVisualBuilder();
			BOOL LoadFile(LPCSTR pzFile);
			BOOL BuildDocument(TinyVisualDocument* ps);
			static TinySize	 GetSize(const TinyString& str);
			static TinyPoint GetPosition(const TinyString& str);
			static TinyRectangle GetRectangle(const TinyString& str);
			static UINT	 GetAlign(const TinyString& str);
			static Alignment GetAlignment(const TinyString& str);
			static COLORREF	 GetColor(const TinyString& str);
			static BOOL	GetBool(const TinyString& str);
		private:
			static void CreateInstace(const TiXmlNode* pXMLNode, TinyVisual* spvisParent, TinyVisualDocument* ps);
			static BOOL BuildProperty(const TiXmlElement* pXMLNode, TinyVisual* spvis);
		private:
			TiXmlDocument	m_doc;
		};
	};
}



