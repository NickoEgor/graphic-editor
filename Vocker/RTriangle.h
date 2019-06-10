#pragma once

#include "RShape.h"

// RTriangle - representation of triangle
class RTriangle : public RShape {
public:
  // constructing
  RTriangle(CRect rect);

  // moving
  void Move(CSize shiftSize) override;

  // resizing
  void Resize(int markerNumber, CPoint shift) override;
  std::vector<CPoint> GetResizingMarkers() const override;

  // rotating
  void Rotate(double radians) override;

  // drawing
  void Draw(CDC *dc, CPoint scrollPosition) const override;

  // normalizing
  void Normalize() override;

  // other
  DoublePoint GetCenter() const override;
  std::vector<DoublePoint> GetCornerPoints() const override;

  // lines
  DoublePoint GetBorderPoint(double angle) const override;

  // properties
  void SetProperty(PropName name, CString value) override;

  // serialization
  virtual void Serialize(CArchive &ar);

private:
  DECLARE_SERIAL(RTriangle);
  RTriangle();

  std::vector<DoublePoint> Points;
};

CArchive &operator>>(CArchive &ar, std::vector<DoublePoint> &vec);
CArchive &operator<<(CArchive &ar, const std::vector<DoublePoint> &vec);
