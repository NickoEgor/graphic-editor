#include "RTriangle.h"
#include "GeometryLine.h"
#include "stdafx.h"

#include <algorithm>

IMPLEMENT_SERIAL(RTriangle, CObject, 1)

RTriangle::RTriangle(CRect rect) {
  const int size = 3;
  Points.resize(3);
  CPoint points[size];
  CalculateTrianglePoints(points, rect);

  for (size_t i = 0; i < size; ++i) {
    Points[i] = points[i];
  }

  SetUniqueName(_T("Triangle"));
}

RTriangle::RTriangle() {}

void RTriangle::Draw(CDC *dc, CPoint scrollPosition) const {
  CPen *oldPen = dc->SelectObject(&Pen);
  CBrush *oldBrush = dc->SelectObject(&Brush);

  const int size = 3;
  auto cornerPoints = GetCornerPoints();
  CPoint corners[size];
  for (size_t i = 0; i < size; ++i) {
    corners[i] = cornerPoints[i];
  }

  Region.DeleteObject();
  dc->Polygon(corners, size);
  Region.CreatePolygonRgn(corners, size, WINDING);
  Region.OffsetRgn(-scrollPosition);

  dc->SelectObject(oldPen);
  dc->SelectObject(oldBrush);
}

void RTriangle::Move(CSize shiftSize) {
  for (auto &point : Points) {
    point += shiftSize;
  }
}

void RTriangle::Normalize() {
  DoublePoint center = GetCenter();

  size_t size = Points.size();
  std::vector<DoublePoint> diffs(size);
  std::vector<double> sides(size);
  std::vector<double> angles(size);
  for (size_t i = 0; i < size; ++i) {
    diffs[i] = Points[i] - center;
    sides[i] = CountLength(diffs[i]);
  }

  auto minIter = std::min_element(sides.begin(), sides.end());
  double minSide = *minIter;
  int index = minIter - sides.begin();

  angles[index] = acos(diffs[index].y / sides[index]);
  if (diffs[index].x >= 0)
    angles[index] *= -1;

  int multiplier = 1;
  for (size_t i = 0; i < size; ++i) {
    if (i != index) {
      angles[i] = angles[index] + multiplier * 4 * M_PI / 3;
      multiplier *= -1;
    }

    Points[i].x = minSide * -sin(angles[i]) + center.x;
    Points[i].y = minSide * cos(angles[i]) + center.y;
  }
}

DoublePoint RTriangle::GetCenter() const {
  DoublePoint sum = {0, 0};
  for (auto &point : Points) {
    sum += point;
  }
  return sum / 3.0;
}

std::vector<DoublePoint> RTriangle::GetCornerPoints() const { return Points; }

DoublePoint RTriangle::GetBorderPoint(double angle) const {
  auto corners = GetCornerPoints();
  auto center = GetCenter();

  std::vector<DoublePoint> options;
  for (size_t i = 0; i < corners.size(); ++i) {
    DoublePoint p1 = corners[i], p2;
    if (i != corners.size() - 1) {
      p2 = corners[i + 1];
    } else {
      p2 = corners[0];
    }

    GeometryLine l1(p1, p2);

    DoublePoint rayPoint{center.x + sin(angle), center.y - cos(angle)};

    GeometryLine l2(center, rayPoint);

    DoublePoint result;
    if (GeometryLine::Intersect(l1, l2, result)) {
      if (result.x >= min(p1.x - EPSILON, p2.x - EPSILON) &&
          result.x <= max(p1.x + EPSILON, p2.x + EPSILON) &&
          result.y >= min(p1.y - EPSILON, p2.y - EPSILON) &&
          result.y <= max(p1.y + EPSILON, p2.y + EPSILON)) {
        options.push_back(result);
      }
    }
  }

  for (auto &option : options) {
    DoublePoint line = option - center;
    double optionAngle = acos(-line.y / CountLength(line));
    optionAngle *= (line.x < 0 ? -1 : 1);

    if (abs(angle - optionAngle) < EPSILON) {
      return option;
    }
  }

  return RShape::GetBorderPoint(angle);
}

std::vector<CPoint> RTriangle::GetResizingMarkers() const {
  std::vector<CPoint> points;
  for (auto &point : Points) {
    points.push_back(point);
  }
  return points;
}

void RTriangle::Resize(int markerNumber, CPoint shift) {
  Points[markerNumber] += shift;
}

void RTriangle::Rotate(double radians) {
  RShape::Rotate(radians);

  DoublePoint center = GetCenter();
  for (auto &point : Points) {
    point = GetRotatedPoint(point, radians, center);
  }
}

void RTriangle::SetProperty(PropName name, CString value) {
  switch (name) {
  case PropName::ANGLE: {
    double angle = CStringToDouble(value);
    DoublePoint center = GetCenter();
    for (auto &point : Points) {
      point = GetRotatedPoint(point, -Angle, center);
      point = GetRotatedPoint(point, angle, center);
    }
    Angle = angle;
    break;
  }
  default:
    RShape::SetProperty(name, value);
    break;
  }
}

void RTriangle::Serialize(CArchive &ar) {
  RShape::Serialize(ar);

  size_t pointsSize;
  if (ar.IsStoring()) {
    pointsSize = Points.size();
    ar << pointsSize << Points;
  } else {
    ar >> pointsSize;
    Points.resize(pointsSize);
    ar >> Points;
  }
}

CArchive &operator>>(CArchive &ar, std::vector<DoublePoint> &vec) {
  for (size_t i = 0; i < vec.size(); ++i) {
    ar >> vec[i];
  }

  return ar;
}

CArchive &operator<<(CArchive &ar, const std::vector<DoublePoint> &vec) {
  for (auto &item : vec) {
    ar << item;
  }

  return ar;
}
