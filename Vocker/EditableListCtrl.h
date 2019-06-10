#pragma once

#include "DrawableProperty.h"
#include "InPlaceCombo.h"
#include "InPlaceEdit.h"
#include "RDrawable.h"
#include <memory>
#include <utility>

class EditableListCtrl : public CMFCListCtrl {
public:
  BOOL PreCreateWindow(CREATESTRUCT &cs);
  int OnCreate(LPCREATESTRUCT lpCreateStruct);

  void InitializeList(std::vector<std::vector<DrawableProperty>> properties);

  void EditColor(int row, int col, IDType id, PropName name);
  void EditBool(int row, int col, IDType id, PropName name);
  InPlaceEdit *EditSubLabel(int row, int col, IDType id, DrawableProperty prop);
  InPlaceCombo *EditComboBox(int row, int col, IDType id, DrawableProperty prop,
                             CStringList &comboItemsList, int selectedItem);

  int GetRowFromPoint(CPoint &point, int *col) const;
  void CalculateCellRect(int row, int col, CRect &robCellRect);

  void UpdateCellValue(IDType id, PropName name, CString value);
  IDType GetSelectionID();

  void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
  void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);

  void SetCellText(int row, int col, PropType type, CString text);
  void SetSelectionById(IDType id);

  COLORREF OnGetCellBkColor(int nRow, int nColumn);
  COLORREF OnGetCellTextColor(int nRow, int nColumn);

protected:
  afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

  DECLARE_MESSAGE_MAP()

private:
  std::vector<std::vector<DrawableProperty>> Properties;

  static const CString TrueStr;
  static const CString FalseStr;

  static PropType SortPropType;
};
