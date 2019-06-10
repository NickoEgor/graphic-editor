#pragma once

#include "RDrawable.h"
#include "RLine.h"
#include "Utilities.h"
#include <memory>

// RShape - representation of abstract shape
class RShape : public RDrawable {
public:
  // constructing
  RShape();

  // moving
  virtual void Move(CSize shiftSize) = 0;

  // resizing
  virtual void Resize(int markerNumber, CPoint shift) = 0;
  virtual std::vector<CPoint> GetResizingMarkers() const = 0;

  // rotation
  virtual void Rotate(double radians);
  double GetAngle() const;

  // normalizing
  virtual void Normalize() = 0;

  // other
  virtual DoublePoint GetCenter() const = 0;
  virtual std::vector<DoublePoint> GetCornerPoints() const = 0;
  void SetBrush(LPLOGBRUSH brush);

  // lines
  virtual DoublePoint GetBorderPoint(double angle) const;
  const std::vector<std::shared_ptr<RLine>> &GetLines() const;
  void AddLine(std::shared_ptr<RLine> line);
  void RemoveLine(std::shared_ptr<RLine> line);
  void RemoveLines();

  // properties
  virtual std::vector<DrawableProperty> GetProperties() const override;
  virtual void SetProperty(PropName name, CString value) override;

  // serialization
  virtual void Serialize(CArchive &ar) override;
  void RestoreReferences(const std::vector<std::shared_ptr<RLine>> &lines);

protected:
  double Angle = 0;

  mutable CBrush Brush;

  std::vector<std::shared_ptr<RLine>> Lines;
  std::vector<IDType> LinesID;
};

CArchive &operator>>(CArchive &ar, std::vector<IDType> &vec);
CArchive &operator<<(CArchive &ar, const std::vector<IDType> &vec);
