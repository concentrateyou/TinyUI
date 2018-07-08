#include "../stdafx.h"
#include "../Common/TinyString.h"
#include "TinyVisualCommon.h"
#include "TinyVisualManage.h"
#include "TinyVisualWindowless.h"
#include "TinyVisualLabel.h"

namespace TinyFramework
{
	namespace Windowless
	{
		IMPLEMENT_DYNCREATE(TinyVisualLabel, TinyVisual);

		TinyVisualLabel::TinyVisualLabel()
		{

		}
		TinyVisualLabel::TinyVisualLabel(TinyVisual* spvisParent, TinyVisualDocument* document)
			:TinyVisual(spvisParent, document)
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
			return TinyVisual::OnDraw(hDC, rcPaint);
		}
	}
}
