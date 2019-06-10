#pragma once

struct DoublePoint {
  double x, y;

  DoublePoint();
  DoublePoint(CPoint point);
  DoublePoint(double x, double y);

  operator CPoint() const;
  explicit operator CSize() const;

  DoublePoint operator-();

  DoublePoint &operator+=(const CPoint &point);
  DoublePoint &operator-=(const CPoint &point);

  DoublePoint operator+(const DoublePoint &point) const;
  DoublePoint operator-(const DoublePoint &point) const;

  DoublePoint operator+(const CPoint &point) const;
  DoublePoint operator-(const CPoint &point) const;

  bool operator==(const DoublePoint &point) const;

  DoublePoint operator/(double divider) const;
};

CArchive &operator>>(CArchive &ar, DoublePoint &point);
CArchive &operator<<(CArchive &ar, const DoublePoint &point);
