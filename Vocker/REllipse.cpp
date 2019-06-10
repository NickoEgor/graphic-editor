#include "REllipse.h"
#include "stdafx.h"

IMPLEMENT_SERIAL(REllipse, CObject, 1)

REllipse::REllipse(CPoint firstCorner, CPoint secondCorner) {
  int x1 = firstCorner.x, y1 = firstCorner.y, x2 = secondCorner.x,
      y2 = secondCorner.y;
  majorAxis = abs((x1 - x2) / 2.0);
  minorAxis = abs((y1 - y2) / 2.0);
  center = {min(x1, x2) + majorAxis, min(y1, y2) + minorAxis};

  SetUniqueName(_T("Ellipse"));
}

REllipse::REllipse(CRect rect)
    : REllipse::REllipse(rect.TopLeft(), rect.BottomRight()) {}

REllipse::REllipse() {}

void REllipse::Draw(CDC *dc, CPoint scrollPosition) const {
  CPen *oldPen = dc->SelectObject(&Pen);
  CBrush *oldBrush = dc->SelectObject(&Brush);

  CRect rect = GetRect();
  HDC hdc = *dc;
  XFORM xForm = SetWorldRotation(hdc, Angle, center);

  Region.DeleteObject();

  CRgn tempRegion;
  tempRegion.CreateEllipticRgnIndirect(rect - center);

  int nBytes = tempRegion.GetRegionData(0, NULL);
  RGNDATA *lpRgnData = new RGNDATA[nBytes];
  tempRegion.GetRegionData(lpRgnData, nBytes);
  Region.CreateFromData(&xForm, nBytes, lpRgnData);
  delete[] lpRgnData;

  dc->Ellipse(rect - center);
  SetWorldRotation(hdc);
  Region.OffsetRgn(-scrollPosition);

  dc->SelectObject(oldPen);
  dc->SelectObject(oldBrush);
}

CRect REllipse::GetRect() const {
  return CRect(DoublePoint(center.x - majorAxis, center.y - minorAxis),
               DoublePoint(center.x + majorAxis, center.y + minorAxis));
}

void REllipse::Move(CSize shiftSize) { center += shiftSize; }

void REllipse::Normalize() {
  majorAxis = min(majorAxis, minorAxis);
  minorAxis = majorAxis;
}

DoublePoint REllipse::GetCenter() const { return center; }

std::vector<DoublePoint> REllipse::GetCornerPoints() const {
  DoublePoint center = GetCenter();
  return {GetRotatedPoint({center.x - majorAxis, center.y - minorAxis}, Angle,
                          center),
          GetRotatedPoint({center.x + majorAxis, center.y + minorAxis}, Angle,
                          center)};
}

DoublePoint REllipse::GetBorderPoint(double angle) const {
  auto center = GetCenter();
  DoublePoint ray{sin(angle), -cos(angle)};
  ray = GetRotatedPoint(ray, -Angle);
  double k = ray.y / ray.x;

  double x = abs(minorAxis * majorAxis) /
             sqrt(minorAxis * minorAxis + k * k * majorAxis * majorAxis);

  double y = k * x;

  std::vector<DoublePoint> options;
  options.push_back({x, y});
  options.push_back({-x, -y});

  for (auto &option : options) {
    DoublePoint line = GetRotatedPoint(option, Angle);
    double optionAngle = acos(-line.y / CountLength(line));
    optionAngle *= (line.x < 0 ? -1 : 1);

    if (abs(angle - optionAngle) < EPSILON) {
      return center + line;
    }
  }

  return RShape::GetBorderPoint(angle);
}

std::vector<CPoint> REllipse::GetResizingMarkers() const {
  DoublePoint center = GetCenter();
  return std::vector<CPoint>{
      GetRotatedPoint({center.x, center.y - minorAxis}, Angle, center), // top
      GetRotatedPoint({center.x + majorAxis, center.y}, Angle, center), // right
      GetRotatedPoint({center.x, center.y + minorAxis}, Angle,
                      center),                                         // bottom
      GetRotatedPoint({center.x - majorAxis, center.y}, Angle, center) // left
  };
}

void REllipse::Resize(int markerNumber, CPoint shift) {
  double len = CountLength(shift);

  MarkerSide side = static_cast<MarkerSide>(markerNumber);

  switch (side) {
  case MarkerSide::TOP: {
    double phi = acos(-shift.y / len);
    if (shift.x < 0) {
      // len *= -cos(M_PI - Angle - phi);
      len *= cos(phi + Angle);
    } else {
      len *= cos(phi - Angle);
    }

    minorAxis += len / 2;
    center.y -= len * cos(Angle) / 2;
    center.x += len * sin(Angle) / 2;
    break;
  }
  case MarkerSide::BOTTOM: {
    double phi = acos(shift.y / len);
    if (shift.x < 0) {
      len *= cos(phi - Angle);
    } else {
      // len *= -cos(M_PI - Angle - phi);
      len *= cos(phi + Angle);
    }

    minorAxis += len / 2;
    center.y += len * cos(Angle) / 2;
    center.x -= len * sin(Angle) / 2;
    break;
  }
  case MarkerSide::RIGHT: {
    double phi = acos(-shift.y / len);
    if (shift.x < 0) {
      // len *= -cos(phi + Angle - M_PI_2);
      len *= -sin(phi + Angle);
    } else {
      // len *= cos(Angle + M_PI_2 - phi);
      len *= sin(phi - Angle);
    }

    majorAxis += len / 2;
    center.y -= len * cos(Angle + M_PI_2) / 2;
    center.x += len * sin(Angle + M_PI_2) / 2;
    break;
  }
  case MarkerSide::LEFT: {
    double phi = acos(shift.y / len);
    if (shift.x < 0) {
      // len *= cos(Angle + M_PI_2 - phi);
      len *= sin(phi - Angle);
    } else {
      // len *= -cos(phi + Angle - M_PI_2);
      len *= -sin(phi + Angle);
    }

    majorAxis += len / 2;
    center.y += len * cos(Angle + M_PI_2) / 2;
    center.x -= len * sin(Angle + M_PI_2) / 2;
    break;
  }
  }
}

std::vector<DrawableProperty> REllipse::GetProperties() const {
  auto properties = RShape::GetProperties();

  auto center = GetCenter();
  CString tempString;

  tempString.Format(_T("%f"), center.x);
  properties.push_back(
      DrawableProperty(PropName::ELLIPSECENTERX, tempString, PropType::DOUBLE));

  tempString.Format(_T("%f"), center.y);
  properties.push_back(
      DrawableProperty(PropName::ELLIPSECENTERY, tempString, PropType::DOUBLE));

  tempString.Format(_T("%f"), majorAxis);
  properties.push_back(
      DrawableProperty(PropName::ELLIPSEMAJOR, tempString, PropType::DOUBLE));

  tempString.Format(_T("%f"), minorAxis);
  properties.push_back(
      DrawableProperty(PropName::ELLIPSEMINOR, tempString, PropType::DOUBLE));

  return properties;
}

void REllipse::SetProperty(PropName name, CString value) {
  switch (name) {
  case PropName::ELLIPSECENTERX:
    center.x = CStringToDouble(value);
    break;
  case PropName::ELLIPSECENTERY:
    center.y = CStringToDouble(value);
    break;
  case PropName::ELLIPSEMAJOR:
    majorAxis = CStringToDouble(value);
    break;
  case PropName::ELLIPSEMINOR:
    minorAxis = CStringToDouble(value);
    break;
  default:
    RShape::SetProperty(name, value);
    break;
  }
}

void REllipse::Serialize(CArchive &ar) {
  RShape::Serialize(ar);

  if (ar.IsStoring()) {
    ar << center << majorAxis << minorAxis;
  } else {
    ar >> center >> majorAxis >> minorAxis;
  }
}
