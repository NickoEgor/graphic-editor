
#include "stdafx.h"

#include "MainFrm.h"
#include "PropertiesWnd.h"
#include "Resource.h"
#include "Vocker.h"
#include "VockerView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar

PropertiesWnd::PropertiesWnd() noexcept {
  // m_bDisableAnimation = TRUE;
}

PropertiesWnd::~PropertiesWnd() {}

BEGIN_MESSAGE_MAP(PropertiesWnd, CDockablePane)
ON_WM_CREATE()
ON_WM_SIZE()
ON_WM_SETFOCUS()
ON_WM_SETTINGCHANGE()

ON_NOTIFY(LVN_ITEMCHANGED, ID_PROPLIST, OnItemChanged)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar message handlers

void PropertiesWnd::AdjustLayout() {
  if (GetSafeHwnd() == nullptr ||
      (AfxGetMainWnd() != nullptr && AfxGetMainWnd()->IsIconic())) {
    return;
  }

  CRect rectClient;
  GetClientRect(rectClient);

  m_wndEditList.SetWindowPos(nullptr, rectClient.left, rectClient.top,
                             rectClient.Width(), rectClient.Height(),
                             SWP_NOACTIVATE | SWP_NOZORDER);
}

void PropertiesWnd::OnCurrentSelectionChange(IDType id) {
  m_wndEditList.SetSelectionById(id);
}

int PropertiesWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) {
  if (CDockablePane::OnCreate(lpCreateStruct) == -1)
    return -1;

  CRect rectDummy;
  rectDummy.SetRectEmpty();

  if (!m_wndEditList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this,
                            ID_PROPLIST)) {
    TRACE0("Failed to create Properties List \n");
    return -1; // fail to create
  }

  m_wndEditList.SetExtendedStyle(m_wndEditList.GetExtendedStyle() |
                                 LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

  AdjustLayout();
  return 0;
}

void PropertiesWnd::OnSize(UINT nType, int cx, int cy) {
  CDockablePane::OnSize(nType, cx, cy);
  AdjustLayout();
}

void PropertiesWnd::OnSetFocus(CWnd *pOldWnd) {
  CDockablePane::OnSetFocus(pOldWnd);
  m_wndEditList.SetFocus();
}

bool PropertiesWnd::CheckViewPtr() {
  if (View == nullptr) {
    CView *view = static_cast<CMainFrame *>(AfxGetMainWnd())->GetActiveView();
    CVockerView *vockerView = dynamic_cast<CVockerView *>(view);
    if (vockerView != nullptr) {
      View = vockerView;
    }
  }

  return View != nullptr;
}

void PropertiesWnd::OnSlide(BOOL bSlideOut) {
  if (CheckViewPtr()) {
    CDockablePane::OnSlide(bSlideOut);
    View->Invalidate();
    View->UpdateWindow();
  }
}

BOOL PropertiesWnd::CheckStopSlideCondition(BOOL bDirection) {
  BOOL stopping = CDockablePane::CheckStopSlideCondition(bDirection);
  if (stopping) {
    AfxGetMainWnd()->Invalidate();
    AfxGetMainWnd()->UpdateWindow();
  }
  return stopping;
}

void PropertiesWnd::SetHeaders(std::vector<PropName> propNames) {
  for (size_t i = 0; i < propNames.size(); ++i) {
    m_wndEditList.InsertColumn(
        i, DrawableProperty::GetPropertyStringName(propNames[i]));
    PropNameToColumn[propNames[i]] = i;
  }

  for (size_t i = 0; i < propNames.size(); ++i) {
    m_wndEditList.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
  }
}

void PropertiesWnd::UpdateValues(
    std::vector<std::vector<DrawableProperty>> properties) {
  m_wndEditList.InitializeList(properties);
}

void PropertiesWnd::UpdateValue(IDType id, PropName name, CString value) {
  m_wndEditList.UpdateCellValue(id, name, value);
}

void PropertiesWnd::OnItemChanged(NMHDR *pNMHDR, LRESULT *pResult) {
  NM_LISTVIEW *pNMListView = (NM_LISTVIEW *)pNMHDR;

  if ((pNMListView->uChanged & LVIF_STATE) &&
      (pNMListView->uNewState & LVIS_SELECTED)) {
    size_t id = m_wndEditList.GetSelectionID();
    if (id != ULONG_MAX) {
      CWnd *parent = GetParent();
      LV_DISPINFO dispInfo;
      dispInfo.hdr.hwndFrom = parent->m_hWnd;
      dispInfo.item.lParam = id;

      AfxGetMainWnd()->SendMessage(WM_NOTIFY_SELECTION_CHANGED, NULL,
                                   (LPARAM)&dispInfo);
    }
  }
}
