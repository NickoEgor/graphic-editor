
#pragma once

#include "EditableListCtrl.h"
#include <map>

class CVockerView;

class CPropertiesToolBar : public CMFCToolBar {
public:
  virtual void OnUpdateCmdUI(CFrameWnd * /*pTarget*/, BOOL bDisableIfNoHndler) {
    CMFCToolBar::OnUpdateCmdUI((CFrameWnd *)GetOwner(), bDisableIfNoHndler);
  }

  virtual BOOL AllowShowOnList() const { return FALSE; }
};

class PropertiesWnd : public CDockablePane {
  // Construction
public:
  PropertiesWnd() noexcept;

  void AdjustLayout();

  void SetHeaders(std::vector<PropName> propNames);
  void UpdateValues(std::vector<std::vector<DrawableProperty>> properties);
  void UpdateValue(IDType id, PropName name, CString value);
  void OnCurrentSelectionChange(IDType id);

protected:
  EditableListCtrl m_wndEditList;
  CVockerView *View = nullptr;

  // Implementation
public:
  virtual ~PropertiesWnd();

protected:
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnSetFocus(CWnd *pOldWnd);

  void OnSlide(BOOL bSlideOut) override;
  BOOL CheckStopSlideCondition(BOOL bDirection) override;
  void OnItemChanged(NMHDR *pNMHDR, LRESULT *pResult);

  DECLARE_MESSAGE_MAP()

private:
  std::map<PropName, int> PropNameToColumn;

  bool CheckViewPtr();
};
