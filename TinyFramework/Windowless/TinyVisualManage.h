#pragma once
#include "TinyVisual.h"
#include "../IO/TinyIO.h"
#include "../XML/TinyXMLParse.h"
#include "../Common/TinyCollection.h"
#include "TinyVisualCommon.h"
#include "TinyVisualWindow.h"

namespace TinyFramework
{
	namespace Windowless
	{
		class TinyVisualDocument;
		/*/// <summary>
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
			static TinySize				GetSize(const TinyString& str);
			static TinyPoint			GetPosition(const TinyString& str);
			static TinyRectangle		GetRectangle(const TinyString& str);
			static UINT					GetAlign(const TinyString& str);
			static HorizontalAlignment	GetHorizontalAlignment(const TinyString& str);
			static VerticalAlignment	GetVerticalAlignment(const TinyString& str);
			static COLORREF				GetColor(const TinyString& str);
			static BOOL					GetBool(const TinyString& str);
			static INT					GetInt32(const TinyString& str);
		private:
			static void CreateInstace(const TiXmlNode* pXMLNode, TinyVisual* spvisParent, TinyVisualDocument* ps);
			static BOOL CalculateLayout(TinyVisual* spvisParent, TinyVisualDocument* document);
			static BOOL BuildProperty(const TiXmlElement* pXMLNode, TinyVisual* spvis);
		private:
			TiXmlDocument	m_doc;
		};*/
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
			BOOL Load(LPCSTR pzFile);
			string GetDefaultPath();
		public:
			BOOL		Add(TinyImage* image);
			TinyImage*	Add(const TinyString& szName, const TinyString& szFile);
			void		Remove(const TinyString& szName);
			void		Remove(TinyImage* image);
			void		RemoveAll();
			TinyImage* operator[](const TinyString& szName);
		public:
			BOOL		Register(const TinyString& tag, const TinyString& value);
			void		Unregister(const TinyString& tag);
			TinyString	GetClassName(const TinyString& tag);
		private:
			static void BuildContext(const TiXmlNode* pXMLNode);
			static void BuildImage(const TiXmlNode* pXMLNode);
		private:
			string							m_szPath;
			TinyLock						m_lock;
			TiXmlDocument					m_doc;
			TinyArray<TinyImage*>			m_images1;//动态
			TinyMap<TinyString, TinyImage*> m_images2;//静态
			TinyMap<TinyString, TinyString>	m_types;
		};
	};
}



