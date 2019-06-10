#include "RDrawable.h"
#include "Utilities.h"
#include "stdafx.h"
#include <set>

IDType RDrawable::NextId = 0;
std::set<IDType> RDrawable::Ids;
std::set<CString> RDrawable::Names;

RDrawable::RDrawable() { SetUniqueId(); }

unsigned long RDrawable::GetId() const { return Id; }

void RDrawable::SetId(IDType id) {
  if (id == Id)
    return;

  if (IsIdExists(id)) {
    throw std::invalid_argument("Id is occupied");
  }

  Ids.erase(Id);
  this->Id = id;
  Ids.insert(id);
}

CString RDrawable::GetName() const { return Name; }

void RDrawable::SetName(CString name) {
  if (name == Name)
    return;

  if (IsNameExists(name)) {
    throw std::invalid_argument("Name is occupied");
  } else if (name.GetLength() == 0) {
    throw std::invalid_argument("Name can't be empty");
  }

  Names.erase(Name);
  this->Name = name;
  Names.insert(name);
}

void RDrawable::SetPen(LPLOGPEN pen) {
  this->Pen.DeleteObject();
  this->Pen.CreatePenIndirect(pen);
}

std::vector<DrawableProperty> RDrawable::GetProperties() const {
  std::vector<DrawableProperty> properties;

  CString tempString;

  tempString.Format(_T("%d"), Id);
  properties.push_back(
      DrawableProperty(PropName::ID, tempString, PropType::UINT));

  properties.push_back(
      DrawableProperty(PropName::NAME, Name, PropType::STRING));

  LOGPEN penInfo;
  Pen.GetLogPen(&penInfo);

  tempString.Format(_T("%d"), penInfo.lopnColor);
  properties.push_back(
      DrawableProperty(PropName::PENCOLOR, tempString, PropType::COLOR));

  tempString.Format(_T("%d"), penInfo.lopnStyle);
  properties.push_back(
      DrawableProperty(PropName::PENSTYLE, tempString, PropType::PENSTYLE));

  tempString.Format(_T("%d"), penInfo.lopnWidth.x);
  properties.push_back(
      DrawableProperty(PropName::PENWIDTH, tempString, PropType::UINT));

  return properties;
}

void RDrawable::SetProperty(PropName name, CString value) {
  switch (name) {
  case PropName::ID: {
    SetId(CStringToUlong(value));
    break;
  }
  case PropName::NAME: {
    SetName(value);
    break;
  }
  case PropName::PENCOLOR: {
    COLORREF color = CStringToUlong(value);
    LOGPEN penInfo;
    Pen.GetLogPen(&penInfo);
    Pen.DeleteObject();
    Pen.CreatePen(penInfo.lopnStyle, penInfo.lopnWidth.x, color);
    break;
  }
  case PropName::PENSTYLE: {
    int style = CStringToInt(value);
    LOGPEN penInfo;
    Pen.GetLogPen(&penInfo);
    Pen.DeleteObject();
    Pen.CreatePen(style, penInfo.lopnWidth.x, penInfo.lopnColor);
    break;
  }
  case PropName::PENWIDTH: {
    int width = CStringToInt(value);
    LOGPEN penInfo;
    Pen.GetLogPen(&penInfo);
    Pen.DeleteObject();
    Pen.CreatePen(penInfo.lopnStyle, width, penInfo.lopnColor);
    break;
  }
  }
}

bool RDrawable::IsPointInDrawable(CPoint point) const {
  return Region.PtInRegion(point);
}

void RDrawable::SetUniqueId() {
  while (IsIdExists(NextId)) {
    NextId++;
  }
  Id = NextId;
  Ids.insert(NextId);
  NextId++;
}

void RDrawable::SetUniqueName(CString prefix) {
  CString name;
  IDType num = Id;
  do {
    name.Format(prefix + "%d", num);
    num++;
  } while (IsNameExists(name));

  Name = name;
  Names.insert(Name);
}

bool RDrawable::IsIdExists(IDType id) { return Ids.find(id) != Ids.end(); }

bool RDrawable::IsNameExists(const CString &name) {
  return Names.find(name) != Names.end();
}

RDrawable::~RDrawable() {
  Ids.erase(Id);
  Names.erase(Name);
}

void RDrawable::Serialize(CArchive &ar) {
  CObject::Serialize(ar);

  if (ar.IsStoring()) {
    ar << Id << Name << Pen;
  } else {
    ar >> Id >> Name >> Pen;
  }
}
