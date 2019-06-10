#pragma once

#include "DoublePoint.h"

struct GeometryLine {
  GeometryLine(DoublePoint first, DoublePoint second);
  static double Det(double a, double b, double c, double d);
  static bool Intersect(GeometryLine m, GeometryLine n, DoublePoint &result);

  double a, b, c;
  static const double EPS;
};
