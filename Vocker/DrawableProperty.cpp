#include "DrawableProperty.h"
#include "stdafx.h"

DrawableProperty::DrawableProperty(PropName name, CString value, PropType type)
    : Name(name), Value(value), Type(type) {}

const std::vector<std::pair<UINT, CString>> DrawableProperty::BrushStyles{
    std::make_pair(SolidBrushStyle, _T("Solid")),
    std::make_pair(HS_HORIZONTAL, _T("Horizontal hatch")),
    std::make_pair(HS_VERTICAL, _T("Vertical hatch")),
    std::make_pair(HS_FDIAGONAL, _T("Upward hatch at 45 degrees")),
    std::make_pair(HS_BDIAGONAL, _T("Downward hatch at 45 degrees")),
    std::make_pair(HS_CROSS, _T("Horizontal and vertical crosshatch")),
    std::make_pair(HS_DIAGCROSS, _T("Crosshatch at 45 degrees"))};

const std::vector<std::pair<UINT, CString>> DrawableProperty::PenStyles{
    std::make_pair(PS_DASH, _T("Dash")), std::make_pair(PS_DOT, _T("Dot")),
    std::make_pair(PS_DASHDOT, _T("Dash/Dot")),
    std::make_pair(PS_DASHDOTDOT, _T("Dash/Dot/Dot")),
    std::make_pair(PS_SOLID, _T("Solid"))};

UINT DrawableProperty::GetBrushIdentifier(LOGBRUSH brushInfo) {
  if (brushInfo.lbStyle == BS_SOLID) {
    return SolidBrushStyle;
  } else {
    return brushInfo.lbHatch;
  }
}

CString DrawableProperty::GetStringByIdentifier(const StyleList &list,
                                                UINT id) {
  for (auto &item : list) {
    if (item.first == id) {
      return item.second;
    }
  }

  return L"";
}

UINT DrawableProperty::GetIdentifierByString(const StyleList &list,
                                             CString str) {
  for (auto &item : list) {
    if (item.second == str) {
      return item.first;
    }
  }

  return -1;
}

int DrawableProperty::GetIndexById(const StyleList &list, UINT id) {
  for (size_t i = 0; i < list.size(); ++i) {
    if (list[i].first == id) {
      return i;
    }
  }

  // to get max uint value
  return -1;
}

CString DrawableProperty::GetPropertyStringName(PropName name) {
  switch (name) {
  case PropName::ID:
    return _T("ID");
  case PropName::NAME:
    return _T("Name");
  case PropName::PENCOLOR:
    return _T("Pen Color");
  case PropName::PENSTYLE:
    return _T("Pen Style");
  case PropName::PENWIDTH:
    return _T("Pen Width");
  case PropName::BRUSHCOLOR:
    return _T("Brush Color");
  case PropName::BRUSHSTYLE:
    return _T("Brush Style");
  case PropName::ANGLE:
    return _T("Angle");
  case PropName::RECTHORZ:
    return _T("Horizontal size");
  case PropName::RECTVERT:
    return _T("Vertical size");
  case PropName::ELLIPSECENTERX:
    return _T("Center X");
  case PropName::ELLIPSECENTERY:
    return _T("Center Y");
  case PropName::ELLIPSEMAJOR:
    return _T("Major Axis");
  case PropName::ELLIPSEMINOR:
    return _T("Minor Axis");
  case PropName::LINESHAPE1:
    return _T("Shape #1 ID");
  case PropName::LINESHAPE2:
    return _T("Shape #2 ID");
  case PropName::LINEHASARROW1:
    return _T("Arrow #1");
  case PropName::LINEHASARROW2:
    return _T("Arrow #2");
  }

  return _T("");
}

std::vector<PropName> DrawableProperty::GetDrawableNames() {
  return {PropName::ID, PropName::NAME, PropName::PENCOLOR, PropName::PENSTYLE,
          PropName::PENWIDTH};
}

std::vector<PropName> DrawableProperty::GetShapeNames() {
  auto props = GetDrawableNames();
  props.push_back(PropName::BRUSHCOLOR);
  props.push_back(PropName::BRUSHSTYLE);
  props.push_back(PropName::ANGLE);
  return props;
}

std::vector<PropName> DrawableProperty::GetLineNames() {
  auto props = GetDrawableNames();
  props.push_back(PropName::LINESHAPE1);
  props.push_back(PropName::LINESHAPE2);
  props.push_back(PropName::LINEHASARROW1);
  props.push_back(PropName::LINEHASARROW2);
  return props;
}
