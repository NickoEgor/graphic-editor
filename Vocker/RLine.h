#pragma once

#include "RDrawable.h"
#include "Utilities.h"
#include <memory>

class RShape;

// RLine - representation of connection line
class RLine : public RDrawable {
public:
  // construction
  RLine(const std::shared_ptr<RShape> &firstShape,
        const std::shared_ptr<RShape> &secondShape);

  // drawing
  void Draw(CDC *dc, CPoint scrollPosition) const override;
  static void Draw(CDC *dc, DoublePoint firstPoint, bool isFirstArrow,
                   DoublePoint secondPoint, bool isSecondArrow, CPen *pen);

  // shapes properties
  void SetFirstArrow(bool isEnabled = true);
  void SetSecondArrow(bool isEnabled = true);

  std::shared_ptr<RShape> GetFirstShape() const;
  std::shared_ptr<RShape> GetSecondShape() const;
  int GetFirstShapeId() const;
  int GetSecondShapeId() const;

  // properties
  std::vector<DrawableProperty> GetProperties() const override;
  void SetProperty(PropName name, CString value) override;

  // serialization
  void Serialize(CArchive &ar) override;
  void RestoreReferences(const std::vector<std::shared_ptr<RShape>> &shapes);

private:
  DECLARE_SERIAL(RLine);
  RLine();

  std::shared_ptr<RShape> FirstShape, SecondShape;
  bool IsFirstArrow = false, IsSecondArrow = false;

  IDType firstShapeId, secondShapeId;

  static void DrawArrowHead(CDC *dc, DoublePoint from, DoublePoint to,
                            CPen *pen);
};
