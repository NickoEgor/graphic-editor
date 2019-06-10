#pragma once

#include "RDrawable.h"

class InPlaceEdit : public CEdit {
private:
  int Row, Col;
  CString InitText;
  BOOL IsEsc;

  IDType Id;
  DrawableProperty Property;

public:
  InPlaceEdit(int row, int col, IDType id, DrawableProperty property,
              CString initText);
  virtual BOOL PreTranslateMessage(MSG *pMsg);

protected:
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnKillFocus(CWnd *pNewWnd);
  afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg void OnNcDestroy();

  DECLARE_MESSAGE_MAP()
};
