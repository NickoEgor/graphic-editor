
// MainFrm.h : interface of the CMainFrame class
//

#pragma once
#include "PaintTool.h"
#include "PropertiesWnd.h"

class CMainFrame : public CFrameWndEx {

protected: // create from serialization only
  CMainFrame() noexcept;
  DECLARE_DYNCREATE(CMainFrame)

  // Attributes
public:
  // Operations
public:
  // Overrides
public:
  virtual BOOL PreCreateWindow(CREATESTRUCT &cs);
  virtual BOOL
  LoadFrame(UINT nIDResource,
            DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE,
            CWnd *pParentWnd = nullptr, CCreateContext *pContext = nullptr);

  // Implementation
public:
  virtual ~CMainFrame();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext &dc) const;
#endif

  void UpdateProperty(IDType oldID, IDType newID, PropName name, CString value);

private:
  PaintTool currentTool = PaintTool::RECTANGLE;

public:
  PaintTool GetCurrentTool();
  void OnToolChanged(UINT nID);
  void OnToolChangedUI(CCmdUI *pCmdUI);

  LRESULT OnNotifyPropertyEdited(WPARAM wParam, LPARAM lParam);
  LRESULT OnNotifyPropertyDeleted(WPARAM wParam, LPARAM lParam);
  LRESULT OnNotifySelectionChanged(WPARAM wParam, LPARAM lParam);

  void OnCurrentShapeChanged(IDType id);
  void OnDeleteCurrentShape();

  void UpdatePropertiesWindows();

protected: // control bar embedded members
  CMFCMenuBar m_wndMenuBar;
  CMFCToolBar m_wndToolBar;
  CMFCToolBar m_wndDrawToolBar;
  PropertiesWnd m_wndShapeProperties;
  PropertiesWnd m_wndLineProperties;

  // HACCEL m_hAccelTable;

  // Generated message map functions
protected:
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnViewCustomize();
  afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
  afx_msg void OnGetMinMaxInfo(MINMAXINFO *lpMMI);

  DECLARE_MESSAGE_MAP()

  BOOL CreateDockingWindows();
  void SetDockingWindowIcons(BOOL bHiColorIcons);

  void OnViewMainToolBar();
  void OnUpdateMainToolBar(CCmdUI *pCmdUI);
  void OnViewDrawToolBar();
  void OnUpdateDrawToolBar(CCmdUI *pCmdUI);
  void OnViewShapePane();
  void OnUpdateShapePane(CCmdUI *pCmdUI);
  void OnViewLinePane();
  void OnUpdateLinePane(CCmdUI *pCmdUI);
  void OnViewShowBorder();
  void OnUpdateShowBorder(CCmdUI *pCmdUI);
};
