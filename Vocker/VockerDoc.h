
// VockerDoc.h : interface of the CVockerDoc class
//

#pragma once

#include <memory>
#include <vector>

#include "RLine.h"
#include "RShape.h"

using ShapePtr = std::shared_ptr<RShape>;
using LinePtr = std::shared_ptr<RLine>;
using DrawablePtr = std::shared_ptr<RDrawable>;

class CVockerDoc : public CDocument {
protected: // create from serialization only
  CVockerDoc() noexcept;
  DECLARE_DYNCREATE(CVockerDoc)

public:
  CPoint ViewSize;
  CPen CurrentPen;
  CBrush CurrentBrush;

  // Operations
public:
  const std::vector<ShapePtr> &GetShapes();
  const std::vector<LinePtr> &GetLines();
  const std::vector<DrawablePtr> &GetOrderedDrawables();

  void AddShape(ShapePtr &shape);
  void DeleteShape(ShapePtr &shape);

  void SetShapeOnTop(ShapePtr &shape);
  void SetShapeOnBottom(ShapePtr &shape);

  void AddLine(LinePtr &line);
  void DeleteLine(LinePtr &line);

  ShapePtr GetShape(CPoint point);
  DrawablePtr GetDrawable(CPoint point);
  DrawablePtr GetDrawable(IDType id);
  void DeleteDrawable(IDType id);

  void UpdateDocument(bool isUpdateProps = true, bool isModified = true);

private:
  std::vector<ShapePtr> Shapes;
  std::vector<LinePtr> Lines;

  std::vector<DrawablePtr> OrderedDrawables;
  void UpdateOrderedDrawables();

private:
  enum class ShapeType { RECTANGLE, ELLIPSE, TRIANGLE };
  void Serialization(CArchive &ar);
  void Deserialization(CArchive &ar);

  // Overrides
public:
  virtual BOOL OnNewDocument();
  virtual void Serialize(CArchive &ar);

  void ClearMembers();

  // Implementation
public:
  virtual ~CVockerDoc();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext &dc) const;
#endif

protected:
  // Generated message map functions
protected:
  DECLARE_MESSAGE_MAP()
};
