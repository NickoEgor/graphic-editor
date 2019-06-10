#include "Marker.h"
#include "Utilities.h"
#include "stdafx.h"

CPen Marker::Pen(PS_SOLID, 2, RGB(0, 0, 255));

Marker::Marker(CPoint point) : Point(point) {}

Marker::Marker(const Marker &other) : Point(other.Point) {}

void Marker::Draw(CDC *dc, CPoint point) {
  CPen *oldPen = dc->SelectObject(&Pen);

  CRect markerRect = ExtendPointToRect(point, MarkerRadius);

  dc->MoveTo(markerRect.left, markerRect.top),
      dc->LineTo(markerRect.right, markerRect.bottom);
  dc->MoveTo(markerRect.left, markerRect.bottom);
  dc->LineTo(markerRect.right, markerRect.top);

  dc->SelectObject(oldPen);
}

void Marker::Draw(CDC *dc) {
  Draw(dc, Point);

  CRect markerRect = ExtendPointToRect(Point, MarkerRadius);
  Region.DeleteObject();
  Region.CreateRectRgnIndirect(markerRect);
}

bool Marker::IsPointInMarker(CPoint point) { return Region.PtInRegion(point); }
