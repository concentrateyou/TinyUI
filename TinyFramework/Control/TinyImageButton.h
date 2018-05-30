#pragma once
#include "TinyButton.h"
#include "../Render/TinyImage.h"

namespace TinyFramework
{
	/// <summary>
	/// ÈýÌ¬°´Å¥
	/// </summary>
	class TinyImageButton : public TinyButton
	{
		DECLARE_DYNAMIC(TinyImageButton)
	public:
		TinyImageButton();
		virtual ~TinyImageButton();
		BOOL SetFiles(const TinyString& szFile1, const TinyString& szFile2, const TinyString& szFile3);
		BOOL IsPushButton() const;
		void SetTextAlignment(DWORD dwAlignment);
		virtual INT GetTextAlignment() const;
	public:
		LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	private:
		void OnDraw(HDC hDC);
		void CalcRect(HDC hDC, const TinyString& szText, TinyRectangle& rectangle, UINT& wFlag);
	private:
		INT			m_state;
		TinyImage	m_images[3];
	};
}


