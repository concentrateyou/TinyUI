#include "../stdafx.h"
#include "TinyVisualUtility.h"
#include "TinyVisualDocument.h"

namespace TinyUI
{
	namespace Windowless
	{
		LONG HimetricXtoDX(LONG xHimetric, LONG xPerInch)
		{
			return (LONG)MulDiv(xHimetric, xPerInch, HIMETRIC_PER_INCH);
		}
		LONG HimetricYtoDY(LONG yHimetric, LONG yPerInch)
		{
			return (LONG)MulDiv(yHimetric, yPerInch, HIMETRIC_PER_INCH);
		}
		LONG DXtoHimetricX(LONG dx, LONG xPerInch)
		{
			return (LONG)MulDiv(dx, HIMETRIC_PER_INCH, xPerInch);
		}
		LONG DYtoHimetricY(LONG dy, LONG yPerInch)
		{
			return (LONG)MulDiv(dy, HIMETRIC_PER_INCH, yPerInch);
		}
		//////////////////////////////////////////////////////////////////////////
		TinyClipCanvas::TinyClipCanvas(HDC hDC, TinyVisual* spvis, const RECT& rcPaint)
			:TinyCanvas(hDC),
			m_hRGN(NULL),
			m_hOldRGN(NULL)
		{
			ASSERT(spvis);
			RECT clip = { 0 };
			::GetClipBox(hDC, &clip);
			m_hOldRGN = ::CreateRectRgnIndirect(&clip);
			m_hRGN = ::CreateRectRgnIndirect(&rcPaint);
			if (spvis->GetClip())
			{
				::CombineRgn(m_hRGN, m_hRGN, spvis->GetClip(), RGN_AND);
			}
			::ExtSelectClipRgn(m_hDC, m_hRGN, RGN_AND);
		}
		TinyClipCanvas::~TinyClipCanvas()
		{
			::SelectClipRgn(m_hDC, m_hOldRGN);
			SAFE_DELETE_OBJECT(m_hOldRGN);
			SAFE_DELETE_OBJECT(m_hRGN);
		}
		//////////////////////////////////////////////////////////////////////////
		TinyVisualFilters::TinyVisualFilters()
		{

		}
		TinyVisualFilters::~TinyVisualFilters()
		{
		}
		BOOL TinyVisualFilters::Add(TinyVisualFilter*  element)
		{
			return m_array.Add(element);
		}
		BOOL TinyVisualFilters::Remove(TinyVisualFilter* element)
		{
			return m_array.Remove(element);
		}
		BOOL TinyVisualFilters::RemoveAt(INT index)
		{
			return m_array.RemoveAt(index);
		}
		void TinyVisualFilters::RemoveAll()
		{
			m_array.RemoveAll();
		}
		TinyVisualFilter* TinyVisualFilters::operator[](INT index)
		{
			return m_array[index];
		}
		const TinyVisualFilter* TinyVisualFilters::operator[](INT index) const
		{
			return m_array[index];
		}
		INT TinyVisualFilters::Lookup(TinyVisualFilter* element) const
		{
			return m_array.Lookup(element);
		}
		INT  TinyVisualFilters::GetSize() const
		{
			return m_array.GetSize();
		}
	}
}
