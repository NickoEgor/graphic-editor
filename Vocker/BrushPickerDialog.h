#pragma once

#include <utility>

class BrushPickerDialog : public CDialogEx {
private:
  DECLARE_DYNAMIC(BrushPickerDialog)

public:
  BrushPickerDialog(CWnd *pParent, CBrush *brush);

  void OnColorDialogButtonClicked();

  BOOL OnInitDialog();
  void OnOK();

// Dialog Data
#ifdef AFX_DESIGN_TIME
  enum { IDD = IDD_BRUSHPICKERBOX };
#endif

protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support

  DECLARE_MESSAGE_MAP()

private:
  CBrush *Brush;
  COLORREF BrushColor;

  CComboBox StyleComboBox;
  CMFCButton ColoredButton;
};
