#include "GeometryLine.h"
#include "stdafx.h"

const double GeometryLine::EPS = 1e-9;

GeometryLine::GeometryLine(DoublePoint first, DoublePoint second) {
  a = second.y - first.y;
  b = first.x - second.x;
  c = first.y * (second.x - first.x) - first.x * (second.y - first.y);
}

double GeometryLine::Det(double a, double b, double c, double d) {
  return a * d - b * c;
}

bool GeometryLine::Intersect(GeometryLine m, GeometryLine n,
                             DoublePoint &result) {
  double sign = Det(m.a, m.b, n.a, n.b);
  if (abs(sign) < EPS)
    return false;
  result.x = -Det(m.c, m.b, n.c, n.b) / sign;
  result.y = -Det(m.a, m.c, n.a, n.c) / sign;
  return true;
}
