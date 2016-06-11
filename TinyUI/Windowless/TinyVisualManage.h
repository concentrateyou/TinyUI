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
		class TinyVisualFactory;
		class TinyVisualTree;
		/// <summary>
		/// Visual工厂类
		/// </summary>
		class TinyVisualFactory
		{
			friend class TinyVisual;
			friend class TinyVisualTree;
			DISALLOW_COPY_AND_ASSIGN(TinyVisualFactory);
		private:
			TinyVisualFactory(TinyVisualTree* vtree);
		public:
			TinyVisual* Create(INT x, INT y, INT cx, INT cy, TinyVisual* spvisParent, const TinyString& tag);
			BOOL		Destory(TinyVisual* spvis);
		private:
			TinyVisualTree* m_vtree;
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
			static void CreateInstace(const TiXmlNode* pXMLNode, TinyVisual* spvisParent, TinyVisualTree* ps);
		private:
			static void BuildProperty(TinyMap<TinyString, TinyString> &map, TinyVisual* spvis);
			static BOOL GetAttributeMap(const TiXmlElement* pXMLNode, TinyMap<TinyString, TinyString>& map);
			static TinySize	GetSize(const TinyString* ps);
			static BOOL	GetBool(const TinyString* str);
		private:
			TiXmlDocument	m_doc;
		};
	};
}



