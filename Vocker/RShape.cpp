#include "RShape.h"
#include "stdafx.h"
#include <algorithm>

RShape::RShape() {}

DoublePoint RShape::GetBorderPoint(double angle) const { return GetCenter(); }

const std::vector<std::shared_ptr<RLine>> &RShape::GetLines() const {
  return Lines;
}

void RShape::Rotate(double radians) { Angle += radians; }

std::vector<DrawableProperty> RShape::GetProperties() const {
  auto properties = RDrawable::GetProperties();

  CString tempString;
  CPoint center = GetCenter();

  LOGBRUSH brushInfo;
  Brush.GetLogBrush(&brushInfo);

  tempString.Format(_T("%d"), brushInfo.lbColor);
  properties.push_back(
      DrawableProperty(PropName::BRUSHCOLOR, tempString, PropType::COLOR));

  UINT brushId = DrawableProperty::GetBrushIdentifier(brushInfo);
  tempString.Format(_T("%d"), brushId);
  properties.push_back(
      DrawableProperty(PropName::BRUSHSTYLE, tempString, PropType::BRUSHSTYLE));

  tempString.Format(_T("%f"), Angle);
  properties.push_back(
      DrawableProperty(PropName::ANGLE, tempString, PropType::DOUBLE));

  return properties;
}

void RShape::SetProperty(PropName name, CString value) {
  switch (name) {
  case PropName::BRUSHCOLOR: {
    try {
      COLORREF color = CStringToUlong(value);
      LOGBRUSH brushInfo;
      Brush.GetLogBrush(&brushInfo);
      Brush.DeleteObject();
      if (brushInfo.lbStyle == BS_SOLID) {
        Brush.CreateSolidBrush(color);
      } else {
        Brush.CreateHatchBrush(brushInfo.lbHatch, color);
      }
    } catch (std::invalid_argument) {
      throw std::invalid_argument("Invalid value");
    }
    break;
  }
  case PropName::BRUSHSTYLE: {
    int style = CStringToInt(value);
    LOGBRUSH brushInfo;
    Brush.GetLogBrush(&brushInfo);
    Brush.DeleteObject();

    if (style != DrawableProperty::SolidBrushStyle) {
      Brush.CreateHatchBrush(style, brushInfo.lbColor);
    } else {
      Brush.CreateSolidBrush(brushInfo.lbColor);
    }
    break;
  }
  case PropName::ANGLE: {
    Angle = CStringToDouble(value);
    break;
  }
  default:
    RDrawable::SetProperty(name, value);
    break;
  }
}

double RShape::GetAngle() const { return Angle; }

void RShape::SetBrush(LPLOGBRUSH brush) {
  this->Brush.DeleteObject();
  this->Brush.CreateBrushIndirect(brush);
}

void RShape::AddLine(std::shared_ptr<RLine> line) { Lines.push_back(line); }

void RShape::RemoveLine(std::shared_ptr<RLine> line) {
  Lines.erase(std::find(Lines.begin(), Lines.end(), line));
}

void RShape::RemoveLines() { Lines.clear(); }

void RShape::Serialize(CArchive &ar) {
  RDrawable::Serialize(ar);

  size_t linesSize;

  if (ar.IsStoring()) {
    ar << Angle << Brush;

    LinesID.clear();
    for (auto &line : Lines) {
      LinesID.push_back(line->GetId());
    }
    linesSize = Lines.size();

    ar << linesSize << LinesID;
  } else {
    ar >> Angle >> Brush;

    ar >> linesSize;
    LinesID.resize(linesSize);
    ar >> LinesID;
  }
}

void RShape::RestoreReferences(
    const std::vector<std::shared_ptr<RLine>> &lines) {
  Lines.clear();
  for (auto lineId : LinesID) {
    auto lineById = std::find_if(lines.begin(), lines.end(),
                                 [&lineId](const std::shared_ptr<RLine> &line) {
                                   return line->GetId() == lineId;
                                 });

    Lines.push_back(*lineById);
  }
}

CArchive &operator>>(CArchive &ar, std::vector<IDType> &vec) {
  for (size_t i = 0; i < vec.size(); ++i) {
    ar >> vec[i];
  }

  return ar;
}

CArchive &operator<<(CArchive &ar, const std::vector<IDType> &vec) {
  for (auto &item : vec) {
    ar << item;
  }

  return ar;
}
