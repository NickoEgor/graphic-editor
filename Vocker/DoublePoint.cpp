#include "DoublePoint.h"
#include "stdafx.h"

DoublePoint::DoublePoint() : x(0.0), y(0.0) {}

DoublePoint::DoublePoint(CPoint point) : x(point.x), y(point.y) {}

DoublePoint::DoublePoint(double x, double y) : x(x), y(y) {}

DoublePoint::operator CPoint() const {
  return CPoint(static_cast<int>(round(x)), static_cast<int>(round(y)));
}

DoublePoint::operator CSize() const {
  return CSize(static_cast<int>(round(x)), static_cast<int>(round(y)));
}

DoublePoint DoublePoint::operator-() { return {-x, -y}; }

DoublePoint &DoublePoint::operator+=(const CPoint &point) {
  x += point.x;
  y += point.y;
  return *this;
}

DoublePoint &DoublePoint::operator-=(const CPoint &point) {
  x -= point.x;
  y -= point.y;
  return *this;
}

DoublePoint DoublePoint::operator+(const DoublePoint &point) const {
  return {x + point.x, y + point.y};
}

DoublePoint DoublePoint::operator-(const DoublePoint &point) const {
  return {x - point.x, y - point.y};
}

DoublePoint DoublePoint::operator+(const CPoint &point) const {
  return {x + point.x, y + point.y};
}

DoublePoint DoublePoint::operator-(const CPoint &point) const {
  return {x - point.x, y - point.y};
}

bool DoublePoint::operator==(const DoublePoint &point) const {
  return this->x == point.x && this->y == point.y;
}

DoublePoint DoublePoint::operator/(double divider) const {
  return {x / divider, y / divider};
}

CArchive &operator>>(CArchive &ar, DoublePoint &point) {
  ar >> point.x >> point.y;
  return ar;
}

CArchive &operator<<(CArchive &ar, const DoublePoint &point) {
  ar << point.x << point.y;
  return ar;
}
