#include "TinyRectTracker.h"

namespace TinyUI
{
	static HCURSOR g_Cursors[10] = { 0, };
	static HBRUSH g_HatchBrush = 0;
	static HBRUSH g_HalftoneBrush = 0;
	static HPEN g_BlackDottedPen = 0;
	static INT g_HandleSize = 5;

	struct HANDLEINFO
	{
		size_t nOffsetX;
		size_t nOffsetY;
		int nCenterX;
		int nCenterY;
		int nHandleX;
		int nHandleY;
		int nInvertX;
		int nInvertY;
	};

	static const HANDLEINFO g_HandleInfo[] =
	{
		{ offsetof(RECT, left), offsetof(RECT, top),        0, 0,  0,  0, 1, 3 },
		{ offsetof(RECT, right), offsetof(RECT, top),       0, 0, -1,  0, 0, 2 },
		{ offsetof(RECT, right), offsetof(RECT, bottom),    0, 0, -1, -1, 3, 1 },
		{ offsetof(RECT, left), offsetof(RECT, bottom),     0, 0,  0, -1, 2, 0 },
		{ offsetof(RECT, left), offsetof(RECT, top),        1, 0,  0,  0, 4, 6 },
		{ offsetof(RECT, right), offsetof(RECT, top),       0, 1, -1,  0, 7, 5 },
		{ offsetof(RECT, left), offsetof(RECT, bottom),     1, 0,  0, -1, 6, 4 },
		{ offsetof(RECT, left), offsetof(RECT, top),        0, 1,  0,  0, 5, 7 }
	};
	struct RECTINFO
	{
		size_t nOffsetAcross;
		int nSignAcross;
	};
	static const RECTINFO g_RectInfo[] =
	{
		{ offsetof(RECT, right), +1 },
		{ offsetof(RECT, bottom), +1 },
		{ offsetof(RECT, left), -1 },
		{ offsetof(RECT, top), -1 },
	};

	TinyRectTracker::TinyRectTracker()
	{
		Construct();
	}

	TinyRectTracker::TinyRectTracker(LPCRECT lpSrcRect, UINT nStyle)
	{
		Construct();
		m_rect.CopyRect(lpSrcRect);
		m_nStyle |= nStyle;
	}
	TinyRectTracker::TinyRectTracker(INT x, INT y, INT cx, INT cy, UINT nStyle)
	{
		Construct();
		RECT rect = { x, y, x + cx,y + cy };
		m_rect.CopyRect(&rect);
		m_nStyle |= nStyle;
	}
	void TinyRectTracker::Release()
	{
		if (g_HatchBrush != NULL)
		{
			DeleteObject(g_HatchBrush);
			g_HatchBrush = NULL;
		}
		if (g_HalftoneBrush != NULL)
		{
			DeleteObject(g_HalftoneBrush);
			g_HalftoneBrush = NULL;
		}
		if (g_BlackDottedPen != NULL)
		{
			DeleteObject(g_BlackDottedPen);
			g_BlackDottedPen = NULL;
		}
	}

	void TinyRectTracker::Construct()
	{
		static BOOL bInitialized;
		if (!bInitialized)
		{
			ASSERT(sizeof(((RECT*)NULL)->left) == sizeof(int));
			ASSERT(offsetof(RECT, top) > offsetof(RECT, left));
			ASSERT(offsetof(RECT, right) > offsetof(RECT, top));
			ASSERT(offsetof(RECT, bottom) > offsetof(RECT, right));
			if (g_HatchBrush == NULL)
			{
				WORD hatchPattern[8];
				WORD wPattern = 0x1111;
				for (int i = 0; i < 4; i++)
				{
					hatchPattern[i] = wPattern;
					hatchPattern[i + 4] = wPattern;
					wPattern <<= 1;
				}
				HBITMAP hatchBitmap = CreateBitmap(8, 8, 1, 1, hatchPattern);
				if (hatchBitmap == NULL)
				{
					return;
				}
				g_HatchBrush = CreatePatternBrush(hatchBitmap);
				DeleteObject(hatchBitmap);
				if (g_HatchBrush == NULL)
				{
					return;
				}
			}
			if (g_HalftoneBrush == NULL)
			{
				WORD grayPattern[8];
				for (int i = 0; i < 8; i++)
					grayPattern[i] = (WORD)(0x5555 << (i & 1));
				HBITMAP grayBitmap = CreateBitmap(8, 8, 1, 1, grayPattern);
				if (grayBitmap != NULL)
				{
					g_HalftoneBrush = ::CreatePatternBrush(grayBitmap);
					DeleteObject(grayBitmap);
					if (g_HalftoneBrush == NULL)
					{
						return;
					}
				}
			}
			if (g_BlackDottedPen == NULL)
			{
				g_BlackDottedPen = CreatePen(PS_DOT, 0, RGB(0, 0, 0));
				if (g_BlackDottedPen == NULL)
				{
					return;
				}
			}
			g_Cursors[0] = ::LoadCursor(NULL, IDC_SIZENWSE);
			g_Cursors[1] = ::LoadCursor(NULL, IDC_SIZENESW);
			g_Cursors[2] = g_Cursors[0];
			g_Cursors[3] = g_Cursors[1];
			g_Cursors[4] = ::LoadCursor(NULL, IDC_SIZENS);
			g_Cursors[5] = ::LoadCursor(NULL, IDC_SIZEWE);
			g_Cursors[6] = g_Cursors[4];
			g_Cursors[7] = g_Cursors[5];
			g_Cursors[8] = ::LoadCursor(NULL, IDC_SIZEALL);
			g_Cursors[9] = ::LoadCursor(NULL, IDC_SIZEALL);
			bInitialized = TRUE;
		}
		m_nStyle = resizeOutside;
		m_nHandleSize = g_HandleSize;
		m_sizeMin.cy = m_sizeMin.cx = m_nHandleSize * 2;
		m_rectLast.SetRectEmpty();
		m_sizeLast.cx = m_sizeLast.cy = 0;
		m_bErase = FALSE;
		m_bFinalErase = FALSE;
	}

	TinyRectTracker::~TinyRectTracker()
	{
	}

	void TinyRectTracker::Draw(TinyDC* pDC)
	{
		pDC->SaveDC();
		pDC->SetMapMode(MM_TEXT);
		pDC->SetViewportOrg(0, 0);
		pDC->SetWindowOrg(0, 0);
		pDC->SetBkMode(TRANSPARENT);

		TinyRectangle rect = m_rect;
		rect.NormalizeRect();
		rect.InflateRect(1, 1, 1, 1);
		HPEN hOldPen = NULL;
		HBRUSH hOldBrush = NULL;
		HGDIOBJ hGDIObj = NULL;
		INT nOldROP;
		if ((m_nStyle & (dottedLine | solidLine)) != 0)
		{
			if (m_nStyle & dottedLine)
			{
				hOldPen = (HPEN)pDC->SelectObject(g_BlackDottedPen);
			}
			else
			{
				hOldPen = (HPEN)pDC->SelectStockObject(BLACK_PEN);
			}
			hOldBrush = (HBRUSH)pDC->SelectStockObject(NULL_BRUSH);
			nOldROP = pDC->SetROP2(R2_COPYPEN);
			rect.InflateRect(+1, +1);
			pDC->Rectangle(rect.left, rect.top, rect.right, rect.bottom);
			pDC->SetROP2(nOldROP);
		}
		if ((m_nStyle & (hatchInside | hatchedBorder)) != 0)
		{
			UnrealizeObject(g_HatchBrush);
		}

		if ((m_nStyle & hatchInside) != 0)
		{
			hGDIObj = pDC->SelectStockObject(NULL_PEN);
			if (hOldPen == NULL)
			{
				hOldPen = (HPEN)hGDIObj;
			}
			hGDIObj = pDC->SelectObject(g_HatchBrush);
			if (hOldBrush == NULL)
			{
				hOldBrush = (HBRUSH)hGDIObj;
			}
			pDC->SetBkMode(TRANSPARENT);
			nOldROP = pDC->SetROP2(R2_MASKNOTPEN);
			pDC->Rectangle(rect.left + 1, rect.top + 1, rect.right, rect.bottom);
			pDC->SetROP2(nOldROP);
		}
		if ((m_nStyle & hatchedBorder) != 0)
		{
			hGDIObj = pDC->SelectObject(g_HatchBrush);
			if (hOldBrush == NULL)
			{
				hOldBrush = (HBRUSH)hGDIObj;
			}
			pDC->SetBkMode(OPAQUE);
			TinyRectangle rectTrue;
			GetTrueRect(&rectTrue);
			pDC->PatBlt(rectTrue.left, rectTrue.top, rectTrue.Width(), rect.top - rectTrue.top, 0x000F0001 /* Pn */);
			pDC->PatBlt(rectTrue.left, rect.bottom, rectTrue.Width(), rectTrue.bottom - rect.bottom, 0x000F0001 /* Pn */);
			pDC->PatBlt(rectTrue.left, rect.top, rect.left - rectTrue.left, rect.Height(), 0x000F0001 /* Pn */);
			pDC->PatBlt(rect.right, rect.top, rectTrue.right - rect.right, rect.Height(), 0x000F0001 /* Pn */);
		}
		if ((m_nStyle & (resizeInside | resizeOutside)) != 0)
		{
			UINT mask = GetHandleMask();
			for (int i = 0; i < 8; ++i)
			{
				if (mask & (1 << i))
				{
					GetHandleRect((TrackerHit)i, &rect);
					pDC->FillSolidRect(rect, RGB(63, 83, 93));
				}
			}
		}
		if (hOldPen != NULL)
		{
			pDC->SelectObject(hOldPen);
		}
		if (hOldBrush != NULL)
		{
			pDC->SelectObject(hOldBrush);
		}
		pDC->RestoreDC(-1);
	}

	BOOL TinyRectTracker::SetCursor(TinyControl* pWindow, UINT nHitTest)
	{
		if (nHitTest != HTCLIENT)
		{
			return FALSE;
		}
		TinyPoint point;
		GetCursorPos(&point);
		pWindow->ScreenToClient(&point);
		int nHandle = HitTestHandles(point);
		if (nHandle < 0)
		{
			::SetCursor(g_Cursors[10]);
			return FALSE;
		}
		nHandle = NormalizeHit(nHandle);
		if (nHandle == hitMiddle && !m_rect.PtInRect(point))
		{
			if (m_nStyle & hatchedBorder)
			{
				nHandle = (TrackerHit)9;
			}
		}
		::SetCursor(g_Cursors[nHandle]);
		return TRUE;
	}

	int TinyRectTracker::HitTest(const TinyPoint& point)
	{
		TrackerHit hitResult = hitNothing;
		TinyRectangle rectTrue;
		GetTrueRect(&rectTrue);
		ASSERT(rectTrue.left <= rectTrue.right);
		ASSERT(rectTrue.top <= rectTrue.bottom);
		if (rectTrue.PtInRect(point))
		{
			if ((m_nStyle & (resizeInside | resizeOutside)) != 0)
			{
				hitResult = (TrackerHit)HitTestHandles(point);
			}
			else
			{
				hitResult = hitMiddle;
			}
		}
		return hitResult;
	}

	int TinyRectTracker::NormalizeHit(int nHandle)
	{
		ASSERT(nHandle <= 8 && nHandle >= -1);
		if (nHandle == hitMiddle || nHandle == hitNothing)
		{
			return nHandle;
		}
		ASSERT(0 <= nHandle && nHandle < _countof(g_HandleInfo));
		const HANDLEINFO* pHandleInfo = &g_HandleInfo[nHandle];
		if (m_rect.Width() < 0)
		{
			nHandle = (TrackerHit)pHandleInfo->nInvertX;
			ASSERT(0 <= nHandle && nHandle < _countof(g_HandleInfo));
			pHandleInfo = &g_HandleInfo[nHandle];
		}
		if (m_rect.Height() < 0)
		{
			nHandle = (TrackerHit)pHandleInfo->nInvertY;
		}
		return nHandle;
	}

	BOOL TinyRectTracker::Track(TinyControl* pWindow, const TinyPoint& point, BOOL bAllowInvert, TinyControl* pWindowClip)
	{
		INT nHandle = HitTestHandles(point);
		if (nHandle < 0)
		{
			return FALSE;
		}
		m_bAllowInvert = bAllowInvert;
		return TrackHandle(nHandle, pWindow, point, pWindowClip);
	}

	BOOL TinyRectTracker::TrackRubberBand(TinyControl* pWindow, const TinyPoint& point, BOOL bAllowInvert)
	{
		m_bAllowInvert = bAllowInvert;
		m_rect.SetRect(point.x, point.y, point.x, point.y);
		return TrackHandle(hitBottomRight, pWindow, point, NULL);
	}
	void TinyRectTracker::DrawDragRect(TinyDC* pDC, LPCRECT lpRect, SIZE size, LPCRECT lpRectLast, SIZE sizeLast, HBRUSH hBrush, HBRUSH hBrushLast)
	{
		ASSERT(pDC != NULL);
		TinyRgn rgnNew;
		TinyRgn rgnOutside, rgnInside;
		rgnOutside.CreateRgn(lpRect);
		TinyRectangle rect = *lpRect;
		rect.InflateRect(-size.cx, -size.cy);
		rect.IntersectRect(rect, lpRect);
		rgnInside.CreateRgn(rect);
		rgnNew.CreateRgn(0, 0, 0, 0);
		rgnNew.CombineRgn(&rgnOutside, &rgnInside, RGN_XOR);

		HBRUSH hBrushOld = NULL;
		if (hBrush == NULL)
		{
			hBrush = g_HalftoneBrush;
		}
		ASSERT(hBrush);
		if (hBrushLast == NULL)
		{
			hBrushLast = hBrush;
		}
		TinyRgn rgnLast, rgnUpdate;
		if (lpRectLast != NULL)
		{
			rgnLast.CreateRgn(0, 0, 0, 0);
			rgnOutside.SetRectRgn(lpRectLast);
			rect = *lpRectLast;
			rect.InflateRect(-sizeLast.cx, -sizeLast.cy);
			rect.IntersectRect(rect, lpRectLast);
			rgnInside.SetRectRgn(rect);
			rgnLast.CombineRgn(&rgnOutside, &rgnInside, RGN_XOR);
			if (hBrush == hBrushLast)
			{
				rgnUpdate.CreateRgn(0, 0, 0, 0);
				rgnUpdate.CombineRgn(&rgnLast, &rgnNew, RGN_XOR);
			}
		}
		if (hBrush != hBrushLast && lpRectLast != NULL)
		{
			pDC->SelectClipRgn(&rgnLast);
			pDC->GetClipBox(&rect);
			hBrushOld = (HBRUSH)pDC->SelectObject(hBrushLast);
			pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATINVERT);
			pDC->SelectObject(hBrushOld);
			hBrushOld = NULL;
		}
		pDC->SelectClipRgn(rgnUpdate.m_hHRGN != NULL ? &rgnUpdate : &rgnNew);
		pDC->GetClipBox(&rect);
		hBrushOld = (HBRUSH)pDC->SelectObject(hBrush);
		pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATINVERT);
		if (hBrushOld != NULL)
		{
			pDC->SelectObject(hBrushOld);
		}
		pDC->SelectClipRgn(NULL);
	}
	void TinyRectTracker::DrawTrackerRect(LPCRECT lpRect, TinyControl* pWindowClip, TinyDC* pDC, TinyControl* pWindow)
	{
		TinyRectangle rect = *lpRect;
		rect.NormalizeRect();

		if (pWindowClip != NULL)
		{
			pWindow->ClientToScreen(&rect);
			pWindowClip->ScreenToClient(&rect);
		}

		TinySize size(0, 0);
		if (!m_bFinalErase)
		{
			if (m_nStyle & hatchedBorder)
			{
				size.cx = size.cy = max(1, GetHandleSize(rect) - 1);
				rect.InflateRect(size);
			}
			else
			{
				size.cx = 1;
				size.cy = 1;
			}
		}

		if (m_bFinalErase || !m_bErase)
		{
			DrawDragRect(pDC, rect, size, m_rectLast, m_sizeLast);
		}

		m_rectLast = rect;
		m_sizeLast = size;
	}

	void TinyRectTracker::AdjustRect(int nHandle, LPRECT)
	{
		if (nHandle == hitMiddle)
		{
			return;
		}

		int *px, *py;
		GetModifyPointers(nHandle, &px, &py, NULL, NULL);

		int nNewWidth = m_rect.Width();
		int nAbsWidth = m_bAllowInvert ? abs(nNewWidth) : nNewWidth;
		if (px != NULL && nAbsWidth < m_sizeMin.cx)
		{
			nNewWidth = nAbsWidth != 0 ? nNewWidth / nAbsWidth : 1;
			ptrdiff_t iRectInfo = (int*)px - (int*)&m_rect;
			ASSERT(0 <= iRectInfo && iRectInfo < _countof(g_RectInfo));
			const RECTINFO* pRectInfo = &g_RectInfo[iRectInfo];
			*px = *(int*)((BYTE*)&m_rect + pRectInfo->nOffsetAcross) + nNewWidth * m_sizeMin.cx * -pRectInfo->nSignAcross;
		}

		int nNewHeight = m_rect.Height();
		int nAbsHeight = m_bAllowInvert ? abs(nNewHeight) : nNewHeight;
		if (py != NULL && nAbsHeight < m_sizeMin.cy)
		{
			nNewHeight = nAbsHeight != 0 ? nNewHeight / nAbsHeight : 1;
			ptrdiff_t iRectInfo = (int*)py - (int*)&m_rect;
			ASSERT(0 <= iRectInfo && iRectInfo < _countof(g_RectInfo));
			const RECTINFO* pRectInfo = &g_RectInfo[iRectInfo];
			*py = *(int*)((BYTE*)&m_rect + pRectInfo->nOffsetAcross) + nNewHeight * m_sizeMin.cy * -pRectInfo->nSignAcross;
		}
	}

	void TinyRectTracker::GetTrueRect(LPRECT lpTrueRect)
	{
		TinyRectangle rect = m_rect;
		rect.NormalizeRect();
		int nInflateBy = 0;
		if ((m_nStyle & (resizeOutside | hatchedBorder)) != 0)
		{
			nInflateBy += GetHandleSize() - 1;
		}
		if ((m_nStyle & (solidLine | dottedLine)) != 0)
		{
			++nInflateBy;
		}
		rect.InflateRect(nInflateBy, nInflateBy);
		*lpTrueRect = rect;
	}

	void TinyRectTracker::OnChangedRect(const TinyRectangle& rectOld)
	{
		ChangedRect(this, rectOld);
	}

	void TinyRectTracker::GetHandleRect(INT nHandle, TinyRectangle* pHandleRect)
	{
		ASSERT(nHandle < 8);

		TinyRectangle rectT = m_rect;
		rectT.NormalizeRect();
		if ((m_nStyle & (solidLine | dottedLine)) != 0)
		{
			rectT.InflateRect(+1, +1);
		}

		nHandle = NormalizeHit(nHandle);
		int size = GetHandleSize();
		if (m_nStyle & resizeOutside)
		{
			rectT.InflateRect(size - 1, size - 1);
		}
		int nWidth = rectT.Width();
		int nHeight = rectT.Height();
		TinyRectangle rect;
		const HANDLEINFO* pHandleInfo = &g_HandleInfo[nHandle];
		rect.left = *(int*)((BYTE*)&rectT + pHandleInfo->nOffsetX);
		rect.top = *(int*)((BYTE*)&rectT + pHandleInfo->nOffsetY);
		rect.left += size * pHandleInfo->nHandleX;
		rect.top += size * pHandleInfo->nHandleY;
		rect.left += pHandleInfo->nCenterX * (nWidth - size) / 2;
		rect.top += pHandleInfo->nCenterY * (nHeight - size) / 2;
		rect.right = rect.left + size;
		rect.bottom = rect.top + size;
		*pHandleRect = rect;
	}

	INT TinyRectTracker::GetHandleSize(LPCRECT lpRect)
	{
		if (lpRect == NULL)
		{
			lpRect = &m_rect;
		}

		int size = m_nHandleSize;
		if (!(m_nStyle & resizeOutside))
		{
			int sizeMax = min(abs(lpRect->right - lpRect->left), abs(lpRect->bottom - lpRect->top));
			if (size * 2 > sizeMax)
			{
				size = sizeMax / 2;
			}
		}
		return size;
	}

	INT TinyRectTracker::HitTestHandles(const TinyPoint& point)
	{
		TinyRectangle rect;
		UINT mask = GetHandleMask();
		GetTrueRect(&rect);
		if (!rect.PtInRect(point))
		{
			return hitNothing;
		}

		for (int i = 0; i < 8; ++i)
		{
			if (mask & (1 << i))
			{
				GetHandleRect((TrackerHit)i, &rect);
				if (rect.PtInRect(point))
				{
					return (TrackerHit)i;
				}
			}
		}
		if ((m_nStyle & hatchedBorder) == 0)
		{
			TinyRectangle rect = m_rect;
			rect.NormalizeRect();
			if ((m_nStyle & dottedLine | solidLine) != 0)
			{
				rect.InflateRect(+1, +1);
			}
			if (!rect.PtInRect(point))
			{
				return hitNothing;
			}
		}
		return hitMiddle;
	}
	void TinyRectTracker::MoveHandle(LPCRECT lpNewRect, LPRECT lpOldRect)
	{

	}
	BOOL TinyRectTracker::TrackHandle(INT nHandle, TinyControl* pWindow, const TinyPoint& point, TinyControl* pWindowClip)
	{
		ASSERT(nHandle >= 0);
		ASSERT(nHandle <= 8);
		if (::GetCapture() != NULL)
		{
			return FALSE;
		}

		ASSERT(!m_bFinalErase);
		int nWidth = m_rect.Width();
		int nHeight = m_rect.Height();
		SetCapture(pWindow->Handle());
		ASSERT(pWindow->Handle() == ::GetCapture());
		pWindow->UpdateWindow();
		if (pWindowClip != NULL)
		{
			pWindowClip->UpdateWindow();
		}
		TinyRectangle rectSave = m_rect;
		int *px, *py;
		int xDiff, yDiff;
		GetModifyPointers(nHandle, &px, &py, &xDiff, &yDiff);
		xDiff = point.x - xDiff;
		yDiff = point.y - yDiff;
		TinyDC drawDC;
		if (pWindowClip != NULL)
		{
			drawDC.Attach(::GetDCEx(pWindowClip->Handle(), NULL, DCX_CACHE));
		}
		else
		{
			drawDC.Attach(::GetDC(pWindow->Handle()));
		}
		TinyRectangle rectOld;
		BOOL bMoved = FALSE;
		for (;;)
		{
			MSG msg;
			::GetMessage(&msg, NULL, 0, 0);
			if (::GetCapture() != pWindow->Handle())
				break;

			switch (msg.message)
			{
			case WM_LBUTTONUP:
			case WM_MOUSEMOVE:
				rectOld = m_rect;
				if (px != NULL)
					*px = GET_X_LPARAM(msg.lParam) - xDiff;
				if (py != NULL)
					*py = GET_Y_LPARAM(msg.lParam) - yDiff;
				if (nHandle == hitMiddle)
				{
					m_rect.right = m_rect.left + nWidth;
					m_rect.bottom = m_rect.top + nHeight;
				}
				AdjustRect(nHandle, &m_rect);
				m_bFinalErase = (msg.message == WM_LBUTTONUP);
				if (!rectOld.EqualRect(&m_rect) || m_bFinalErase)
				{
					if (bMoved)
					{
						m_bErase = TRUE;
						DrawTrackerRect(&rectOld, pWindowClip, &drawDC, pWindow);
					}
					OnChangedRect(rectOld);
					if (msg.message != WM_LBUTTONUP)
						bMoved = TRUE;
				}
				if (m_bFinalErase)
					goto ExitLoop;

				if (!rectOld.EqualRect(&m_rect))
				{
					m_bErase = FALSE;
					DrawTrackerRect(&m_rect, pWindowClip, &drawDC, pWindow);
				}
				break;
			case WM_KEYDOWN:
				if (msg.wParam != VK_ESCAPE)
					break;
			case WM_RBUTTONDOWN:
				if (bMoved)
				{
					m_bErase = m_bFinalErase = TRUE;
					DrawTrackerRect(&m_rect, pWindowClip, &drawDC, pWindow);
				}
				m_rect = rectSave;
				goto ExitLoop;
			default:
				DispatchMessage(&msg);
				break;
			}
		}
	ExitLoop:
		if (pWindowClip != NULL)
		{
			::ReleaseDC(pWindowClip->Handle(), drawDC.Detach());
		}
		else
		{
			::ReleaseDC(pWindow->Handle(), drawDC.Detach());
		}
		ReleaseCapture();

		if (!bMoved)
		{
			m_rect = rectSave;
		}
		m_bFinalErase = FALSE;
		m_bErase = FALSE;
		if (!rectSave.EqualRect(&m_rect))
		{
			MoveHandle(&m_rect, &rectSave);
			return TRUE;
		}
		return FALSE;
	}

	void TinyRectTracker::GetModifyPointers(int nHandle, int** ppx, int** ppy, int* px, int* py)
	{
		ASSERT(nHandle >= 0);
		ASSERT(nHandle <= 8);

		if (nHandle == hitMiddle)
		{
			nHandle = hitTopLeft;
		}
		*ppx = NULL;
		*ppy = NULL;
		const HANDLEINFO* pHandleInfo = &g_HandleInfo[nHandle];
		if (pHandleInfo->nInvertX != nHandle)
		{
			*ppx = (int*)((BYTE*)&m_rect + pHandleInfo->nOffsetX);
			if (px != NULL)
			{
				*px = **ppx;
			}
		}
		else
		{
			if (px != NULL)
			{
				*px = m_rect.left + abs(m_rect.Width()) / 2;
			}
		}
		if (pHandleInfo->nInvertY != nHandle)
		{
			*ppy = (int*)((BYTE*)&m_rect + pHandleInfo->nOffsetY);
			if (py != NULL)
			{
				*py = **ppy;
			}
		}
		else
		{
			if (py != NULL)
			{
				*py = m_rect.top + abs(m_rect.Height()) / 2;
			}
		}
	}

	UINT TinyRectTracker::GetHandleMask()
	{
		UINT mask = 0x0F;
		int size = m_nHandleSize * 3;
		if (abs(m_rect.Width()) - size > 4)
		{
			mask |= 0x50;
		}
		if (abs(m_rect.Height()) - size > 4)
		{
			mask |= 0xA0;
		}
		return mask;
	}
}