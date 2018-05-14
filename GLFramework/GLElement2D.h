#pragma once
#include "GLFramework.h"

namespace GLFramework
{
	class GLElement2D :public TinyObject
	{
		DECLARE_DYNAMIC(GLElement2D)
	public:
		GLElement2D();
		virtual ~GLElement2D();
	public:
		TinyString	GetElementName() const;
		void		SetElementName(const TinyString& name);
	protected:
		TinyString	m_szElement;
	};
}



