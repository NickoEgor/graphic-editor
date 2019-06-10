
// VockerDoc.cpp : implementation of the CVockerDoc class
//

#include "Vocker.h"
#include "stdafx.h"

#include "MainFrm.h"
#include "VockerDoc.h"
#include "VockerView.h"

#include <propkey.h>

#include <algorithm>

#include "REllipse.h"
#include "RRectangle.h"
#include "RTriangle.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CVockerDoc

IMPLEMENT_DYNCREATE(CVockerDoc, CDocument)

BEGIN_MESSAGE_MAP(CVockerDoc, CDocument)
END_MESSAGE_MAP()

// CVockerDoc construction/destruction

CVockerDoc::CVockerDoc() noexcept {
  // TODO: add one-time construction code here
}

CVockerDoc::~CVockerDoc() {}

BOOL CVockerDoc::OnNewDocument() {
  if (!CDocument::OnNewDocument())
    return FALSE;

  // TODO: add reinitialization code here
  // (SDI documents will reuse this document)

  ClearMembers();
  return TRUE;
}

void CVockerDoc::ClearMembers() {
  ViewSize = {0, 0};

  CurrentPen.DeleteObject();
  CurrentPen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));

  CurrentBrush.DeleteObject();
  CurrentBrush.CreateSolidBrush(RGB(255, 255, 255));

  Shapes.clear();
  Lines.clear();
}

// CVockerDoc serialization

void CVockerDoc::Serialize(CArchive &ar) {
  if (ar.IsStoring()) {
    Serialization(ar);
  } else {
    Deserialization(ar);
  }
}

void CVockerDoc::Serialization(CArchive &ar) {
  ar << ViewSize << CurrentPen << CurrentBrush;

  size_t shapesSize = Shapes.size(), linesSize = Lines.size();

  ar << shapesSize;
  for (auto &shape : Shapes) {
    CString type = CString(shape->GetRuntimeClass()->m_lpszClassName);
    ShapeType typeEnum;
    if (type == _T("RRectangle")) {
      typeEnum = ShapeType::RECTANGLE;
    } else if (type == _T("REllipse")) {
      typeEnum = ShapeType::ELLIPSE;
    } else if (type == _T("RTriangle")) {
      typeEnum = ShapeType::TRIANGLE;
    } else {
      AfxThrowArchiveException(CArchiveException::genericException,
                               ar.m_strFileName);
    }

    ar << static_cast<int>(typeEnum);
    shape->Serialize(ar);
  }

  ar << linesSize;
  for (auto &line : Lines) {
    line->Serialize(ar);
  }
}

void CVockerDoc::Deserialization(CArchive &ar) {
  ClearMembers();
  ar >> ViewSize >> CurrentPen >> CurrentBrush;

  size_t shapesSize, linesSize;

  ar >> shapesSize;
  Shapes.resize(shapesSize);
  for (size_t i = 0; i < shapesSize; ++i) {
    std::shared_ptr<RShape> shape;

    int typeInt;
    ar >> typeInt;
    ShapeType typeEnum = static_cast<ShapeType>(typeInt);
    if (typeEnum == ShapeType::RECTANGLE) {
      shape = std::make_shared<RRectangle>();
    } else if (typeEnum == ShapeType::ELLIPSE) {
      shape = std::make_shared<REllipse>();
    } else if (typeEnum == ShapeType::TRIANGLE) {
      shape = std::make_shared<RTriangle>();
    } else {
      AfxThrowArchiveException(CArchiveException::badIndex, ar.m_strFileName);
    }

    shape->Serialize(ar);
    Shapes[i] = shape;
  }

  ar >> linesSize;
  Lines.resize(linesSize);
  for (size_t i = 0; i < linesSize; ++i) {
    std::shared_ptr<RLine> line = std::make_shared<RLine>();
    line->Serialize(ar);
    Lines[i] = line;
  }

  for (auto &line : Lines) {
    line->RestoreReferences(Shapes);
  }

  for (auto &shape : Shapes) {
    shape->RestoreReferences(Lines);
  }

  UpdateDocument(true, false);
}

// CVockerDoc diagnostics

#ifdef _DEBUG
void CVockerDoc::AssertValid() const { CDocument::AssertValid(); }

void CVockerDoc::Dump(CDumpContext &dc) const { CDocument::Dump(dc); }
#endif //_DEBUG

// CVockerDoc commands

const std::vector<ShapePtr> &CVockerDoc::GetShapes() { return Shapes; }

const std::vector<LinePtr> &CVockerDoc::GetLines() { return Lines; }

const std::vector<DrawablePtr> &CVockerDoc::GetOrderedDrawables() {
  return OrderedDrawables;
}

void CVockerDoc::AddShape(ShapePtr &shape) {
  Shapes.push_back(shape);
  UpdateDocument();
}

void CVockerDoc::DeleteShape(ShapePtr &shape) {
  for (auto &line : shape->GetLines()) {
    std::shared_ptr<RShape> anotherShape;
    if (line->GetFirstShapeId() == shape->GetId()) {
      anotherShape = line->GetSecondShape();
    } else {
      anotherShape = line->GetFirstShape();
    }

    anotherShape->RemoveLine(line);
    Lines.erase(std::find(Lines.begin(), Lines.end(), line));
  }

  shape->RemoveLines();
  Shapes.erase(std::find(Shapes.begin(), Shapes.end(), shape));

  UpdateDocument();
}

void CVockerDoc::SetShapeOnTop(ShapePtr &shape) {
  Shapes.erase(std::find(Shapes.begin(), Shapes.end(), shape));
  Shapes.push_back(shape);
  UpdateDocument();
}

void CVockerDoc::SetShapeOnBottom(ShapePtr &shape) {
  Shapes.erase(std::find(Shapes.begin(), Shapes.end(), shape));
  Shapes.insert(Shapes.begin(), shape);
  UpdateDocument();
}

void CVockerDoc::AddLine(LinePtr &line) {
  line->GetFirstShape()->AddLine(line);
  line->GetSecondShape()->AddLine(line);
  Lines.push_back(line);
  UpdateDocument();
}

void CVockerDoc::DeleteLine(LinePtr &line) {
  line->FirstShape->RemoveLine(line);
  line->SecondShape->RemoveLine(line);
  Lines.erase(std::find(Lines.begin(), Lines.end(), line));
  UpdateDocument();
}

ShapePtr CVockerDoc::GetShape(CPoint point) {
  for (auto iter = Shapes.rbegin(); iter != Shapes.rend(); ++iter) {
    if ((*iter)->IsPointInDrawable(point)) {
      return (*iter);
    }
  }

  return nullptr;
}

DrawablePtr CVockerDoc::GetDrawable(CPoint point) {
  for (auto iter = OrderedDrawables.rbegin(); iter != OrderedDrawables.rend();
       ++iter) {
    if ((*iter)->IsPointInDrawable(point)) {
      return *iter;
    }
  }

  return nullptr;
}

DrawablePtr CVockerDoc::GetDrawable(IDType id) {
  // find as shape
  auto iter1 =
      std::find_if(Shapes.begin(), Shapes.end(),
                   [&id](ShapePtr shape) { return shape->GetId() == id; });

  if (iter1 != Shapes.end()) {
    return *iter1;
  }

  // find as line
  auto iter2 = std::find_if(Lines.begin(), Lines.end(), [&id](LinePtr line) {
    return line->GetId() == id;
  });

  if (iter2 != Lines.end()) {
    return *iter2;
  }

  return nullptr;
}

void CVockerDoc::DeleteDrawable(IDType id) {
  // find as shape
  auto iter1 =
      std::find_if(Shapes.begin(), Shapes.end(),
                   [&id](ShapePtr shape) { return shape->GetId() == id; });

  if (iter1 != Shapes.end()) {
    DeleteShape(*iter1);
    return;
  }

  // find as line
  auto iter2 = std::find_if(Lines.begin(), Lines.end(), [&id](LinePtr line) {
    return line->GetId() == id;
  });

  if (iter2 != Lines.end()) {
    DeleteLine(*iter2);
    return;
  }

  UpdateDocument();
}

void CVockerDoc::UpdateDocument(bool isUpdateProps, bool isModified) {
  CDocument::SetModifiedFlag(isModified);
  UpdateOrderedDrawables();

  POSITION pos = GetFirstViewPosition();
  CVockerView *view = dynamic_cast<CVockerView *>(GetNextView(pos));
  if (view != nullptr) {
    view->UpdateViewSize();
    view->Invalidate();
    view->UpdateWindow();
  }

  if (isUpdateProps) {
    static_cast<CMainFrame *>(AfxGetMainWnd())->UpdatePropertiesWindows();
  }
}

void CVockerDoc::UpdateOrderedDrawables() {
  std::vector<IDType> previousId;
  OrderedDrawables.clear();

  for (auto &shape : Shapes) {
    previousId.push_back(shape->GetId());
    OrderedDrawables.push_back(shape);
    for (auto &line : shape->GetLines()) {
      IDType lineID = line->GetId();
      if (std::find(previousId.begin(), previousId.end(), lineID) ==
          previousId.end()) {
        previousId.push_back(lineID);
        OrderedDrawables.push_back(line);
      }
    }
  }
}
