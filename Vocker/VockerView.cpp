
// VockerView.cpp : implementation of the CVockerView class
//

#include "Vocker.h"
#include "stdafx.h"

#include "MainFrm.h"
#include "VockerDoc.h"
#include "VockerView.h"

#include "BrushPickerDialog.h"
#include "PenPickerDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CVockerView

IMPLEMENT_DYNCREATE(CVockerView, CScrollView)

BEGIN_MESSAGE_MAP(CVockerView, CScrollView)
ON_WM_LBUTTONDOWN()
ON_WM_LBUTTONUP()
ON_WM_MOUSEMOVE()

ON_WM_LBUTTONDBLCLK()

ON_WM_CONTEXTMENU()
ON_WM_RBUTTONUP()

ON_WM_MOUSEWHEEL()
ON_WM_VSCROLL()
ON_WM_HSCROLL()
ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

const CPoint CVockerView::InvalidPoint = {CVockerView::InvalidCoordinate,
                                          CVockerView::InvalidCoordinate};

const CRect CVockerView::InvalidRect = {CVockerView::InvalidPoint,
                                        CVockerView::InvalidPoint};

const CPoint CVockerView::EmptyPoint = {0, 0};

const CRect CVockerView::EmptyRect = {CVockerView::EmptyPoint,
                                      CVockerView::EmptyPoint};

// CVockerView construction/destruction

CVockerView::CVockerView() noexcept {
  ContextMenus.LoadMenuW(IDR_VIEW_CONTEXTMENU);
  SetScrollSizes(MM_TEXT, {0, 0});
}

void CVockerView::OnToolChanged() {
  Invalidate();
  UpdateWindow();
}

CVockerView::~CVockerView() {}

BOOL CVockerView::PreCreateWindow(CREATESTRUCT &cs) {
  // TODO: Modify the Window class or styles here by modifying
  //  the CREATESTRUCT cs

  return CScrollView::PreCreateWindow(cs);
}

PaintTool CVockerView::GetCurrentTool() const {
  return static_cast<CMainFrame *>(AfxGetMainWnd())->GetCurrentTool();
}

CMainFrame *CVockerView::GetMainWindow() {
  return static_cast<CMainFrame *>(AfxGetMainWnd());
}

int CVockerView::GetMarkerNumber(CPoint point) {
  for (size_t i = 0; i < TempMarkers.size(); ++i) {
    if (TempMarkers[i].IsPointInMarker(point))
      return i;
  }

  return InvalidMarkerNumber;
}

// View resizing

CRect CVockerView::CalculateShapesRect() {
  CVockerDoc *pDoc = GetDocument();
  if (!pDoc)
    return InvalidRect;

  CPoint topLeftCorner = {INT_MAX, INT_MAX};
  CPoint bottomRightCorner{-1, -1};

  for (auto &shape : pDoc->GetShapes()) {
    auto corners = shape->GetCornerPoints();
    DoublePoint center = shape->GetCenter();

    for (auto &corner : corners) {
      if (corner.x > bottomRightCorner.x) {
        bottomRightCorner.x = static_cast<LONG>(ceil(corner.x + 1));
      }

      if (corner.y > bottomRightCorner.y) {
        bottomRightCorner.y = static_cast<LONG>(ceil(corner.y + 1));
      }

      if (corner.x < topLeftCorner.x) {
        topLeftCorner.x = static_cast<LONG>(floor(corner.x));
      }

      if (corner.y < topLeftCorner.y) {
        topLeftCorner.y = static_cast<LONG>(floor(corner.y));
      }
    }
  }

  return {topLeftCorner, bottomRightCorner};
}

void CVockerView::NotifyCurrentShapeChanged() {
  if (CurrentShape != nullptr) {
    static_cast<CMainFrame *>(AfxGetMainWnd())
        ->OnCurrentShapeChanged(CurrentShape->GetId());
  }
}

void CVockerView::UpdateViewSize() {
  CVockerDoc *pDoc = GetDocument();
  if (!pDoc)
    return;

  CRect minRect = CalculateShapesRect();
  CPoint viewSize = pDoc->ViewSize;

  CPoint shift = {-min(0, minRect.left), -min(0, minRect.top)};

  for (auto &shape : pDoc->GetShapes()) {
    shape->Move(shift);
  }

  minRect.BottomRight() += shift;
  viewSize += shift;

  pDoc->ViewSize = {max(minRect.right, viewSize.x),
                    max(minRect.bottom, viewSize.y)};

  SetScrollSizes(MM_TEXT, CSize(pDoc->ViewSize));
}

void CVockerView::ShrinkViewSize() {
  CVockerDoc *pDoc = GetDocument();
  if (!pDoc)
    return;

  CRect minRect = CalculateShapesRect();
  CPoint size = {minRect.Width(), minRect.Height()};
  CPoint shift = {-minRect.left, -minRect.top};

  for (auto &shape : pDoc->GetShapes()) {
    shape->Move(shift);
  }

  pDoc->ViewSize = size;

  SetScrollSizes(MM_TEXT, CSize(pDoc->ViewSize));
}

// Dialogs

void CVockerView::ShowPenPickerDialog() {
  CVockerDoc *pDoc = GetDocument();
  if (!pDoc)
    return;

  PenPickerDialog dialog(this, &pDoc->CurrentPen);
  dialog.DoModal();
}

void CVockerView::ShowBrushPickerDialog() {
  CVockerDoc *pDoc = GetDocument();
  if (!pDoc)
    return;

  BrushPickerDialog dialog(this, &pDoc->CurrentBrush);
  dialog.DoModal();
}

void CVockerView::SetCurrentShape(IDType id) {
  CVockerDoc *pDoc = GetDocument();
  if (!pDoc)
    return;

  auto drawable = pDoc->GetDrawable(id);
  auto shape = std::dynamic_pointer_cast<RShape>(drawable);
  if (shape != nullptr) {
    CurrentShape = shape;
  }
}

// CVockerView diagnostics

#ifdef _DEBUG
void CVockerView::AssertValid() const { CScrollView::AssertValid(); }

void CVockerView::Dump(CDumpContext &dc) const { CScrollView::Dump(dc); }

CVockerDoc *CVockerView::GetDocument() const // non-debug version is inline
{
  ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CVockerDoc)));
  return (CVockerDoc *)m_pDocument;
}
#endif //_DEBUG

// CVockerView message handlers
