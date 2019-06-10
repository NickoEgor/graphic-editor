#pragma once

#include "RShape.h"

// RRectangle - representation of rectangle
class RRectangle : public RShape {
public:
  // constructing
  RRectangle(CPoint firstCorner, CPoint secondCorner);
  RRectangle(CRect rect);

  // moving
  void Move(CSize shiftSize) override;

  // resizing
  void Resize(int markerNumber, CPoint shift) override;
  std::vector<CPoint> GetResizingMarkers() const override;

  // normalizing
  void Normalize() override;

  // other
  std::vector<DoublePoint> GetCornerPoints() const override;
  DoublePoint GetCenter() const override;

  // lines
  DoublePoint GetBorderPoint(double angle) const override;

  // drawing
  void Draw(CDC *dc, CPoint scrollPosition) const override;

  // properties
  std::vector<DrawableProperty> GetProperties() const override;
  void SetProperty(PropName name, CString value) override;

  // serialization
  void Serialize(CArchive &ar) override;

private:
  DECLARE_SERIAL(RRectangle);
  RRectangle();

  DoublePoint TopLeft, BottomRight;

  enum class MarkerSide { TOP = 0, RIGHT = 1, BOTTOM = 2, LEFT = 3 };
};
