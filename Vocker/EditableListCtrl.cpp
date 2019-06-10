#include "EditableListCtrl.h"
#include "Resource.h"
#include "Utilities.h"
#include "stdafx.h"
#include <algorithm>

BEGIN_MESSAGE_MAP(EditableListCtrl, CMFCListCtrl)
ON_WM_CREATE()
ON_WM_LBUTTONDBLCLK()

ON_WM_KEYDOWN()
END_MESSAGE_MAP()

const CString EditableListCtrl::TrueStr = _T("Enabled");
const CString EditableListCtrl::FalseStr = _T("Disabled");

BOOL EditableListCtrl::PreCreateWindow(CREATESTRUCT &cs) {
  cs.style |= WS_CHILD | LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS;
  return CMFCListCtrl::PreCreateWindow(cs);
}

int EditableListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) {
  if (CMFCListCtrl::OnCreate(lpCreateStruct) == -1)
    return -1;

  return 0;
}

void EditableListCtrl::InitializeList(
    std::vector<std::vector<DrawableProperty>> properties) {
  size_t selected = ULONG_MAX;

  POSITION pos = GetFirstSelectedItemPosition();
  if (pos != NULL) {
    selected = GetNextSelectedItem(pos);
  }

  DeleteAllItems();
  Properties = properties;

  for (size_t k = 0; k < Properties.size(); ++k) {
    auto &itemProps = Properties[k];
    InsertItem(k, itemProps[0].Value);

    for (size_t j = 1; j < itemProps.size(); ++j) {
      SetCellText(k, j, itemProps[j].Type, itemProps[j].Value);
    }
  }

  if (selected != ULONG_MAX && selected >= Properties.size()) {
    selected = Properties.size() - 1;
  }

  if (selected != ULONG_MAX && selected < Properties.size()) {
    SetItemState(selected, LVIS_SELECTED, LVIS_SELECTED);
    SetSelectionMark(selected);
  }
}

void EditableListCtrl::EditColor(int row, int col, IDType id, PropName name) {
  CColorDialog dialog(CStringToUlong(Properties[row][col].Value));
  if (dialog.DoModal() != IDOK)
    return;

  COLORREF color = dialog.GetColor();
  CString colorStr;
  colorStr.Format(_T("%d"), color);

  // Properties[row][col].Value = colorStr;

  CWnd *parent = GetParent();
  LV_DISPINFO dispInfo;
  dispInfo.hdr.hwndFrom = parent->m_hWnd;
  dispInfo.hdr.code = LVN_ENDLABELEDIT;
  dispInfo.item.mask = LVIF_TEXT;
  // dispInfo.item.iItem = row;
  // dispInfo.item.iSubItem = col;
  dispInfo.item.pszText = LPTSTR((LPCTSTR)colorStr);
  dispInfo.item.cchTextMax = colorStr.GetLength();
  dispInfo.item.iItem = static_cast<int>(name);
  dispInfo.item.lParam = id;

  AfxGetMainWnd()->SendMessage(WM_NOTIFY_PROPERTY_EDITED, NULL,
                               (LPARAM)&dispInfo);
}

void EditableListCtrl::EditBool(int row, int col, IDType id, PropName name) {
  CString newValue =
      (Properties[row][col].Value != _T("1") ? _T("1") : _T("0"));

  CWnd *parent = GetParent();
  LV_DISPINFO dispInfo;
  dispInfo.hdr.hwndFrom = parent->m_hWnd;
  dispInfo.hdr.idFrom = GetDlgCtrlID();
  dispInfo.hdr.code = LVN_ENDLABELEDIT;
  dispInfo.item.mask = LVIF_TEXT;
  // dispInfo.item.iItem = row;
  // dispInfo.item.iSubItem = col;
  dispInfo.item.pszText = LPTSTR((LPCTSTR)newValue);
  dispInfo.item.cchTextMax = newValue.GetLength();
  dispInfo.item.iItem = static_cast<int>(name);
  dispInfo.item.lParam = id;

  AfxGetMainWnd()->SendMessage(WM_NOTIFY_PROPERTY_EDITED, NULL,
                               (LPARAM)&dispInfo);
}

InPlaceCombo *EditableListCtrl::EditComboBox(int row, int col, IDType id,
                                             DrawableProperty prop,
                                             CStringList &comboItemsList,
                                             int selectedIndex) {
  if (!EnsureVisible(row, TRUE)) {
    return nullptr;
  }

  // Make sure that column index is valid
  CHeaderCtrl *header = static_cast<CHeaderCtrl *>(GetDlgItem(0));

  int columnCount = header->GetItemCount();
  int minColumnWidth = 10;

  if (col >= columnCount || GetColumnWidth(col) < minColumnWidth) {
    return nullptr;
  }

  // Calculate the rectangle specifications for the combo box
  CRect cellRect(0, 0, 0, 0);
  CalculateCellRect(row, col, cellRect);

  int height = cellRect.Height();
  int count = comboItemsList.GetCount();

  int maxDropDownItemCount = 10;
  count = (count < maxDropDownItemCount) ? count + maxDropDownItemCount
                                         : (maxDropDownItemCount + 1);

  cellRect.bottom += height * count;

  int dropDownStyle = WS_BORDER | WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL |
                      ES_AUTOVSCROLL | CBS_DROPDOWNLIST | CBS_DISABLENOSCROLL;

  InPlaceCombo *inPlaceCombo =
      new InPlaceCombo(row, col, id, prop, comboItemsList, selectedIndex);
  inPlaceCombo->Create(dropDownStyle, cellRect, this, 0);

  return inPlaceCombo;
}

InPlaceEdit *EditableListCtrl::EditSubLabel(int row, int col, IDType id,
                                            DrawableProperty prop) {
  // The returned pointer should not be saved, make sure item visible
  if (!EnsureVisible(row, TRUE)) {
    return nullptr;
  }

  // Make sure that column number is valid
  CHeaderCtrl *header = static_cast<CHeaderCtrl *>(GetDlgItem(0));
  int columnCount = header->GetItemCount();
  int minColumnWidth = 10;

  if (col >= columnCount || GetColumnWidth(col) < minColumnWidth) {
    return nullptr;
  }

  // Get Column alignment
  LV_COLUMN lvcol;
  lvcol.mask = LVCF_FMT;
  GetColumn(col, &lvcol);
  DWORD dwStyle;

  if ((lvcol.fmt & LVCFMT_JUSTIFYMASK) == LVCFMT_LEFT) {
    dwStyle = ES_LEFT;
  } else if ((lvcol.fmt & LVCFMT_JUSTIFYMASK) == LVCFMT_RIGHT) {
    dwStyle = ES_RIGHT;
  } else {
    dwStyle = ES_CENTER;
  }

  dwStyle |= WS_BORDER | WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL;

  CRect cellRect(0, 0, 0, 0);
  CalculateCellRect(row, col, cellRect);

  InPlaceEdit *pEdit =
      new InPlaceEdit(row, col, id, prop, GetItemText(row, col));
  pEdit->Create(dwStyle, cellRect, this, 0);

  return pEdit;
}

void EditableListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) {
  CMFCListCtrl::OnLButtonDown(nFlags, point);

  int col;
  int row = GetRowFromPoint(point, &col);
  if (row != -1) {
    UINT flag = LVIS_FOCUSED;
    if ((GetItemState(row, flag) & flag) == flag) {
      if (GetWindowLong(m_hWnd, GWL_STYLE)) {
        auto &prop = Properties[row][col];
        IDType id = CStringToUlong(Properties[row][0].Value);

        switch (prop.Type) {
        case PropType::UINT:
        case PropType::DOUBLE:
        case PropType::STRING:
          EditSubLabel(row, col, id, prop);
          break;
        case PropType::BOOL:
          EditBool(row, col, id, prop.Name);
          break;
        case PropType::PENSTYLE: {
          int selectedIndex = DrawableProperty::GetIndexById(
              DrawableProperty::PenStyles, CStringToInt(prop.Value));

          CStringList comboItemsList;
          for (auto &item : DrawableProperty::PenStyles) {
            comboItemsList.AddTail(item.second);
          }

          EditComboBox(row, col, id, prop, comboItemsList, selectedIndex);
          break;
        }
        case PropType::BRUSHSTYLE: {
          int selectedIndex = DrawableProperty::GetIndexById(
              DrawableProperty::BrushStyles, CStringToInt(prop.Value));

          CStringList comboItemsList;
          for (auto &item : DrawableProperty::BrushStyles) {
            comboItemsList.AddTail(item.second);
          }

          EditComboBox(row, col, id, prop, comboItemsList, selectedIndex);
          break;
        }
        case PropType::COLOR:
          EditColor(row, col, id, prop.Name);
          break;
        }
      }
    } else {
      SetItemState(row, LVIS_SELECTED | LVIS_FOCUSED,
                   LVIS_SELECTED | LVIS_FOCUSED);
    }
  }
}

void EditableListCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
  if (nChar == VK_DELETE) {
    size_t selected = ULONG_MAX;

    POSITION pos = GetFirstSelectedItemPosition();
    if (pos != NULL) {
      selected = GetNextSelectedItem(pos);

      CWnd *parent = GetParent();
      LV_DISPINFO dispInfo;
      dispInfo.hdr.hwndFrom = parent->m_hWnd;
      dispInfo.item.lParam = CStringToUlong(Properties[selected][0].Value);

      AfxGetMainWnd()->SendMessage(WM_NOTIFY_PROPERTY_DELETED, NULL,
                                   (LPARAM)&dispInfo);
    }
  }

  CMFCListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

void EditableListCtrl::OnHScroll(UINT nSBCode, UINT nPos,
                                 CScrollBar *pScrollBar) {
  if (GetFocus() != this) {
    SetFocus();
  }
  CMFCListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}

void EditableListCtrl::OnVScroll(UINT nSBCode, UINT nPos,
                                 CScrollBar *pScrollBar) {
  if (GetFocus() != this) {
    SetFocus();
  }
  CMFCListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}

void EditableListCtrl::SetCellText(int row, int col, PropType type,
                                   CString text) {
  if (type == PropType::BRUSHSTYLE) {
    CString brushStr = DrawableProperty::GetStringByIdentifier(
        DrawableProperty::BrushStyles, CStringToUlong(text));
    SetItemText(row, col, brushStr);
  } else if (type == PropType::PENSTYLE) {
    CString penStr = DrawableProperty::GetStringByIdentifier(
        DrawableProperty::PenStyles, CStringToUlong(text));
    SetItemText(row, col, penStr);
  } else if (type == PropType::BOOL) {
    if (text == _T("1")) {
      SetItemText(row, col, TrueStr);
    } else {
      SetItemText(row, col, FalseStr);
    }
  } else if (type == PropType::COLOR) {
    SetItemText(row, col, ColorToRGBString(CStringToUlong(text)));
  } else {
    SetItemText(row, col, text);
  }
}

void EditableListCtrl::SetSelectionById(IDType id) {
  CString idStr;
  idStr.Format(_T("%d"), id);
  for (auto it = Properties.begin(); it != Properties.end(); ++it) {
    if ((*it)[0].Value == idStr) {
      int selected = it - Properties.begin();
      SetItemState(selected, LVIS_SELECTED, LVIS_SELECTED);
      SetSelectionMark(selected);
      break;
    }
  }
}

COLORREF EditableListCtrl::OnGetCellBkColor(int nRow, int nColumn) {
  if (Properties[nRow][nColumn].Type == PropType::COLOR) {
    return CStringToUlong(Properties[nRow][nColumn].Value);
  }

  return CMFCListCtrl::OnGetCellBkColor(nRow, nColumn);
}

COLORREF EditableListCtrl::OnGetCellTextColor(int nRow, int nColumn) {
  if (Properties[nRow][nColumn].Type == PropType::COLOR) {
    return CStringToUlong(Properties[nRow][nColumn].Value);
  }

  return CMFCListCtrl::OnGetCellTextColor(nRow, nColumn);
}

int EditableListCtrl::GetRowFromPoint(CPoint &point, int *col) const {
  int column = 0;
  int row = HitTest(point, NULL);

  if (col)
    *col = 0;

  // Make sure that the ListView is in LVS_REPORT
  if ((GetWindowLong(m_hWnd, GWL_STYLE) & LVS_TYPEMASK) != LVS_REPORT) {
    return row;
  }

  // Get the top and bottom row visible
  row = GetTopIndex();
  int bottom = row + GetCountPerPage();

  if (bottom > GetItemCount()) {
    bottom = GetItemCount();
  }

  // Get the number of columns
  CHeaderCtrl *header = (CHeaderCtrl *)GetDlgItem(0);
  int columnCount = header->GetItemCount();

  // Loop through the visible rows
  for (; row <= bottom; row++) {
    // Get bounding rectangle of item and check whether point falls in it.
    CRect rect;
    GetItemRect(row, &rect, LVIR_BOUNDS);

    if (rect.PtInRect(point)) {
      // Find the column
      for (column = 0; column < columnCount; column++) {
        int colWidth = GetColumnWidth(column);
        if (point.x >= rect.left && point.x <= (rect.left + colWidth)) {
          if (col) {
            *col = column;
          }
          return row;
        }
        rect.left += colWidth;
      }
    }
  }

  return -1;
}

void EditableListCtrl::CalculateCellRect(int row, int col, CRect &cellRect) {
  GetItemRect(row, &cellRect, LVIR_BOUNDS);

  CRect rcClient;
  GetClientRect(&rcClient);

  if (cellRect.right > rcClient.right) {
    cellRect.right = rcClient.right;
  }

  int columnWidth = GetColumnWidth(col);

  // Get the column iOffset
  int offSet = 0;
  for (int i = 0; i < col; i++) {
    offSet += GetColumnWidth(i);
  }

  // If x1 of cell rect is < x1 of ctrl rect or
  // If x1 of cell rect is > x1 of ctrl rect or **Should not ideally happen**
  // If the width of the cell extends beyond x2 of ctrl rect then
  // Scroll

  CSize obScrollSize(0, 0);

  if (((offSet + cellRect.left) < rcClient.left) ||
      ((offSet + cellRect.left) > rcClient.right)) {
    obScrollSize.cx = offSet + cellRect.left;
  } else if ((offSet + cellRect.left + columnWidth) > rcClient.right) {
    obScrollSize.cx = offSet + cellRect.left + columnWidth - rcClient.right;
  }

  Scroll(obScrollSize);
  cellRect.left -= obScrollSize.cx;

  // Set the width to the column width
  cellRect.left += offSet;
  cellRect.right = cellRect.left + columnWidth;
}

void EditableListCtrl::UpdateCellValue(IDType id, PropName name,
                                       CString value) {
  CString idStr;
  idStr.Format(_T("%d"), id);
  for (auto it = Properties.begin(); it != Properties.end(); ++it) {
    if ((*it)[0].Value == idStr) {
      for (auto prop = it->begin(); prop != it->end(); ++prop) {
        if (prop->Name == name) {
          prop->Value = value;
          int row = it - Properties.begin();
          SetCellText(row, prop - it->begin(), prop->Type, value);
          break;
        }
      }

      break;
    }
  }
}

IDType EditableListCtrl::GetSelectionID() {
  POSITION pos = GetFirstSelectedItemPosition();
  if (pos != NULL) {
    size_t selected = GetNextSelectedItem(pos);
    return CStringToUlong(Properties[selected][0].Value);
  } else {
    return ULONG_MAX;
  }
}
