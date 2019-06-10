#include "Utilities.h"
#include "stdafx.h"
#include <string>

CRect ExtendPointToRect(const CPoint &point, int indent) {
  return CRect{point.x - indent, point.y - indent, point.x + indent,
               point.y + indent};
}

double DegreesToRadian(int degrees) { return degrees * M_PI / 180.0; }

int RadianToDegrees(double radians) {
  return static_cast<int>(round(radians * 180 / M_PI));
}

XFORM SetWorldRotation(HDC hdc, double angle, CPoint shift) {
  XFORM xForm;
  SetGraphicsMode(hdc, GM_ADVANCED);
  double angleCos = cos(angle), angleSin = sin(angle);
  xForm.eM11 = static_cast<FLOAT>(angleCos);
  xForm.eM12 = static_cast<FLOAT>(angleSin);
  xForm.eM21 = static_cast<FLOAT>(-angleSin);
  xForm.eM22 = static_cast<FLOAT>(angleCos);
  xForm.eDx = static_cast<FLOAT>(shift.x);
  xForm.eDy = static_cast<FLOAT>(shift.y);
  SetWorldTransform(hdc, &xForm);
  return xForm;
}

DoublePoint GetRotatedPoint(DoublePoint point, double angle,
                            DoublePoint shift) {
  double angleCos = cos(angle), angleSin = sin(angle);
  DoublePoint nonShiftedPoint = point - shift;
  return DoublePoint(
      nonShiftedPoint.x * angleCos + nonShiftedPoint.y * (-angleSin) + shift.x,
      nonShiftedPoint.x * (angleSin) + nonShiftedPoint.y * angleCos + shift.y);
}

CString ColorToRGBString(COLORREF color) {
  int red = GetRValue(color);
  int green = GetGValue(color);
  int blue = GetBValue(color);
  CString str;
  str.Format(_T("(%d %d %d)"), red, green, blue);
  return str;
}

double CountLength(DoublePoint point) {
  return sqrt(point.x * point.x + point.y * point.y);
}

void CalculateTrianglePoints(CPoint *points, CRect rect) {
  points[0].x = static_cast<int>((rect.left + rect.right) / 2.0);
  points[0].y = rect.top;
  points[1] = rect.BottomRight();
  points[2] = {rect.left, rect.bottom};
}

int CStringToInt(CString str) {
  return std::stoi({str.GetString(), static_cast<size_t>(str.GetLength())});
}

unsigned long CStringToUlong(CString str) {
  if (str[0] == '-') {
    throw std::invalid_argument("Invalid value");
  }
  return std::stoul({str.GetString(), static_cast<size_t>(str.GetLength())});
}

double CStringToDouble(CString str) {
  return std::stod({str.GetString(), static_cast<size_t>(str.GetLength())});
}

CArchive &operator>>(CArchive &ar, CPen &pen) {
  int style;
  int width;
  COLORREF color;

  ar >> style >> width >> color;

  pen.DeleteObject();
  pen.CreatePen(style, width, color);

  return ar;
}

CArchive &operator<<(CArchive &ar, CPen &pen) {
  LOGPEN penInfo;
  pen.GetLogPen(&penInfo);

  ar << static_cast<int>(penInfo.lopnStyle);
  ar << static_cast<int>(penInfo.lopnWidth.x);
  ar << penInfo.lopnColor;
  return ar;
}

CArchive &operator>>(CArchive &ar, CBrush &brush) {
  int style;
  int hatch;
  COLORREF color;

  ar >> style >> hatch >> color;

  brush.DeleteObject();
  if (style == BS_SOLID) {
    brush.CreateSolidBrush(color);
  } else {
    brush.CreateHatchBrush(hatch, color);
  }

  return ar;
}

CArchive &operator<<(CArchive &ar, CBrush &brush) {
  LOGBRUSH brushInfo;
  brush.GetLogBrush(&brushInfo);

  ar << static_cast<int>(brushInfo.lbStyle);
  ar << static_cast<int>(brushInfo.lbHatch);
  ar << brushInfo.lbColor;
  return ar;
}
