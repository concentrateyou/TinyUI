#pragma once
#include "Windowless/TinyVisual.h"
using namespace TinyUI;
using namespace TinyUI::Windowless;

namespace MShow
{
	/// <summary>
	/// ÒôÁ¿·Ö±´¿Ø¼þ
	/// </summary>
	class MAudioDB : public TinyVisual
	{
		friend class TinyVisualDocument;
		DECLARE_DYNCREATE(MAudioDB)
		DISALLOW_COPY_AND_ASSIGN(MAudioDB)
	protected:
		MAudioDB();
		MAudioDB(TinyVisual* spvisParent, TinyVisualDocument* vtree);
	public:
		virtual ~MAudioDB();
		TinyString RetrieveTag() const OVERRIDE;
	public:
		void SetDB(INT db);
	protected:
		BOOL OnDraw(HDC hDC, const RECT& rcPaint) OVERRIDE;
	private:
		INT	m_db;
	};
}

