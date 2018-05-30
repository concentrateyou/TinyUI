#include "TinyRectTracker.h"
#include "../Common/TinyApplication.h"

namespace TinyFramework
{
	struct HANDLEINFO
	{
		size_t nOffsetX;
		size_t nOffsetY;
		INT nCenterX;
		INT nCenterY;
		INT nHandleX;
		INT nHandleY;
		INT nInvertX;
		INT nInvertY;
	};

	STATIC_DATA const HANDLEINFO g_handleInfos[] =
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
		INT nSignAcross;
	};

	STATIC_DATA const RECTINFO g_rectInfos[] =
	{
		{ offsetof(RECT, right), +1 },
		{ offsetof(RECT, bottom), +1 },
		{ offsetof(RECT, left), -1 },
		{ offsetof(RECT, top), -1 },
	};

	STATIC_DATA HCURSOR g_cursors[10] = { 0, };

	TinyRectTracker::TinyRectTracker()
	{
		Construct();
	}
	TinyRectTracker::~TinyRectTracker()
	{
		m_bErase = FALSE;
		m_bFinalErase = FALSE;
		m_trackerRect.SetRectEmpty();
	}
	void TinyRectTracker::Construct()
	{
		static BOOL bInitialized;
		if (!bInitialized)
		{
			ASSERT(sizeof(((RECT*)NULL)->left) == sizeof(INT));
			ASSERT(offsetof(RECT, top) > offsetof(RECT, left));
			ASSERT(offsetof(RECT, right) > offsetof(RECT, top));
			ASSERT(offsetof(RECT, bottom) > offsetof(RECT, right));

			g_cursors[0] = ::LoadCursor(NULL, IDC_SIZENWSE);
			g_cursors[1] = ::LoadCursor(NULL, IDC_SIZENESW);
			g_cursors[2] = g_cursors[0];
			g_cursors[3] = g_cursors[1];
			g_cursors[4] = ::LoadCursor(NULL, IDC_SIZENS);
			g_cursors[5] = ::LoadCursor(NULL, IDC_SIZEWE);
			g_cursors[6] = g_cursors[4];
			g_cursors[7] = g_cursors[5];
			g_cursors[8] = ::LoadCursor(NULL, IDC_SIZEALL);
			g_cursors[9] = ::LoadCursor(NULL, IDC_SIZEALL);
			bInitialized = TRUE;
		}
		m_handleSize = 6;
		m_sizeMin.cy = m_sizeMin.cx = m_handleSize * 2;
		m_bErase = FALSE;
		m_bFinalErase = FALSE;
		m_trackerRect.SetRectEmpty();
		m_pen.CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
		m_brush.CreateBrush(RGB(100, 200, 255));
	}
	void TinyRectTracker::GetTrueRect(LPRECT lpTrueRect) const
	{
		TinyRectangle rect = m_trackerRect;
		rect.NormalizeRect();
		INT nInflateBy = 1;
		rect.InflateRect(nInflateBy, nInflateBy);
		*lpTrueRect = rect;
	}
	BOOL TinyRectTracker::SetCursor(HWND hWND, UINT nHitTest) const
	{
		if (nHitTest != HTCLIENT)
			return FALSE;
		TinyPoint point;
		GetCursorPos(&point);
		::ScreenToClient(hWND, &point);
		INT nHandle = HitTestHandles(point);
		if (nHandle < 0)
			return FALSE;
		nHandle = NormalizeHit(nHandle);
		::SetCursor(g_cursors[nHandle]);
		return TRUE;
	}
	void TinyRectTracker::Draw(TinyDC* pDC) const
	{
		INT iSave = pDC->SaveDC();
		pDC->SetMapMode(MM_TEXT);
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetViewportOrg(0, 0);
		pDC->SetWindowOrg(0, 0);
		TinyRectangle rectangle = m_trackerRect;
		rectangle.NormalizeRect();
		HBRUSH hOldBrush = (HBRUSH)pDC->SelectObject(GetStockObject(NULL_BRUSH));
		HPEN hOldPen = (HPEN)pDC->SelectObject(m_pen);
		INT oldROP = pDC->SetROP2(R2_COPYPEN);
		rectangle.InflateRect(1, 1);
		pDC->Rectangle(rectangle.left, rectangle.top, rectangle.right, rectangle.bottom);
		pDC->SetROP2(oldROP);
		UINT mask = GetHandleMask();
		for (INT i = 0; i < 8; ++i)
		{
			if (mask & (1 << i))
			{
				GetHandleRect((TrackerHit)i, &rectangle);
				pDC->FillSolidRect(rectangle, RGB(100, 200, 255));
			}
		}
		if (hOldPen != NULL)
			pDC->SelectObject(hOldPen);
		if (hOldBrush != NULL)
			pDC->SelectObject(hOldBrush);
		pDC->RestoreDC(iSave);
	}
	BOOL TinyRectTracker::Track(HWND hWND, const TinyPoint& point, BOOL bAllowInvert)
	{
		INT nHandle = HitTestHandles(point);
		if (nHandle < 0)
		{
			return FALSE;
		}
		m_bAllowInvert = bAllowInvert;
		return TrackHandle(nHandle, hWND, point);
	}
	void TinyRectTracker::OnChangedRect(const TinyRectangle& rectOld)
	{

	}
	BOOL TinyRectTracker::TrackRubberBand(HWND hWND, const TinyPoint& point, BOOL bAllowInvert)
	{
		m_bAllowInvert = bAllowInvert;
		m_trackerRect.SetRect(point.x, point.y, point.x, point.y);
		return TrackHandle(hitBottomRight, hWND, point);
	}
	BOOL TinyRectTracker::TrackHandle(INT nHandle, HWND hWND, const TinyPoint& point)
	{
		ASSERT(nHandle >= 0);
		ASSERT(nHandle <= 8);
		if (::GetCapture() != NULL)
			return FALSE;
		ASSERT(!m_bFinalErase);

		INT nWidth = m_trackerRect.Width();
		INT nHeight = m_trackerRect.Height();

		::SetCapture(hWND);
		ASSERT(hWND == ::GetCapture());
		TinyRectangle rectSave = m_trackerRect;
		INT *px, *py;
		INT xDiff, yDiff;
		GetModifyPointers(nHandle, &px, &py, &xDiff, &yDiff);
		xDiff = point.x - xDiff;
		yDiff = point.y - yDiff;
		TinyRectangle rectOld;
		BOOL bMoved = FALSE;
		for (;;)
		{
			MSG msg;
			::GetMessage(&msg, NULL, 0, 0);
			if (::GetCapture() != hWND)
				break;
			switch (msg.message)
			{
			case WM_LBUTTONUP:
			case WM_MOUSEMOVE:
				rectOld = m_trackerRect;
				if (px != NULL)
					*px = GET_X_LPARAM(msg.lParam) - xDiff;
				if (py != NULL)
					*py = GET_Y_LPARAM(msg.lParam) - yDiff;
				if (nHandle == hitMiddle)
				{
					m_trackerRect.right = m_trackerRect.left + nWidth;
					m_trackerRect.bottom = m_trackerRect.top + nHeight;
				}
				AdjustRect(nHandle, &m_trackerRect);
				m_bFinalErase = (msg.message == WM_LBUTTONUP);
				if (!rectOld.EqualRect(&m_trackerRect) || m_bFinalErase)
				{
					if (bMoved)
					{
						m_bErase = TRUE;
					}
					OnChangedRect(rectOld);
					if (msg.message != WM_LBUTTONUP)
					{
						bMoved = TRUE;
					}
				}
				if (m_bFinalErase)
				{
					goto ExitLoop;
				}
				if (!rectOld.EqualRect(&m_trackerRect))
				{
					m_bErase = FALSE;
				}
				break;
			case WM_KEYDOWN:
				if (msg.wParam != VK_ESCAPE)
					break;
			case WM_RBUTTONDOWN:
				if (bMoved)
				{
					m_bErase = m_bFinalErase = TRUE;
				}
				m_trackerRect = rectSave;
				goto ExitLoop;
			default:
				DispatchMessage(&msg);
				break;
			}
		}
	ExitLoop:
		ReleaseCapture();
		if (!bMoved)
		{
			m_trackerRect = rectSave;
		}
		m_bFinalErase = FALSE;
		m_bErase = FALSE;
		return !rectSave.EqualRect(&m_trackerRect);
	}

	INT TinyRectTracker::NormalizeHit(INT nHandle) const
	{
		if (nHandle == hitMiddle || nHandle == hitNothing)
			return nHandle;
		const HANDLEINFO* pHandleInfo = &g_handleInfos[nHandle];
		if (m_trackerRect.Width() < 0)
		{
			nHandle = (TrackerHit)pHandleInfo->nInvertX;
			pHandleInfo = &g_handleInfos[nHandle];
		}
		if (m_trackerRect.Height() < 0)
		{
			nHandle = (TrackerHit)pHandleInfo->nInvertY;
		}
		return nHandle;
	}
	INT TinyRectTracker::HitTest(const TinyPoint& point) const
	{
		TrackerHit hitResult = hitNothing;
		TinyRectangle rectTrue;
		GetTrueRect(&rectTrue);
		ASSERT(rectTrue.left <= rectTrue.right);
		ASSERT(rectTrue.top <= rectTrue.bottom);
		if (rectTrue.PtInRect(point))
		{
			hitResult = (TrackerHit)HitTestHandles(point);
		}
		return hitResult;
	}
	INT TinyRectTracker::HitTestHandles(const TinyPoint& point) const
	{
		TinyRectangle rect;
		UINT mask = GetHandleMask();
		GetTrueRect(&rect);
		if (!rect.PtInRect(point))
		{
			return hitNothing;
		}
		for (INT i = 0; i < 8; ++i)
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
		rect = m_trackerRect;
		rect.NormalizeRect();
		rect.InflateRect(+1, +1);
		if (!rect.PtInRect(point))
		{
			return hitNothing;
		}
		return hitMiddle;
	}
	UINT TinyRectTracker::GetHandleMask() const
	{
		UINT mask = 0x0F;
		INT size = m_handleSize * 3;
		if (abs(m_trackerRect.Width()) - size > 4)
			mask |= 0x50;
		if (abs(m_trackerRect.Height()) - size > 4)
			mask |= 0xA0;
		return mask;
	}
	void TinyRectTracker::SetEmpty()
	{
		m_trackerRect.SetRectEmpty();
	}
	void TinyRectTracker::GetHandleRect(INT nHandle, TinyRectangle* pHandleRect) const
	{
		TinyRectangle rectT = m_trackerRect;
		rectT.NormalizeRect();
		rectT.InflateRect(1, 1);
		nHandle = NormalizeHit(nHandle);
		INT size = GetHandleSize();
		INT nWidth = rectT.Width();
		INT nHeight = rectT.Height();
		TinyRectangle rect;
		const HANDLEINFO* pHandleInfo = &g_handleInfos[nHandle];
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
	void TinyRectTracker::AdjustRect(INT nHandle, LPRECT)
	{
		if (nHandle == hitMiddle)
			return;
		INT *px, *py;
		GetModifyPointers(nHandle, &px, &py, NULL, NULL);
		INT nNewWidth = m_trackerRect.Width();
		INT nAbsWidth = m_bAllowInvert ? abs(nNewWidth) : nNewWidth;
		if (px != NULL && nAbsWidth < m_sizeMin.cx)
		{
			nNewWidth = nAbsWidth != 0 ? nNewWidth / nAbsWidth : 1;
			ptrdiff_t iRectInfo = (INT*)px - (INT*)&m_trackerRect;
			const RECTINFO* pRectInfo = &g_rectInfos[iRectInfo];
			*px = *(INT*)((BYTE*)&m_trackerRect + pRectInfo->nOffsetAcross) + nNewWidth * m_sizeMin.cx * -pRectInfo->nSignAcross;
		}
		INT nNewHeight = m_trackerRect.Height();
		INT nAbsHeight = m_bAllowInvert ? abs(nNewHeight) : nNewHeight;
		if (py != NULL && nAbsHeight < m_sizeMin.cy)
		{
			nNewHeight = nAbsHeight != 0 ? nNewHeight / nAbsHeight : 1;
			ptrdiff_t iRectInfo = (INT*)py - (INT*)&m_trackerRect;
			const RECTINFO* pRectInfo = &g_rectInfos[iRectInfo];
			*py = *(INT*)((BYTE*)&m_trackerRect + pRectInfo->nOffsetAcross) + nNewHeight * m_sizeMin.cy * -pRectInfo->nSignAcross;
		}
	}

	void TinyRectTracker::GetModifyPointers(INT nHandle, INT**ppx, INT**ppy, INT* px, INT*py)
	{
		if (nHandle == hitMiddle)
			nHandle = hitTopLeft;
		*ppx = NULL;
		*ppy = NULL;
		const HANDLEINFO* pHandleInfo = &g_handleInfos[nHandle];
		if (pHandleInfo->nInvertX != nHandle)
		{
			*ppx = (INT*)((BYTE*)&m_trackerRect + pHandleInfo->nOffsetX);
			if (px != NULL)
				*px = **ppx;
		}
		else
		{
			if (px != NULL)
				*px = m_trackerRect.left + abs(m_trackerRect.Width()) / 2;
		}
		if (pHandleInfo->nInvertY != nHandle)
		{
			*ppy = (int*)((BYTE*)&m_trackerRect + pHandleInfo->nOffsetY);
			if (py != NULL)
				*py = **ppy;
		}
		else
		{
			if (py != NULL)
				*py = m_trackerRect.top + abs(m_trackerRect.Height()) / 2;
		}
	}
	INT TinyRectTracker::GetHandleSize() const
	{
		return m_handleSize;
	}
}