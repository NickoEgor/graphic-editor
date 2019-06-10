#pragma once

#include "DoublePoint.h"
#include "DrawableProperty.h"
#include <set>
#include <vector>

using IDType = unsigned long;

// RDrawable - representation of drawable object
class RDrawable : public CObject {
public:
  // constructing/destructing
  RDrawable();
  virtual ~RDrawable();

  // id get/set
  IDType GetId() const;
  void SetId(IDType Id);

  // name get/set
  CString GetName() const;
  void SetName(CString Name);

  // drawing
  virtual void Draw(CDC *dc, CPoint scrollOffset) const = 0;

  // other
  void SetPen(LPLOGPEN pen);
  bool IsPointInDrawable(CPoint point) const;

  // properties
  virtual std::vector<DrawableProperty> GetProperties() const;
  virtual void SetProperty(PropName name, CString value);

  // serialization
  virtual void Serialize(CArchive &ar);

protected:
  virtual void SetUniqueName(CString prefix = _T("Drawable"));

  unsigned long Id;
  CString Name;

  mutable CPen Pen;
  mutable CRgn Region;

private:
  static IDType NextId;
  static std::set<IDType> Ids;
  static bool IsIdExists(IDType id);

  void SetUniqueId();

  static std::set<CString> Names;
  static bool IsNameExists(const CString &name);
};
