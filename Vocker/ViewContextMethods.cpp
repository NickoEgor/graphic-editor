#include "PropertyListDialog.h"
#include "Vocker.h"
#include "VockerView.h"
#include "stdafx.h"

void CVockerView::OnContextMenu(CWnd * /* pWnd */, CPoint point) {
  CVockerDoc *pDoc = GetDocument();
  if (!pDoc)
    return;

  auto drawable = pDoc->GetDrawable(point);
  ClientToScreen(&point);

  auto line = std::dynamic_pointer_cast<RLine>(drawable);
  auto shape = std::dynamic_pointer_cast<RShape>(drawable);

  if (line != nullptr) {
    ShowLineContextMenu(point, line);
  } else if (shape != nullptr) {
    CurrentShape = shape;
    NotifyCurrentShapeChanged();
    Invalidate();
    UpdateWindow();

    ShowShapeContextMenu(point, shape);
  }
}

void CVockerView::ShowShapeContextMenu(CPoint point, ShapePtr shape) {
  CVockerDoc *pDoc = GetDocument();
  if (!pDoc)
    return;

  CPoint bottomRight = pDoc->ViewSize;

  int nCmd = ContextMenus.GetSubMenu(1)->TrackPopupMenu(
      TPM_LEFTALIGN | TPM_RETURNCMD, point.x, point.y, this);

  switch (nCmd) {
  case ID_SHAPE_DELETE:
    pDoc->DeleteShape(shape);
    CurrentShape = nullptr;
    break;
  case ID_SHAPE_NORMALIZE:
    shape->Normalize();
    pDoc->UpdateDocument();
    break;
  case ID_SHAPE_PROPERTIES: {
    PropertyListDialog dialog(this, shape);
    int nRet = dialog.DoModal();
    if (nRet == IDOK) {
      pDoc->UpdateDocument();
    }
    break;
  }
  case ID_SHAPE_ONTOP:
    pDoc->SetShapeOnTop(shape);
    break;
  case ID_SHAPE_ONBOTTOM:
    pDoc->SetShapeOnBottom(shape);
    break;
  }

  Invalidate();
  UpdateWindow();
}

void CVockerView::ShowLineContextMenu(CPoint point, LinePtr line) {
  CVockerDoc *pDoc = GetDocument();
  if (!pDoc)
    return;

  int nCmd = ContextMenus.GetSubMenu(2)->TrackPopupMenu(
      TPM_LEFTALIGN | TPM_RETURNCMD, point.x, point.y, this);

  switch (nCmd) {
  case ID_LINE_PROPERTIES: {
    PropertyListDialog dialog(this, line);
    int nRet = dialog.DoModal();
    if (nRet == IDOK) {
      pDoc->UpdateDocument();
    }
    break;
    break;
  }
  case ID_LINE_DELETE:
    pDoc->DeleteLine(line);
    break;
  }

  Invalidate();
  UpdateWindow();
}
