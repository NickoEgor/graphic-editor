#include "RRectangle.h"
#include "GeometryLine.h"
#include "stdafx.h"

IMPLEMENT_SERIAL(RRectangle, CObject, 1)

RRectangle::RRectangle(CPoint firstCorner, CPoint secondCorner)
    : TopLeft(firstCorner), BottomRight(secondCorner) {
  SetUniqueName(_T("Rectangle"));
}

RRectangle::RRectangle(CRect rect)
    : RRectangle::RRectangle(rect.TopLeft(), rect.BottomRight()) {}

RRectangle::RRectangle() {}

void RRectangle::Draw(CDC *dc, CPoint scrollPosition) const {
  CPen *oldPen = dc->SelectObject(&Pen);
  CBrush *oldBrush = dc->SelectObject(&Brush);

  const int size = 4;
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

std::vector<DoublePoint> RRectangle::GetCornerPoints() const {
  DoublePoint center = GetCenter();
  return {GetRotatedPoint(TopLeft, Angle, center),
          GetRotatedPoint({BottomRight.x, TopLeft.y}, Angle, center),
          GetRotatedPoint(BottomRight, Angle, center),
          GetRotatedPoint({TopLeft.x, BottomRight.y}, Angle, center)};
}

void RRectangle::Move(CSize shiftSize) {
  TopLeft += shiftSize;
  BottomRight += shiftSize;
}

void RRectangle::Normalize() {
  DoublePoint diff = BottomRight - TopLeft;
  double halfSide = min(abs(diff.x), abs(diff.y)) / 2;
  DoublePoint center = GetCenter();
  TopLeft = center - DoublePoint{halfSide, halfSide};
  BottomRight = center + DoublePoint{halfSide, halfSide};
}

DoublePoint RRectangle::GetCenter() const {
  return {(BottomRight.x + TopLeft.x) / 2, (BottomRight.y + TopLeft.y) / 2};
}

DoublePoint RRectangle::GetBorderPoint(double angle) const {
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

std::vector<CPoint> RRectangle::GetResizingMarkers() const {
  DoublePoint center = GetCenter();
  return std::vector<CPoint>{
      GetRotatedPoint({(TopLeft.x + BottomRight.x) / 2, TopLeft.y}, Angle,
                      center), // top
      GetRotatedPoint({BottomRight.x, (TopLeft.y + BottomRight.y) / 2}, Angle,
                      center), // right
      GetRotatedPoint({(TopLeft.x + BottomRight.x) / 2, BottomRight.y}, Angle,
                      center), // bottom
      GetRotatedPoint({TopLeft.x, (TopLeft.y + BottomRight.y) / 2}, Angle,
                      center) // left
  };
}

void RRectangle::Resize(int markerNumber, CPoint shift) {
  double width = BottomRight.x - TopLeft.x;
  double height = BottomRight.y - TopLeft.y;
  double len = CountLength(shift);

  DoublePoint center = GetCenter();

  MarkerSide side = static_cast<MarkerSide>(markerNumber);

  switch (side) {
  case MarkerSide::TOP: {
    double phi = acos(-shift.y / len);
    if (shift.x < 0) {
      len *= cos(phi + Angle);
    } else {
      len *= cos(phi - Angle);
    }

    height += len;
    center.y -= len * cos(Angle) / 2;
    center.x += len * sin(Angle) / 2;
    break;
  }
  case MarkerSide::BOTTOM: {
    double phi = acos(shift.y / len);
    if (shift.x < 0) {
      len *= cos(phi - Angle);
    } else {
      len *= cos(phi + Angle);
    }

    height += len;
    center.y += len * cos(Angle) / 2;
    center.x -= len * sin(Angle) / 2;
    break;
  }
  case MarkerSide::RIGHT: {
    double phi = acos(-shift.y / len);
    if (shift.x < 0) {
      len *= -sin(phi + Angle);
    } else {
      len *= sin(phi - Angle);
    }

    width += len;
    center.y -= len * cos(Angle + M_PI_2) / 2;
    center.x += len * sin(Angle + M_PI_2) / 2;
    break;
  }
  case MarkerSide::LEFT: {
    double phi = acos(shift.y / len);
    if (shift.x < 0) {
      len *= sin(phi - Angle);
    } else {
      len *= -sin(phi + Angle);
    }

    width += len;
    center.y += len * cos(Angle + M_PI_2) / 2;
    center.x -= len * sin(Angle + M_PI_2) / 2;
    break;
  }
  }

  TopLeft.x = center.x - width / 2;
  TopLeft.y = center.y - height / 2;
  BottomRight.x = center.x + width / 2;
  BottomRight.y = center.y + height / 2;
}

std::vector<DrawableProperty> RRectangle::GetProperties() const {
  auto properties = RShape::GetProperties();

  CString tempString;
  CPoint tempPoint;

  tempPoint = BottomRight - TopLeft;
  tempPoint.x = abs(tempPoint.x);
  tempPoint.y = abs(tempPoint.y);

  tempString.Format(_T("%d"), tempPoint.x);
  properties.push_back(
      DrawableProperty(PropName::RECTHORZ, tempString, PropType::DOUBLE));
  tempString.Format(_T("%d"), tempPoint.y);
  properties.push_back(
      DrawableProperty(PropName::RECTVERT, tempString, PropType::DOUBLE));

  return properties;
}

void RRectangle::SetProperty(PropName name, CString value) {
  switch (name) {
  case PropName::RECTHORZ: {
    double size = CStringToDouble(value);
    auto center = GetCenter();
    TopLeft.x = center.x - size / 2;
    BottomRight.x = center.x + size / 2;
    break;
  }
  case PropName::RECTVERT: {
    double size = CStringToDouble(value);
    auto center = GetCenter();
    TopLeft.y = center.y - size / 2;
    BottomRight.y = center.y + size / 2;
    break;
  }
  default:
    RShape::SetProperty(name, value);
    break;
  }
}

void RRectangle::Serialize(CArchive &ar) {
  RShape::Serialize(ar);

  if (ar.IsStoring())
    ar << TopLeft << BottomRight;
  else
    ar >> TopLeft >> BottomRight;
}
