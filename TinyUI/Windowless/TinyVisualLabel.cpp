#include "../stdafx.h"
#include "TinyVisualTree.h"
#include "TinyVisualManage.h"
#include "TinyVisualLabel.h"

namespace TinyUI
{
	namespace Windowless
	{
		TinyVisualLabel::TinyVisualLabel(TinyVisual* spvisParent, TinyVisualTree* vtree)
			:TinyVisual(spvisParent, vtree)
		{

		}
		TinyVisualLabel::~TinyVisualLabel()
		{

		}
		TinyString TinyVisualLabel::RetrieveTag() const
		{
			return TinyVisualTag::LABEL;
		}

		BOOL TinyVisualLabel::OnDraw(HDC hDC, const RECT& rcPaint)
		{
			return FALSE;
		}
	}
}
