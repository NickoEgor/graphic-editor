#pragma once

#include <utility>

class PenPickerDialog : public CDialogEx {
private:
  DECLARE_DYNAMIC(PenPickerDialog)

public:
  PenPickerDialog(CWnd *pParent, CPen *pen);

  BOOL OnInitDialog();
  void OnOK();

// Dialog Data
#ifdef AFX_DESIGN_TIME
  enum { IDD = IDD_PENPICKERBOX };
#endif

protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support

  DECLARE_MESSAGE_MAP()

  afx_msg void OnColorDialogButtonClicked();

private:
  CPen *Pen;
  COLORREF PenColor;

  static const int EditSize = 3;

  CEdit WidthEdit;
  CComboBox StyleComboBox;
  CMFCButton ColoredButton;
  CStatic StaticPicture;
  CStatic StaticLabel;

  void UpdateWarningVisibility(int width);

public:
  afx_msg void OnWidthEditChanged();
};
