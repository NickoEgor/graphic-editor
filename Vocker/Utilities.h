#pragma once

#include "DoublePoint.h"
#include "stdafx.h"
#include <vector>

const double M_PI = 3.14159265358979323846;
const double M_PI_2 = 1.57079632679489661923;
const double EPSILON = 1e-9;

CRect ExtendPointToRect(const CPoint &point, int indent);
double CountLength(DoublePoint point);
void CalculateTrianglePoints(CPoint *points, CRect rect);

// rotations
double DegreesToRadian(int degrees);
int RadianToDegrees(double radians);
XFORM SetWorldRotation(HDC hdc, double angle = 0, CPoint shift = {0, 0});
DoublePoint GetRotatedPoint(DoublePoint point, double angle,
                            DoublePoint shift = {0, 0});
CString ColorToRGBString(COLORREF color);

// casts
int CStringToInt(CString str);
unsigned long CStringToUlong(CString str);
double CStringToDouble(CString str);

// serialization
CArchive &operator>>(CArchive &ar, CPen &pen);
CArchive &operator<<(CArchive &ar, CPen &pen);

CArchive &operator>>(CArchive &ar, CBrush &brush);
CArchive &operator<<(CArchive &ar, CBrush &brush);
