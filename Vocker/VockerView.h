
// VockerView.h : interface of the CVockerView class
//

#pragma once

#include <memory>
#include <vector>

#include "MainFrm.h"
#include "Marker.h"
#include "PaintTool.h"
#include "RShape.h"
#include "VockerDoc.h"

class CVockerView : public CScrollView {
protected:
  CVockerView() noexcept;
  DECLARE_DYNCREATE(CVockerView)

public:
  CVockerDoc *GetDocument() const;

  virtual void OnDraw(CDC *pDC);
  virtual BOOL PreCreateWindow(CREATESTRUCT &cs);
  virtual BOOL OnEraseBkgnd(CDC *pDC);

private:
  static CPen BorderPen;
  void DrawBorder(CDC *pDC, CVockerDoc *pDoc);
  void DrawDrawables(CDC *pDC, CVockerDoc *pDoc);
  void DrawTempDrawable(CDC *pDC, CVockerDoc *pDoc);

  CMenu ContextMenus;
  void ShowShapeContextMenu(CPoint point, ShapePtr shape);
  void ShowLineContextMenu(CPoint point, LinePtr line);

  PaintTool GetCurrentTool() const;
  CMainFrame *GetMainWindow();

  void DragMouse(CPoint point);
  int GetMarkerNumber(CPoint point);

  CRect CalculateShapesRect();

  void NotifyCurrentShapeChanged();

  enum class Mode { NONE, SHAPE, LINE, MOVING, ROTATING, RESIZING };

  Mode CurrentMode = Mode::NONE;

public:
  ShapePtr CurrentShape;

  bool IsShowingBorder = false;

  void ShowPenPickerDialog();
  void ShowBrushPickerDialog();

  void SetCurrentShape(IDType id);

  void UpdateViewSize();
  void ShrinkViewSize();

  void OnToolChanged();

  virtual ~CVockerView();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext &dc) const;
#endif

protected:
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);

  afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

  afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
  afx_msg void OnContextMenu(CWnd *pWnd, CPoint point);

  afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
  afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
  afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

  DECLARE_MESSAGE_MAP()

private:
  static const int InvalidCoordinate = -1;
  static const int InvalidShapeNumber = -1;
  static const int InvalidMarkerNumber = -1;
  static const CPoint InvalidPoint;
  static const CRect InvalidRect;

  static const CPoint EmptyPoint;
  static const CRect EmptyRect;

  static const int BorderIndent = 30;
  static const int ScrollStep = 10;

  CPoint TempPoint = InvalidPoint;
  CRect TempRect = InvalidRect;
  std::vector<Marker> TempMarkers;
  int TempMarkerNumber = InvalidMarkerNumber;
};

#ifndef _DEBUG // debug version in VockerView.cpp
inline CVockerDoc *CVockerView::GetDocument() const {
  return reinterpret_cast<CVockerDoc *>(m_pDocument);
}
#endif
