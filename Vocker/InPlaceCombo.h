#include "DrawableProperty.h"
#include "RDrawable.h"
#include "stdafx.h"

class InPlaceCombo : public CComboBox {
private:
  int Row, Col;
  BOOL IsEsc;
  int SelectedItem;
  IDType Id;

  DrawableProperty Property;

  CStringList DropDownList;
  CString InitialString;

public:
  InPlaceCombo(int row, int col, IDType id, DrawableProperty property,
               CStringList &comboItemsList, int selectedItem);
  virtual BOOL PreTranslateMessage(MSG *pMsg);

protected:
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnKillFocus(CWnd *pNewWnd);
  afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg void OnNcDestroy();

  DECLARE_MESSAGE_MAP()
};
