#include "RLine.h"
#include "RShape.h"
#include "stdafx.h"
#include <algorithm>

IMPLEMENT_SERIAL(RLine, CObject, 1)

RLine::RLine(const std::shared_ptr<RShape> &firstShape,
             const std::shared_ptr<RShape> &secondShape)
    : FirstShape(firstShape), SecondShape(secondShape) {
  SetUniqueName(_T("Line"));
}

RLine::RLine() {}

int RLine::GetFirstShapeId() const { return FirstShape->GetId(); }

int RLine::GetSecondShapeId() const { return SecondShape->GetId(); }

void RLine::Draw(CDC *dc, CPoint scrollPosition) const {
  CPen *oldPen = dc->SelectObject(&Pen);

  DoublePoint firstCenter = FirstShape->GetCenter();
  DoublePoint secondCenter = SecondShape->GetCenter();

  DoublePoint line = secondCenter - firstCenter;
  double angle1 = acos(-line.y / CountLength(line));
  angle1 *= (line.x < 0 ? -1 : 1);
  double angle2 = acos(line.y / CountLength(line));
  angle2 *= (line.x < 0 ? 1 : -1);

  DoublePoint firstPoint = FirstShape->GetBorderPoint(angle1);
  DoublePoint secondPoint = SecondShape->GetBorderPoint(angle2);

  Draw(dc, firstPoint, IsFirstArrow, secondPoint, IsSecondArrow, &Pen);

  Region.DeleteObject();

  LOGPEN penInfo;
  Pen.GetLogPen(&penInfo);
  int width = penInfo.lopnWidth.x;
  CPoint shift = {max(width, 4), max(width, 4)};

  CPoint diff = secondPoint - firstPoint;
  CPoint points[4];

  if ((diff.x < 0 && diff.y < 0) || (diff.x >= 0 && diff.y >= 0)) {
    points[0] = DoublePoint(firstPoint.x + shift.x, firstPoint.y - shift.y);
    points[1] = DoublePoint(secondPoint.x + shift.x, secondPoint.y - shift.y);
    points[2] = DoublePoint(secondPoint.x - shift.x, secondPoint.y + shift.y);
    points[3] = DoublePoint(firstPoint.x - shift.x, firstPoint.y + shift.y);
  } else {
    points[0] = CPoint(firstPoint - shift);
    points[1] = CPoint(secondPoint - shift);
    points[2] = CPoint(secondPoint + shift);
    points[3] = CPoint(firstPoint + shift);
  }

  Region.CreatePolygonRgn(points, 4, WINDING);
  Region.OffsetRgn(-scrollPosition);

  dc->SelectObject(oldPen);
}

void RLine::Draw(CDC *dc, DoublePoint firstPoint, bool isFirstArrow,
                 DoublePoint secondPoint, bool isSecondArrow, CPen *pen) {
  dc->MoveTo(firstPoint);
  dc->LineTo(secondPoint);

  if (isFirstArrow) {
    DrawArrowHead(dc, secondPoint, firstPoint, pen);
  }

  if (isSecondArrow) {
    DrawArrowHead(dc, firstPoint, secondPoint, pen);
  }
}

void RLine::SetFirstArrow(bool isEnabled) { IsFirstArrow = isEnabled; }

void RLine::SetSecondArrow(bool isEnabled) { IsSecondArrow = isEnabled; }

void RLine::DrawArrowHead(CDC *dc, DoublePoint from, DoublePoint to,
                          CPen *pen) {
  double angle = DegreesToRadian(30);
  double length = 10;

  if (to == from)
    return;

  DoublePoint diff = to - from;
  double phi = acos(diff.y / CountLength(diff));
  phi *= (diff.x < 0 ? 1 : -1);

  DoublePoint firstDiff{length * sin(phi - angle), length * -cos(phi - angle)};

  DoublePoint secondDiff{length * sin(phi + angle), length * -cos(phi + angle)};

  CBrush brush;
  CPen solidPen;

  LOGPEN logPen;
  pen->GetLogPen(&logPen);
  brush.CreateSolidBrush(logPen.lopnColor);
  solidPen.CreatePen(PS_SOLID, logPen.lopnWidth.x, logPen.lopnColor);

  CBrush *oldBrush = dc->SelectObject(&brush);
  CPen *oldPen = dc->SelectObject(&solidPen);

  CPoint points[] = {to, to + firstDiff, to + secondDiff};
  dc->Polygon(points, 3);

  dc->SelectObject(oldBrush);
  dc->SelectObject(oldPen);
}

std::vector<DrawableProperty> RLine::GetProperties() const {
  auto properties = RDrawable::GetProperties();

  CString tempString;
  CPoint tempPoint;

  tempString.Format(_T("%d"), GetFirstShapeId());
  properties.push_back(
      DrawableProperty(PropName::LINESHAPE1, tempString, PropType::NONE));

  tempString.Format(_T("%d"), GetSecondShapeId());
  properties.push_back(
      DrawableProperty(PropName::LINESHAPE2, tempString, PropType::NONE));

  tempString.Format(_T("%d"), IsFirstArrow);
  properties.push_back(
      DrawableProperty(PropName::LINEHASARROW1, tempString, PropType::BOOL));

  tempString.Format(_T("%d"), IsSecondArrow);
  properties.push_back(
      DrawableProperty(PropName::LINEHASARROW2, tempString, PropType::BOOL));

  return properties;
}

void RLine::SetProperty(PropName name, CString value) {
  switch (name) {
  case PropName::LINEHASARROW1:
    IsFirstArrow = value == _T("1");
    break;
  case PropName::LINEHASARROW2:
    IsSecondArrow = value == _T("1");
    break;
  default:
    RDrawable::SetProperty(name, value);
    break;
  }
}

void RLine::Serialize(CArchive &ar) {
  RDrawable::Serialize(ar);

  if (ar.IsStoring()) {
    firstShapeId = FirstShape->GetId();
    secondShapeId = SecondShape->GetId();

    ar << firstShapeId << secondShapeId;
    ar << IsFirstArrow << IsSecondArrow;
  } else {
    ar >> firstShapeId >> secondShapeId;
    ar >> IsFirstArrow >> IsSecondArrow;
  }
}

void RLine::RestoreReferences(
    const std::vector<std::shared_ptr<RShape>> &shapes) {
  auto firstShapeByID =
      std::find_if(shapes.begin(), shapes.end(),
                   [this](const std::shared_ptr<RShape> &shape) {
                     return shape->GetId() == firstShapeId;
                   });

  auto secondShapeByID =
      std::find_if(shapes.begin(), shapes.end(),
                   [this](const std::shared_ptr<RShape> &shape) {
                     return shape->GetId() == secondShapeId;
                   });

  FirstShape = *firstShapeByID;
  SecondShape = *secondShapeByID;
}

std::shared_ptr<RShape> RLine::GetFirstShape() const { return FirstShape; }

std::shared_ptr<RShape> RLine::GetSecondShape() const { return SecondShape; }
