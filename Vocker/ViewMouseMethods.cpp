#include "Vocker.h"
#include "VockerView.h"
#include "stdafx.h"

#include "REllipse.h"
#include "RRectangle.h"
#include "RTriangle.h"

void CVockerView::OnLButtonDown(UINT nFlags, CPoint point) {
  CVockerDoc *pDoc = GetDocument();
  if (!pDoc)
    return;

  auto currentTool = GetCurrentTool();

  TempPoint = GetScrollPosition() + point;
  TempRect = {TempPoint, TempPoint};
  SetCapture();

  switch (currentTool) {
  case PaintTool::RECTANGLE:
  case PaintTool::ELLIPSE:
  case PaintTool::TRIANGLE:
    CurrentMode = Mode::SHAPE;
    break;
  case PaintTool::RESIZESHAPE:
    if (CurrentShape != nullptr) {
      TempMarkerNumber = GetMarkerNumber(point);
      if (TempMarkerNumber != InvalidMarkerNumber) {
        CurrentMode = Mode::RESIZING;
      }
    } else {
      CurrentShape = pDoc->GetShape(point);
      NotifyCurrentShapeChanged();
      Invalidate();
      UpdateWindow();
    }
    break;
  default:
    CurrentShape = pDoc->GetShape(point);
    NotifyCurrentShapeChanged();
    Invalidate();
    UpdateWindow();

    if (CurrentShape != nullptr && CurrentShape->IsPointInDrawable(point)) {
      switch (currentTool) {
      case PaintTool::LINE:
      case PaintTool::ONEWAYLINE:
      case PaintTool::TWOWAYLINE: {
        auto center = CurrentShape->GetCenter();
        TempRect.TopLeft() = center;
        CurrentMode = Mode::LINE;
        break;
      }
      case PaintTool::MOVESHAPE:
        CurrentMode = Mode::MOVING;
        break;

      case PaintTool::ROTATESHAPE:
        CurrentMode = Mode::ROTATING;
        break;
      }
    }
    break;
  }
}

void CVockerView::OnLButtonUp(UINT nFlags, CPoint point) {
  CVockerDoc *pDoc = GetDocument();
  if (!pDoc)
    return;

  auto currentTool = GetCurrentTool();
  ReleaseCapture();

  switch (CurrentMode) {
  case Mode::SHAPE: {
    if (TempRect.left != TempRect.right && TempRect.top != TempRect.bottom) {

      ShapePtr shape;
      switch (currentTool) {
      case PaintTool::RECTANGLE:
        shape = std::make_shared<RRectangle>(TempRect);
        break;
      case PaintTool::ELLIPSE:
        shape = std::make_shared<REllipse>(TempRect);
        break;
      case PaintTool::TRIANGLE:
        shape = std::make_shared<RTriangle>(TempRect);
        break;
      }

      LOGPEN logPen;
      pDoc->CurrentPen.GetLogPen(&logPen);
      shape->SetPen(&logPen);

      LOGBRUSH logBrush;
      pDoc->CurrentBrush.GetLogBrush(&logBrush);
      shape->SetBrush(&logBrush);

      pDoc->AddShape(shape);
    }

    TempPoint = InvalidPoint;
    TempRect = InvalidRect;

    CurrentMode = Mode::NONE;
    UpdateViewSize();
    Invalidate();
    UpdateWindow();
    break;
  }
  case Mode::LINE: {
    TempPoint = InvalidPoint;
    TempRect = InvalidRect;
    CurrentMode = Mode::NONE;

    auto secondShape = pDoc->GetShape(point);
    if (secondShape == nullptr || secondShape == CurrentShape) {
      Invalidate();
      UpdateWindow();
      break;
    }

    LinePtr line = std::make_shared<RLine>(CurrentShape, secondShape);
    switch (currentTool) {
    case PaintTool::LINE:
      line->SetFirstArrow(false);
      line->SetSecondArrow(false);
      break;
    case PaintTool::ONEWAYLINE:
      line->SetFirstArrow(false);
      line->SetSecondArrow(true);
      break;
    case PaintTool::TWOWAYLINE:
      line->SetFirstArrow(true);
      line->SetSecondArrow(true);
      break;
    }

    LOGPEN logPen;
    pDoc->CurrentPen.GetLogPen(&logPen);
    line->SetPen(&logPen);

    pDoc->AddLine(line);

    Invalidate();
    UpdateWindow();
    break;
  }
  case Mode::RESIZING:
    TempMarkerNumber = InvalidMarkerNumber;
  case Mode::MOVING:
  case Mode::ROTATING:
    TempPoint = InvalidPoint;
    TempRect = InvalidRect;

    CurrentMode = Mode::NONE;
    UpdateViewSize();
    Invalidate();
    UpdateWindow();
    break;
  }
}

void CVockerView::OnMouseMove(UINT nFlags, CPoint point) {
  CVockerDoc *pDoc = GetDocument();
  if (!pDoc)
    return;

  switch (CurrentMode) {
  case Mode::SHAPE:
  case Mode::LINE:
    DragMouse(point);

    point += GetScrollPosition();
    TempRect.right = point.x;
    TempRect.bottom = point.y;

    Invalidate();
    UpdateWindow();
    break;
  case Mode::MOVING: {
    DragMouse(point);

    TempRect.BottomRight() = point + GetScrollPosition();

    CPoint newPoint = point + GetScrollPosition();
    CurrentShape->Move(CSize(newPoint - TempPoint));
    pDoc->UpdateDocument(false);

    TempPoint = newPoint;

    Invalidate();
    UpdateWindow();
    break;
  }
  case Mode::ROTATING: {
    CPoint newPoint = point + GetScrollPosition();
    CPoint center = CurrentShape->GetCenter();

    CSize vecA{TempPoint - center}, vecB{newPoint - center};
    double lenA = sqrt(vecA.cx * vecA.cx + vecA.cy * vecA.cy);
    double lenB = sqrt(vecB.cx * vecB.cx + vecB.cy * vecB.cy);
    double angleRad =
        acos((vecA.cx * vecB.cx + vecA.cy * vecB.cy) / (lenA * lenB));

    if (vecA.cx * vecB.cy - vecA.cy * vecB.cx < 0) {
      angleRad *= -1;
    }

    CurrentShape->Rotate(angleRad);
    pDoc->UpdateDocument(false);

    CString newAngleStr;
    newAngleStr.Format(_T("%f"), CurrentShape->GetAngle());
    IDType id = CurrentShape->GetId();
    GetMainWindow()->UpdateProperty(id, id, PropName::ANGLE, newAngleStr);

    TempPoint = newPoint;

    Invalidate();
    UpdateWindow();
    break;
  }
  case Mode::RESIZING:
    DragMouse(point);

    CPoint newPoint = point + GetScrollPosition();
    if (newPoint == TempPoint)
      return;

    CurrentShape->Resize(TempMarkerNumber, newPoint - TempPoint);
    pDoc->UpdateDocument(false);
    TempPoint = newPoint;

    Invalidate();
    UpdateWindow();
    break;
  }
}

void CVockerView::OnLButtonDblClk(UINT nFlags, CPoint point) {
  CVockerDoc *pDoc = GetDocument();
  if (!pDoc)
    return;

  CurrentShape = pDoc->GetShape(point);
  NotifyCurrentShapeChanged();

  Invalidate();
  UpdateWindow();
}

void CVockerView::OnRButtonUp(UINT nFlags, CPoint point) {
  if (CurrentMode == Mode::SHAPE || CurrentMode == Mode::LINE) {
    TempPoint = InvalidPoint;
    TempRect = InvalidRect;
    TempMarkerNumber = InvalidMarkerNumber;

    CurrentMode = Mode::NONE;

    Invalidate();
    UpdateWindow();
  } else {
    OnContextMenu(this, point);
  }
}

void CVockerView::DragMouse(CPoint point) {
  CVockerDoc *pDoc = GetDocument();
  if (!pDoc)
    return;

  CPoint position = GetScrollPosition();
  CPoint viewSize = pDoc->ViewSize;
  CRect clientViewSize;
  GetClientRect(&clientViewSize);

  if (point.x > clientViewSize.right - BorderIndent &&
      position.x + clientViewSize.right < viewSize.x) {
    SetScrollPos(SB_HORZ, position.x + ScrollStep);
  } else if (point.x < BorderIndent && position.x > 0) {
    SetScrollPos(SB_HORZ, position.x - ScrollStep);
  }

  if (point.y > clientViewSize.bottom - BorderIndent &&
      position.y + clientViewSize.bottom < viewSize.y) {
    SetScrollPos(SB_VERT, position.y + ScrollStep);
  } else if (point.y < BorderIndent && position.y > 0) {
    SetScrollPos(SB_VERT, position.y - ScrollStep);
  }
}
