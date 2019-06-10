// PropertyListDialog.cpp : implementation file
//

#include "PropertyListDialog.h"
#include "Vocker.h"
#include "afxdialogex.h"
#include "stdafx.h"

void PropertyGridCtrl::SetLeftColumnWidth(int cx) {
  m_nLeftColumnWidth = cx;
  AdjustLayout();
}

// PropertyListDialog dialog

IMPLEMENT_DYNAMIC(PropertyListDialog, CDialogEx)

PropertyListDialog::PropertyListDialog(CWnd *pParent, DrawablePtr drawable)
    : CDialogEx(IDD_PROPERTYLISTBOX, pParent), Drawable(drawable) {}

PropertyListDialog::~PropertyListDialog() {}

BOOL PropertyListDialog::OnInitDialog() {
  CDialogEx::OnInitDialog();

  HICON hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
  SetIcon(hIcon, TRUE);
  SetIcon(hIcon, FALSE);

  auto properties = Drawable->GetProperties();

  SetPropListFont();

  PropertyGrid.SetVSDotNetLook(TRUE);
  PropertyGrid.EnableHeaderCtrl(FALSE);

  CRect rect;
  PropertyGrid.GetWindowRect(&rect);
  PropertyGrid.SetLeftColumnWidth(rect.Width() / 2);

  CMFCPropertyGridProperty *common = new CMFCPropertyGridProperty(_T("Common"));

  CMFCPropertyGridProperty *specific =
      new CMFCPropertyGridProperty(_T("Specific"));

  for (auto &prop : properties) {
    CString name = DrawableProperty::GetPropertyStringName(prop.Name);
    CMFCPropertyGridProperty *propPtr = nullptr;

    switch (prop.Name) {
    // common
    case PropName::ID: {
      IDType id = CStringToUlong(prop.Value);
      propPtr = new CMFCPropertyGridProperty(name, (_variant_t)id, _T(""));
      common->AddSubItem(propPtr);
      break;
    }
    case PropName::NAME:
      propPtr =
          new CMFCPropertyGridProperty(name, (_variant_t)(prop.Value), _T(""));
      common->AddSubItem(propPtr);
      break;
    case PropName::PENCOLOR:
    case PropName::BRUSHCOLOR: {
      COLORREF color = CStringToUlong(prop.Value);
      auto colorPropPtr = new CMFCPropertyGridColorProperty(name, color);
      colorPropPtr->EnableOtherButton(_T("Other..."));
      colorPropPtr->EnableAutomaticButton(_T("Default"),
                                          ::GetSysColor(COLOR_3DFACE));
      propPtr = colorPropPtr;
      common->AddSubItem(propPtr);
      break;
    }
    case PropName::PENSTYLE:
    case PropName::BRUSHSTYLE: {
      const StyleList *styles = nullptr;
      if (prop.Name == PropName::PENSTYLE) {
        styles = &DrawableProperty::PenStyles;
      } else if (prop.Name == PropName::BRUSHSTYLE) {
        styles = &DrawableProperty::BrushStyles;
      }

      auto id = CStringToUlong(prop.Value);
      CString valueStr = DrawableProperty::GetStringByIdentifier(*styles, id);
      propPtr = new CMFCPropertyGridProperty(name, valueStr);

      for (auto &style : *styles) {
        propPtr->AddOption(style.second);
      }
      propPtr->AllowEdit(FALSE);
      common->AddSubItem(propPtr);
      break;
    }
    case PropName::PENWIDTH: {
      long width = CStringToInt(prop.Value);
      propPtr = new CMFCPropertyGridProperty(name, (_variant_t)width, _T(""));
      common->AddSubItem(propPtr);
      break;
    }
    case PropName::ANGLE: {
      double angle = CStringToDouble(prop.Value);
      propPtr = new CMFCPropertyGridProperty(name, (_variant_t)angle, _T(""));
      common->AddSubItem(propPtr);
      break;
    }

    // specific values
    case PropName::RECTHORZ:
    case PropName::RECTVERT:
    case PropName::ELLIPSECENTERX:
    case PropName::ELLIPSECENTERY:
    case PropName::ELLIPSEMAJOR:
    case PropName::ELLIPSEMINOR: {
      double value = CStringToDouble(prop.Value);
      propPtr = new CMFCPropertyGridProperty(name, (_variant_t)value, _T(""));
      specific->AddSubItem(propPtr);
      break;
    }
    case PropName::LINESHAPE1:
    case PropName::LINESHAPE2: {
      IDType shapeID = CStringToUlong(prop.Value);
      propPtr = new CMFCPropertyGridProperty(name, (_variant_t)shapeID, _T(""));
      propPtr->AllowEdit(FALSE);
      specific->AddSubItem(propPtr);
      break;
    }
    case PropName::LINEHASARROW1:
    case PropName::LINEHASARROW2: {
      bool flag = CStringToInt(prop.Value);
      propPtr = new CMFCPropertyGridProperty(name, (_variant_t)flag, _T(""));
      specific->AddSubItem(propPtr);
      break;
    }
    }

    propPtrs[prop.Name] = propPtr;
  }

  PropertyGrid.AddProperty(common);

  if (specific->GetSubItemsCount() > 0) {
    PropertyGrid.AddProperty(specific);
  }

  return TRUE;
}

void PropertyListDialog::SetPropListFont() {
  ::DeleteObject(GridFont.Detach());

  LOGFONT lf;
  afxGlobalData.fontRegular.GetLogFont(&lf);

  NONCLIENTMETRICS info;
  info.cbSize = sizeof(info);

  afxGlobalData.GetNonClientMetrics(info);

  lf.lfHeight = info.lfMenuFont.lfHeight;
  lf.lfWeight = info.lfMenuFont.lfWeight;
  lf.lfItalic = info.lfMenuFont.lfItalic;

  GridFont.CreateFontIndirect(&lf);

  PropertyGrid.SetFont(&GridFont);
}

void PropertyListDialog::DoDataExchange(CDataExchange *pDX) {
  CDialogEx::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_MFCPROPERTYGRID1, PropertyGrid);
}

BEGIN_MESSAGE_MAP(PropertyListDialog, CDialogEx)
ON_BN_CLICKED(IDOK, &PropertyListDialog::OnBnClickedOk)
END_MESSAGE_MAP()

// PropertyListDialog message handlers

void PropertyListDialog::OnBnClickedOk() {
  try {
    for (auto &propItem : propPtrs) {
      if (propItem.second->IsModified()) {
        CString valueStr;

        auto value = propItem.second->GetValue();
        switch (value.vt) {
        case VT_BOOL: // bool
          valueStr.Format(_T("%d"), value.boolVal != VARIANT_FALSE);
          break;
        case VT_I4: // int
          valueStr.Format(_T("%d"), value.lVal);
          break;
        case VT_UI4: // ulong
          valueStr.Format(_T("%d"), value.ulVal);
          break;
        case VT_R4: // float
          valueStr.Format(_T("%f"), value.fltVal);
          break;
        case VT_R8: // double
          valueStr.Format(_T("%f"), value.dblVal);
          break;
        case VT_BSTR: // string
          valueStr = CString(value.bstrVal);
          break;
        }

        if (propItem.first == PropName::PENSTYLE ||
            propItem.first == PropName::BRUSHSTYLE) {
          const StyleList *styles = nullptr;
          if (propItem.first == PropName::PENSTYLE) {
            styles = &DrawableProperty::PenStyles;
          } else if (propItem.first == PropName::BRUSHSTYLE) {
            styles = &DrawableProperty::BrushStyles;
          }

          auto id = DrawableProperty::GetIdentifierByString(*styles, valueStr);
          valueStr.Format(_T("%d"), id);
        }

        Drawable->SetProperty(propItem.first, valueStr);
      }
    }
    CDialogEx::OnOK();
  } catch (const std::exception &e) {
    MessageBox(CString(e.what()), _T("Error"), MB_OK | MB_ICONERROR);
  }
}
