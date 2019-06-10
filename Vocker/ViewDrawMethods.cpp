#include "stdafx.h"
#include <algorithm>
#include <utility>

#include "Vocker.h"
#include "VockerView.h"

CPen CVockerView::BorderPen(PS_SOLID, 1, RGB(192, 192, 192));

void CVockerView::OnDraw(CDC *pDC) {
  CVockerDoc *pDoc = GetDocument();
  if (!pDoc)
    return;

  CRect clipBox;
  pDC->GetClipBox(clipBox);

  CDC memDC;
  memDC.CreateCompatibleDC(pDC);
  memDC.SetMapMode(pDC->GetMapMode());
  memDC.SetViewportOrg(pDC->GetViewportOrg());
  memDC.IntersectClipRect(clipBox);

  CBitmap bmp;
  bmp.CreateCompatibleBitmap(pDC, clipBox.Width(), clipBox.Height());
  CBitmap *pOldBmp = memDC.SelectObject(&bmp);

  memDC.FillSolidRect(clipBox, pDC->GetBkColor());

  if (IsShowingBorder) {
    DrawBorder(&memDC, pDoc);
  }

  DrawDrawables(&memDC, pDoc);
  DrawTempDrawable(&memDC, pDoc);

  pDC->BitBlt(clipBox.left, clipBox.top, clipBox.Width(), clipBox.Height(),
              &memDC, clipBox.left, clipBox.top, SRCCOPY);

  memDC.SelectObject(pOldBmp);
}

void CVockerView::DrawDrawables(CDC *pDC, CVockerDoc *pDoc) {
  for (auto &drawable : pDoc->GetOrderedDrawables()) {
    drawable->Draw(pDC, GetScrollPosition());
  }

  if (CurrentShape != nullptr) {
    switch (GetCurrentTool()) {
    case PaintTool::RESIZESHAPE: {
      auto markerPoints = CurrentShape->GetResizingMarkers();
      TempMarkers.clear();
      TempMarkers.reserve(markerPoints.size());
      for (auto &markerPoint : markerPoints) {
        TempMarkers.push_back(Marker(markerPoint));
        (TempMarkers.end() - 1)->Draw(pDC);
      }
      break;
    }
    default:
      Marker::Draw(pDC, CurrentShape->GetCenter());
      break;
    }
  }
}

void CVockerView::DrawTempDrawable(CDC *pDC, CVockerDoc *pDoc) {
  CPen *oldPen = pDC->SelectObject(&(pDoc->CurrentPen));
  CBrush *oldBrush = pDC->SelectObject(&(pDoc->CurrentBrush));

  auto currentTool = GetCurrentTool();

  switch (CurrentMode) {
  case Mode::SHAPE:
    switch (currentTool) {
    case PaintTool::RECTANGLE:
      pDC->Rectangle(TempRect);
      break;
    case PaintTool::ELLIPSE:
      pDC->Ellipse(TempRect);
      break;
    case PaintTool::TRIANGLE: {
      const int size = 3;
      CPoint points[size];
      CalculateTrianglePoints(points, TempRect);
      pDC->Polygon(points, size);
      break;
    }
    }
    break;
  case Mode::LINE:
    switch (currentTool) {
    case PaintTool::LINE:
      RLine::Draw(pDC, TempRect.TopLeft(), false, TempRect.BottomRight(), false,
                  &pDoc->CurrentPen);
      break;
    case PaintTool::ONEWAYLINE:
      RLine::Draw(pDC, TempRect.TopLeft(), false, TempRect.BottomRight(), true,
                  &pDoc->CurrentPen);
      break;
    case PaintTool::TWOWAYLINE:
      RLine::Draw(pDC, TempRect.TopLeft(), true, TempRect.BottomRight(), true,
                  &pDoc->CurrentPen);
      break;
    }
    break;
  }

  pDC->SelectObject(oldPen);
  pDC->SelectObject(oldBrush);
}

void CVockerView::DrawBorder(CDC *pDC, CVockerDoc *pDoc) {
  CPen *oldPen = pDC->SelectObject(&BorderPen);
  pDC->Rectangle(0, 0, pDoc->ViewSize.x, pDoc->ViewSize.y);
  pDC->SelectObject(oldPen);
}

BOOL CVockerView::OnEraseBkgnd(CDC *pDC) { return TRUE; }

void CVockerView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar) {
  CScrollView::OnHScroll(nSBCode, nPos, pScrollBar);
  Invalidate();
  UpdateWindow();
}

void CVockerView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar) {
  CScrollView::OnVScroll(nSBCode, nPos, pScrollBar);
  Invalidate();
  UpdateWindow();
}

BOOL CVockerView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) {
  BOOL result = CScrollView::OnMouseWheel(nFlags, zDelta, pt);
  Invalidate();
  UpdateWindow();
  return result;
}
