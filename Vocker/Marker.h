#pragma once

#include "stdafx.h"

class Marker {
public:
  Marker(CPoint point);
  Marker(const Marker &other);

  void Draw(CDC *dc);
  bool IsPointInMarker(CPoint point);

  void static Draw(CDC *dc, CPoint point);

private:
  static CPen Pen;
  static const int MarkerRadius = 6;

  CPoint Point;
  CRgn Region;
};
