
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "Vocker.h"
#include "stdafx.h"

#include "MainFrm.h"
#include "VockerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
ON_WM_CREATE()
ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
ON_COMMAND_RANGE(ID_RECTANGLE_NEW, ID_SHRINKSIZE, OnToolChanged)
ON_UPDATE_COMMAND_UI_RANGE(ID_RECTANGLE_NEW, ID_SHRINKSIZE, OnToolChangedUI)
ON_WM_GETMINMAXINFO()
ON_MESSAGE(WM_NOTIFY_PROPERTY_EDITED, OnNotifyPropertyEdited)
ON_MESSAGE(WM_NOTIFY_PROPERTY_DELETED, OnNotifyPropertyDeleted)
ON_MESSAGE(WM_NOTIFY_SELECTION_CHANGED, OnNotifySelectionChanged)
ON_COMMAND(ID_DELETECURRENT, &CMainFrame::OnDeleteCurrentShape)

ON_COMMAND(ID_VIEW_MAINTOOLBAR, OnViewMainToolBar)
ON_UPDATE_COMMAND_UI(ID_VIEW_MAINTOOLBAR, OnUpdateMainToolBar)
ON_COMMAND(ID_VIEW_DRAWTOOLBAR, OnViewDrawToolBar)
ON_UPDATE_COMMAND_UI(ID_VIEW_DRAWTOOLBAR, OnUpdateDrawToolBar)
ON_COMMAND(ID_VIEW_SHAPES, OnViewShapePane)
ON_UPDATE_COMMAND_UI(ID_VIEW_SHAPES, OnUpdateShapePane)
ON_COMMAND(ID_VIEW_LINES, OnViewLinePane)
ON_UPDATE_COMMAND_UI(ID_VIEW_LINES, OnUpdateLinePane)
ON_COMMAND(ID_VIEW_SHOWBORDER, OnViewShowBorder)
ON_UPDATE_COMMAND_UI(ID_VIEW_SHOWBORDER, OnUpdateShowBorder)
END_MESSAGE_MAP()

// CMainFrame construction/destruction

CMainFrame::CMainFrame() noexcept {
  // TODO: add member initialization code here
}

CMainFrame::~CMainFrame() {}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) {
  if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
    return -1;

  BOOL bNameValid;

  if (!m_wndMenuBar.Create(this)) {
    TRACE0("Failed to create menubar\n");
    return -1; // fail to create
  }

  m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC |
                            CBRS_TOOLTIPS | CBRS_FLYBY);
  m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() & ~CBRS_GRIPPER);

  // prevent the menu bar from taking the focus on activation
  CMFCPopupMenu::SetForceMenuFocus(FALSE);

  // main toolbar
  CMFCToolBarInfo tbi1;
  if (!m_wndToolBar.CreateEx(this, TBSTYLE_TRANSPARENT,
                             WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_TOOLTIPS |
                                 CBRS_FLYBY | CBRS_SIZE_DYNAMIC,
                             CRect(1, 1, 1, 1), ID_VIEW_MAINTOOLBAR) ||
      //! m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
      !m_wndToolBar.LoadToolBarEx(IDR_MAINFRAME, tbi1, TRUE)) {
    TRACE0("Failed to create toolbar\n");
    return -1; // fail to create
  }

  CString strToolBarName;
  bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_STANDARD);
  ASSERT(bNameValid);
  m_wndToolBar.SetWindowText(strToolBarName);

  m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~CBRS_GRIPPER);

  // painting toolbar
  CMFCToolBarInfo tbi2;
  if (!m_wndDrawToolBar.CreateEx(this, TBSTYLE_TRANSPARENT,
                                 WS_CHILD | WS_VISIBLE | CBRS_LEFT |
                                     CBRS_TOOLTIPS | CBRS_FLYBY |
                                     CBRS_SIZE_DYNAMIC,
                                 CRect(1, 1, 1, 1), ID_VIEW_DRAWTOOLBAR) ||
      !m_wndDrawToolBar.LoadToolBarEx(IDR_DRAWTOOLBAR, tbi2, TRUE)) {
    TRACE0("Failed to create toolbar\n");
    return -1; // fail to create
  }

  bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_DRAWING);
  ASSERT(bNameValid);
  m_wndDrawToolBar.SetWindowText(strToolBarName);

  m_wndDrawToolBar.SetPaneStyle(m_wndDrawToolBar.GetPaneStyle() &
                                ~CBRS_GRIPPER);

  m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
  m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
  m_wndDrawToolBar.EnableDocking(CBRS_ALIGN_LEFT);
  EnableDocking(CBRS_ALIGN_ANY);
  DockPane(&m_wndMenuBar);
  DockPane(&m_wndToolBar);
  DockPane(&m_wndDrawToolBar);
  m_wndDrawToolBar.DockToFrameWindow(CBRS_LEFT);

  // enable Visual Studio 2005 style docking window behavior
  CDockingManager::SetDockingMode(DT_SMART);
  // enable Visual Studio 2005 style docking window auto-hide behavior
  EnableAutoHidePanes(CBRS_ALIGN_ANY);

  // create docking windows
  if (!CreateDockingWindows()) {
    TRACE0("Failed to create docking windows\n");
    return -1;
  }

  m_wndLineProperties.EnableDocking(CBRS_ALIGN_ANY);
  m_wndShapeProperties.EnableDocking(CBRS_ALIGN_ANY);

  DockPane(&m_wndLineProperties);
  DockPane(&m_wndShapeProperties);

  m_wndShapeProperties.SetAutoHideMode(TRUE, CBRS_RIGHT);
  m_wndLineProperties.SetAutoHideMode(TRUE, CBRS_RIGHT);

  m_wndShapeProperties.SetHeaders(DrawableProperty::GetShapeNames());
  m_wndLineProperties.SetHeaders(DrawableProperty::GetLineNames());

  // set the visual manager used to draw all user interface elements
  CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));

  // Enable toolbar and docking window menu replacement
  // EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

  // enable quick (Alt+drag) toolbar customization
  CMFCToolBar::EnableQuickCustomization();

  return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT &cs) {
  if (!CFrameWndEx::PreCreateWindow(cs))
    return FALSE;
  // TODO: Modify the Window class or styles here by modifying
  //  the CREATESTRUCT cs

  return TRUE;
}

BOOL CMainFrame::CreateDockingWindows() {
  BOOL bNameValid;

  CString strShapesWnd;
  bNameValid = strShapesWnd.LoadString(IDS_SHAPEPROPERTIES_WND);
  ASSERT(bNameValid);
  if (!m_wndShapeProperties.Create(
          strShapesWnd, this, CRect(0, 0, 600, 200), TRUE, ID_VIEW_SHAPESWND,
          WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
              CBRS_RIGHT | CBRS_FLOAT_MULTI)) {
    TRACE0("Failed to create Shapes properties window\n");
    return FALSE; // failed to create
  }

  CString strLinesWnd;
  bNameValid = strLinesWnd.LoadString(IDS_LINEPROPERTIES_WND);
  ASSERT(bNameValid);
  if (!m_wndLineProperties.Create(
          strLinesWnd, this, CRect(0, 0, 600, 200), TRUE, ID_VIEW_LINESWND,
          WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
              CBRS_RIGHT | CBRS_FLOAT_MULTI)) {
    TRACE0("Failed to create Lines properties window\n");
    return FALSE; // failed to create
  }

  SetDockingWindowIcons(theApp.m_bHiColorIcons);
  return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons) {
  HICON hPropertiesBarIcon =
      (HICON)::LoadImage(::AfxGetResourceHandle(),
                         MAKEINTRESOURCE(bHiColorIcons ? IDI_PROPERTIES_WND_HC
                                                       : IDI_PROPERTIES_WND),
                         IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON),
                         ::GetSystemMetrics(SM_CYSMICON), 0);
  m_wndShapeProperties.SetIcon(hPropertiesBarIcon, FALSE);
  m_wndLineProperties.SetIcon(hPropertiesBarIcon, FALSE);
}

void CMainFrame::OnViewMainToolBar() {
  bool isVisible = m_wndToolBar.IsWindowVisible();
  if (isVisible) {
    m_wndToolBar.ShowWindow(SW_HIDE);
  } else {
    m_wndToolBar.ShowWindow(SW_SHOW);
  }
}

void CMainFrame::OnUpdateMainToolBar(CCmdUI *pCmdUI) {
  pCmdUI->SetCheck(m_wndToolBar.IsWindowVisible());
}

void CMainFrame::OnViewDrawToolBar() {
  bool isVisible = m_wndDrawToolBar.IsWindowVisible();
  if (isVisible) {
    m_wndDrawToolBar.ShowWindow(SW_HIDE);
  } else {
    m_wndDrawToolBar.ShowWindow(SW_SHOW);
  }
}

void CMainFrame::OnUpdateDrawToolBar(CCmdUI *pCmdUI) {
  pCmdUI->SetCheck(m_wndDrawToolBar.IsWindowVisible());
}

void CMainFrame::OnViewShapePane() {
  bool isVisible = m_wndShapeProperties.IsPaneVisible();
  if (isVisible) {
    m_wndShapeProperties.ShowPane(FALSE, FALSE, FALSE);
  } else {
    m_wndShapeProperties.ShowPane(TRUE, FALSE, TRUE);
  }
}

void CMainFrame::OnUpdateShapePane(CCmdUI *pCmdUI) {
  pCmdUI->SetCheck(m_wndShapeProperties.IsPaneVisible());
}

void CMainFrame::OnViewLinePane() {
  bool isVisible = m_wndLineProperties.IsPaneVisible();
  if (isVisible) {
    m_wndLineProperties.ShowPane(FALSE, FALSE, FALSE);
  } else {
    m_wndLineProperties.ShowPane(TRUE, FALSE, TRUE);
  }
}

void CMainFrame::OnUpdateLinePane(CCmdUI *pCmdUI) {
  pCmdUI->SetCheck(m_wndLineProperties.IsPaneVisible());
}

void CMainFrame::OnViewShowBorder() {
  CVockerDoc *document = dynamic_cast<CVockerDoc *>(GetActiveDocument());
  if (document == nullptr)
    return;

  POSITION pos = document->GetFirstViewPosition();
  CVockerView *view = dynamic_cast<CVockerView *>(document->GetNextView(pos));
  view->IsShowingBorder = !(view->IsShowingBorder);
  view->Invalidate();
  view->UpdateWindow();
}

void CMainFrame::OnUpdateShowBorder(CCmdUI *pCmdUI) {
  CVockerDoc *document = dynamic_cast<CVockerDoc *>(GetActiveDocument());
  if (document == nullptr)
    return;

  POSITION pos = document->GetFirstViewPosition();
  CVockerView *view = dynamic_cast<CVockerView *>(document->GetNextView(pos));
  pCmdUI->SetCheck(view->IsShowingBorder);
}

PaintTool CMainFrame::GetCurrentTool() { return currentTool; }

void CMainFrame::OnToolChanged(UINT nID) {
  PaintTool newTool = static_cast<PaintTool>(nID);
  CVockerView *view = static_cast<CVockerView *>(GetActiveView());

  if (nID == ID_PEN_PICKER) {
    view->ShowPenPickerDialog();
  } else if (nID == ID_BRUSH_PICKER) {
    view->ShowBrushPickerDialog();
  } else if (nID == ID_SHRINKSIZE) {
    view->ShrinkViewSize();
    view->Invalidate();
    view->UpdateWindow();
  } else if (newTool != currentTool) {
    int index = m_wndDrawToolBar.CommandToIndex(nID);
    currentTool = newTool;
    view->OnToolChanged();
  }
}

void CMainFrame::OnToolChangedUI(CCmdUI *pCmdUI) {
  pCmdUI->SetCheck(pCmdUI->m_nID == static_cast<UINT>(currentTool));
}

LRESULT CMainFrame::OnNotifyPropertyEdited(WPARAM wParam, LPARAM lParam) {
  CVockerDoc *document = dynamic_cast<CVockerDoc *>(GetActiveDocument());
  if (document == nullptr)
    return -1;

  LV_DISPINFO *dispInfo = reinterpret_cast<LV_DISPINFO *>(lParam);
  PropName name = static_cast<PropName>(dispInfo->item.iItem);
  IDType id = dispInfo->item.lParam;

  try {
    DrawablePtr drawable = document->GetDrawable(id);
    if (drawable == nullptr) {
      throw std::exception("Drawable can't be null");
    }

    CString value = dispInfo->item.pszText;
    drawable->SetProperty(name, value);
    IDType newID = drawable->GetId();
    document->UpdateDocument(false);
    UpdateProperty(id, newID, name, value);
  } catch (const std::out_of_range &) {
    MessageBox(_T("Argument out of range"), _T("Error"), MB_OK | MB_ICONERROR);
  } catch (const std::exception &e) {
    MessageBox(CString(e.what()), _T("Error"), MB_OK | MB_ICONERROR);
  }

  return 0;
}

LRESULT CMainFrame::OnNotifyPropertyDeleted(WPARAM wParam, LPARAM lParam) {
  CVockerDoc *document = dynamic_cast<CVockerDoc *>(GetActiveDocument());
  if (document == nullptr)
    return -1;

  LV_DISPINFO *dispInfo = reinterpret_cast<LV_DISPINFO *>(lParam);
  IDType id = dispInfo->item.lParam;

  try {
    document->DeleteDrawable(id);
  } catch (const std::exception &e) {
    MessageBox(CString(e.what()), _T("Error"), MB_OK | MB_ICONERROR);
  }

  return 0;
}

LRESULT CMainFrame::OnNotifySelectionChanged(WPARAM wParam, LPARAM lParam) {
  CVockerDoc *document = dynamic_cast<CVockerDoc *>(GetActiveDocument());
  if (document == nullptr)
    return -1;

  LV_DISPINFO *dispInfo = reinterpret_cast<LV_DISPINFO *>(lParam);
  IDType id = dispInfo->item.lParam;

  POSITION pos = document->GetFirstViewPosition();
  CVockerView *view = dynamic_cast<CVockerView *>(document->GetNextView(pos));
  if (view != nullptr) {
    view->SetCurrentShape(id);
    view->Invalidate();
    view->UpdateWindow();
  }

  return 0;
}

void CMainFrame::OnCurrentShapeChanged(IDType id) {
  m_wndShapeProperties.OnCurrentSelectionChange(id);
}

void CMainFrame::OnDeleteCurrentShape() {
  CVockerDoc *document = dynamic_cast<CVockerDoc *>(GetActiveDocument());
  if (document == nullptr)
    return;

  CVockerView *view = static_cast<CVockerView *>(GetActiveView());

  try {
    if (view->CurrentShape != nullptr) {
      auto shape = view->CurrentShape;
      view->CurrentShape = nullptr;
      document->DeleteShape(shape);
    }
  } catch (const std::exception &e) {
    MessageBox(CString(e.what()), _T("Error"), MB_OK | MB_ICONERROR);
  }
}

void CMainFrame::UpdatePropertiesWindows() {
  CVockerDoc *document = dynamic_cast<CVockerDoc *>(GetActiveDocument());
  if (document == nullptr)
    return;

  std::vector<std::vector<DrawableProperty>> properties;

  auto &shapes = document->GetShapes();
  for (auto &shape : shapes) {
    properties.push_back(shape->RShape::GetProperties());
  }
  m_wndShapeProperties.UpdateValues(properties);

  properties.clear();
  auto &lines = document->GetLines();
  for (auto &line : lines) {
    properties.push_back(line->GetProperties());
  }
  m_wndLineProperties.UpdateValues(properties);
}

void CMainFrame::UpdateProperty(IDType oldID, IDType newID, PropName name,
                                CString value) {
  CVockerDoc *document = dynamic_cast<CVockerDoc *>(GetActiveDocument());
  if (document == nullptr)
    return;

  try {
    auto drawable = document->GetDrawable(newID);
    if (drawable == nullptr) {
      throw std::exception("Drawable can't be null");
    }

    if (std::dynamic_pointer_cast<RShape>(drawable)) {
      m_wndShapeProperties.UpdateValue(oldID, name, value);
    } else {
      m_wndLineProperties.UpdateValue(oldID, name, value);
    }
  } catch (const std::exception &e) {
    MessageBox(CString(e.what()), _T("Error"), MB_OK | MB_ICONERROR);
  }
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const { CFrameWndEx::AssertValid(); }

void CMainFrame::Dump(CDumpContext &dc) const { CFrameWndEx::Dump(dc); }
#endif //_DEBUG

// CMainFrame message handlers

void CMainFrame::OnViewCustomize() {
  CMFCToolBarsCustomizeDialog *pDlgCust =
      new CMFCToolBarsCustomizeDialog(this, TRUE /* scan menus */);
  pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp, LPARAM lp) {
  LRESULT lres = CFrameWndEx::OnToolbarCreateNew(wp, lp);
  if (lres == 0) {
    return 0;
  }

  CMFCToolBar *pUserToolbar = (CMFCToolBar *)lres;
  ASSERT_VALID(pUserToolbar);

  BOOL bNameValid;
  CString strCustomize;
  bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
  ASSERT(bNameValid);

  pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
  return lres;
}

void CMainFrame::OnGetMinMaxInfo(MINMAXINFO *lpMMI) {
  lpMMI->ptMinTrackSize.x = 400;
  lpMMI->ptMinTrackSize.y = 300;
}

BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle,
                           CWnd *pParentWnd, CCreateContext *pContext) {
  // base class does the real work

  if (!CFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd,
                              pContext)) {
    return FALSE;
  }

  return TRUE;
}
