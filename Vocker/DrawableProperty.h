#pragma once

#include "stdafx.h"
#include <utility>
#include <vector>

using StyleList = std::vector<std::pair<UINT, CString>>;

enum class PropType {
  NONE,
  UINT,
  DOUBLE,
  BOOL,
  STRING,
  PENSTYLE,
  BRUSHSTYLE,
  COLOR,
};

enum class PropName {
  // common
  ID,
  NAME,
  PENCOLOR,
  PENSTYLE,
  PENWIDTH,

  // shape
  BRUSHCOLOR,
  BRUSHSTYLE,
  ANGLE,

  // rectangle
  RECTHORZ,
  RECTVERT,

  // ellipse
  ELLIPSECENTERX,
  ELLIPSECENTERY,
  ELLIPSEMAJOR,
  ELLIPSEMINOR,

  // line
  LINESHAPE1,
  LINESHAPE2,
  LINEHASARROW1,
  LINEHASARROW2
};

struct DrawableProperty {
  PropName Name;
  CString Value;
  PropType Type;

  DrawableProperty(PropName name, CString value, PropType type);

  static UINT GetBrushIdentifier(LOGBRUSH brushInfo);
  static CString GetStringByIdentifier(const StyleList &list, UINT id);
  static UINT GetIdentifierByString(const StyleList &list, CString str);
  static int GetIndexById(const StyleList &list, UINT id);
  static CString GetPropertyStringName(PropName name);

  static std::vector<PropName> GetDrawableNames();
  static std::vector<PropName> GetShapeNames();
  static std::vector<PropName> GetLineNames();

  static const int SolidBrushStyle = 100;
  static const StyleList BrushStyles;
  static const StyleList PenStyles;
};
