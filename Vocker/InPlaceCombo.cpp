#include "InPlaceCombo.h"
#include "DrawableProperty.h"
#include "stdafx.h"

BEGIN_MESSAGE_MAP(InPlaceCombo, CComboBox)
ON_WM_CHAR()
ON_WM_CREATE()
ON_WM_KILLFOCUS()
ON_WM_NCDESTROY()
END_MESSAGE_MAP()

InPlaceCombo::InPlaceCombo(int row, int col, IDType id,
                           DrawableProperty property,
                           CStringList &comboItemsList, int selectedItem)
    : Row(row), Col(col), Id(id), Property(property), IsEsc(FALSE),
      SelectedItem(selectedItem) {
  DropDownList.AddTail(&comboItemsList);
}

BOOL InPlaceCombo::PreTranslateMessage(MSG *pMsg) {
  if (pMsg->message == WM_KEYDOWN) {
    if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE) {
      ::TranslateMessage(pMsg);
      ::DispatchMessage(pMsg);
      return TRUE;
    }
  }

  return CComboBox::PreTranslateMessage(pMsg);
}

int InPlaceCombo::OnCreate(LPCREATESTRUCT lpCreateStruct) {
  if (CComboBox::OnCreate(lpCreateStruct) == -1) {
    return -1;
  }

  // Set the proper font
  CFont *pFont = GetParent()->GetFont();
  SetFont(pFont);

  SetFocus();

  ResetContent();
  for (POSITION Pos_ = DropDownList.GetHeadPosition(); Pos_ != NULL;) {
    AddString((LPCTSTR)(DropDownList.GetNext(Pos_)));
  }

  SetCurSel(SelectedItem);
  GetWindowText(InitialString);

  return 0;
}

void InPlaceCombo::OnKillFocus(CWnd *pNewWnd) {
  CComboBox::OnKillFocus(pNewWnd);

  // Get the current selection text
  CString str;
  GetWindowText(str);

  if (Property.Type == PropType::BRUSHSTYLE) {
    auto identifier = DrawableProperty::GetIdentifierByString(
        DrawableProperty::BrushStyles, str);
    str.Format(_T("%d"), identifier);
  } else if (Property.Type == PropType::PENSTYLE) {
    auto identifier = DrawableProperty::GetIdentifierByString(
        DrawableProperty::PenStyles, str);
    str.Format(_T("%d"), identifier);
  }

  CWnd *parent = GetParent();

  // Send Notification to parent of ListView ctrl
  LV_DISPINFO dispInfo;
  dispInfo.hdr.hwndFrom = parent->m_hWnd;
  dispInfo.hdr.idFrom = GetDlgCtrlID();
  dispInfo.hdr.code = LVN_ENDLABELEDIT;

  dispInfo.item.mask = LVIF_TEXT;
  // dispInfo.item.iItem = Row;
  // dispInfo.item.iSubItem = Col;
  dispInfo.item.pszText =
      IsEsc ? LPTSTR((LPCTSTR)InitialString) : LPTSTR((LPCTSTR)str);
  dispInfo.item.cchTextMax =
      IsEsc ? InitialString.GetLength() : str.GetLength();
  dispInfo.item.lParam = static_cast<LPARAM>(Property.Name);
  dispInfo.item.iItem = static_cast<int>(Property.Name);
  dispInfo.item.lParam = Id;

  AfxGetMainWnd()->SendMessage(WM_NOTIFY_PROPERTY_EDITED, NULL,
                               (LPARAM)&dispInfo);
  parent->SetFocus();

  PostMessage(WM_CLOSE);
}

void InPlaceCombo::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) {
  // If the key is "Esc" set focus back to the list control
  if (nChar == VK_ESCAPE || nChar == VK_RETURN) {
    if (nChar == VK_ESCAPE) {
      IsEsc = TRUE;
    }

    GetParent()->SetFocus();
    return;
  }

  CComboBox::OnChar(nChar, nRepCnt, nFlags);
}

void InPlaceCombo::OnNcDestroy() {
  CComboBox::OnNcDestroy();
  delete this;
}
