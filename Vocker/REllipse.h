#pragma once
#include "RShape.h"

// REllipse - representation of ellipse
class REllipse : public RShape {
public:
  // constructing
  REllipse(CPoint firstCorner, CPoint secondCorner);
  REllipse(CRect rect);

  // moving
  void Move(CSize shiftSize) override;

  // resizing
  void Resize(int markerNumber, CPoint shift) override;
  std::vector<CPoint> GetResizingMarkers() const override;

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
  std::vector<DrawableProperty> GetProperties() const override;
  void SetProperty(PropName name, CString value) override;

  // serialization
  void Serialize(CArchive &ar) override;

private:
  DECLARE_SERIAL(REllipse);
  REllipse();

  DoublePoint center;
  double majorAxis /* horizontal */, minorAxis;
  CRect GetRect() const;

  enum class MarkerSide { TOP = 0, RIGHT = 1, BOTTOM = 2, LEFT = 3 };
};
