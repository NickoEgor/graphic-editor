#include "InPlaceEdit.h"
#include "DrawableProperty.h"
#include "stdafx.h"
#include <cctype>

BEGIN_MESSAGE_MAP(InPlaceEdit, CEdit)
ON_WM_CHAR()
ON_WM_CREATE()
ON_WM_KILLFOCUS()
ON_WM_NCDESTROY()
END_MESSAGE_MAP()

InPlaceEdit::InPlaceEdit(int row, int col, IDType id, DrawableProperty property,
                         CString initText)
    : InitText(initText), Row(row), Col(col), Id(id), Property(property),
      IsEsc(FALSE) {}

BOOL InPlaceEdit::PreTranslateMessage(MSG *pMsg) {
  if (pMsg->message == WM_KEYDOWN) {
    if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_DELETE ||
        pMsg->wParam == VK_ESCAPE || GetKeyState(VK_CONTROL)) {
      ::TranslateMessage(pMsg);
      ::DispatchMessage(pMsg);
      return TRUE;
    }
  }

  return CEdit::PreTranslateMessage(pMsg);
}

int InPlaceEdit::OnCreate(LPCREATESTRUCT lpCreateStruct) {
  if (CEdit::OnCreate(lpCreateStruct) == -1) {
    return -1;
  }

  // Set the proper font
  CFont *font = GetParent()->GetFont();
  SetFont(font);

  SetFocus();

  SetWindowText(InitText);
  SetSel(0, -1);

  if (Property.Type == PropType::UINT || Property.Type == PropType::DOUBLE) {
    this->ModifyStyle(0, ES_NUMBER);
  }

  return 0;
}

void InPlaceEdit::OnKillFocus(CWnd *pNewWnd) {
  CEdit::OnKillFocus(pNewWnd);
  CString str;
  GetWindowText(str);

  DestroyWindow();
}

void InPlaceEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) {
  if (nChar == VK_ESCAPE) {
    IsEsc = TRUE;
    GetParent()->SetFocus();
    return;
  }

  CString prevStr;
  GetWindowText(prevStr);

  // to get one dot for double values
  if (Property.Type == PropType::DOUBLE && WCHAR(nChar) == L'.') {
    bool hasDot = false;
    for (int i = 0; i < prevStr.GetLength(); ++i) {
      if (prevStr[i] == L'.') {
        hasDot = true;
        break;
      }
    }

    if (!hasDot) {
      this->ModifyStyle(ES_NUMBER, 0);
      CEdit::OnChar(nChar, nRepCnt, nFlags);
      this->ModifyStyle(0, ES_NUMBER);
    } else {
      CEdit::OnChar(nChar, nRepCnt, nFlags);
    }
  }
  if (Property.Type == PropType::DOUBLE && WCHAR(nChar) == L'-') {
    if (prevStr.GetLength() == 0) {
      this->ModifyStyle(ES_NUMBER, 0);
      CEdit::OnChar(nChar, nRepCnt, nFlags);
      this->ModifyStyle(0, ES_NUMBER);
    } else {
      CEdit::OnChar(nChar, nRepCnt, nFlags);
    }
  } else {
    CEdit::OnChar(nChar, nRepCnt, nFlags);
  }

  // Resize edit control if needed
  CString str;
  GetWindowText(str);

  // if (nChar == VK_RETURN && str.GetLength() == 0)
  //{
  //    GetParent()->SetFocus();
  //    return;
  //}

  CWindowDC dc(this);
  CFont *pFont = GetParent()->GetFont();
  CFont *pFontDC = dc.SelectObject(pFont);
  CSize size = dc.GetTextExtent(str);
  dc.SelectObject(pFontDC);
  size.cx += 5;

  // Get the client rectangle
  CRect rect, parentrect;
  GetClientRect(&rect);
  GetParent()->GetClientRect(&parentrect);

  // Transform rectangle to parent coordinates
  ClientToScreen(&rect);
  GetParent()->ScreenToClient(&rect);

  // Check whether control needs resizing and if sufficient space to grow
  if (size.cx > rect.Width()) {
    if (size.cx + rect.left < parentrect.right) {
      rect.right = rect.left + size.cx;
    } else {
      rect.right = parentrect.right;
    }

    MoveWindow(&rect);
  }

  CWnd *parent = GetParent();

  // Construct list control item data
  LV_DISPINFO dispInfo;
  dispInfo.hdr.hwndFrom = parent->m_hWnd;
  dispInfo.hdr.idFrom = GetDlgCtrlID();
  dispInfo.hdr.code = LVN_ENDLABELEDIT;
  dispInfo.item.mask = LVIF_TEXT;
  // dispInfo.item.iItem = Row;
  // dispInfo.item.iSubItem = Col;
  dispInfo.item.pszText = IsEsc ? NULL : LPTSTR((LPCTSTR)str);
  dispInfo.item.cchTextMax = str.GetLength();
  dispInfo.item.iItem = static_cast<int>(Property.Name);
  dispInfo.item.lParam = Id;

  // Send this Notification to parent of ListView ctrl
  CWnd *pWndViewAttachmentsDlg = parent->GetParent();

  if (pWndViewAttachmentsDlg && nChar == VK_RETURN) {
    AfxGetMainWnd()->SendMessage(WM_NOTIFY_PROPERTY_EDITED, NULL,
                                 (LPARAM)&dispInfo);
    parent->SetFocus();
  }
}

void InPlaceEdit::OnNcDestroy() {
  CEdit::OnNcDestroy();
  delete this;
}
