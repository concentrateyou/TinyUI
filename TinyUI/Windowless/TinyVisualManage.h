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
			DISALLOW_COPY_AND_ASSIGN(TinyVisualBuilder)
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
			static INT	GetInt32(const TinyString& str);
		private:
			static void CreateInstace(const TiXmlNode* pXMLNode, TinyVisual* spvisParent, TinyVisualDocument* ps);
			static BOOL BuildProperty(const TiXmlElement* pXMLNode, TinyVisual* spvis);
		private:
			TiXmlDocument	m_doc;
		};
		/// <summary>
		/// 资源管理
		/// </summary>
		class TinyVisualResource
		{
			DISALLOW_COPY_AND_ASSIGN(TinyVisualResource)
		public:
			TinyVisualResource();
			~TinyVisualResource();
			static TinyVisualResource& GetInstance();
			BOOL LoadResource(LPCSTR pzFile);
			string GetDefaultPath();
		public:
			TinyImage* Add(const TinyString& szName,const TinyString& szFile);
			void Remove(const TinyString& szName);
			void RemoveAll();
			TinyImage* operator[](const TinyString& szName);
		private:
			static void BuildContext(const TiXmlNode* pXMLNode);
			static void BuildImage(const TiXmlNode* pXMLNode);
		private:
			string			m_szPath;
			TiXmlDocument	m_doc;
			TinyMap<TinyString, TinyImage*> m_images;
			
		};
	};
}



