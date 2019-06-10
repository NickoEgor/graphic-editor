#pragma once

#include "VockerDoc.h"
#include <map>

class PropertyGridCtrl : public CMFCPropertyGridCtrl {
public:
  void SetLeftColumnWidth(int cx);
};

// PropertyListDialog dialog

class PropertyListDialog : public CDialogEx {
  DECLARE_DYNAMIC(PropertyListDialog)

public:
  PropertyListDialog(CWnd *pParent,
                     DrawablePtr drawable); // standard constructor
  virtual ~PropertyListDialog();

  virtual BOOL OnInitDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
  enum { IDD = IDD_PROPERTYLISTBOX };
#endif

protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support

  void SetPropListFont();

  DECLARE_MESSAGE_MAP()
public:
  CFont GridFont;
  PropertyGridCtrl PropertyGrid;

private:
  DrawablePtr Drawable;
  std::map<PropName, CMFCPropertyGridProperty *> propPtrs;

public:
  afx_msg void OnBnClickedOk();
};
