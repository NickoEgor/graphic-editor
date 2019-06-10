#include "PenPickerDialog.h"
#include "DrawableProperty.h"
#include "Utilities.h"
#include "Vocker.h"
#include "afxdialogex.h"
#include "stdafx.h"

IMPLEMENT_DYNAMIC(PenPickerDialog, CDialogEx)

BEGIN_MESSAGE_MAP(PenPickerDialog, CDialogEx)
ON_BN_CLICKED(IDC_MFCBUTTON1, &PenPickerDialog::OnColorDialogButtonClicked)
ON_EN_CHANGE(IDC_EDIT1, &PenPickerDialog::OnWidthEditChanged)
END_MESSAGE_MAP()

PenPickerDialog::PenPickerDialog(CWnd *pParent, CPen *pen)
    : CDialogEx(IDD_PENPICKERBOX, pParent), Pen(pen) {}

BOOL PenPickerDialog::OnInitDialog() {
  CDialogEx::OnInitDialog();

  HICON hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
  SetIcon(hIcon, TRUE);
  SetIcon(hIcon, FALSE);

  LOGPEN penInfo;
  Pen->GetLogPen(&penInfo);

  StyleComboBox.SetMinVisibleItems(10);

  for (size_t i = 0; i < DrawableProperty::PenStyles.size(); ++i) {
    StyleComboBox.InsertString(i, DrawableProperty::PenStyles[i].second);
    if (DrawableProperty::PenStyles[i].first == penInfo.lopnStyle) {
      StyleComboBox.SetCurSel(i);
    }
  }

  WidthEdit.LimitText(EditSize);
  CString width;
  width.Format(_T("%d"), penInfo.lopnWidth.x);
  WidthEdit.SetWindowTextW(width);

  ColoredButton.EnableWindowsTheming(FALSE);
  ColoredButton.SetFaceColor(penInfo.lopnColor);

  StaticPicture.SetIcon(AfxGetApp()->LoadStandardIcon(IDI_WARNING));

  UpdateWarningVisibility(penInfo.lopnWidth.x);

  return TRUE;
}

void PenPickerDialog::OnOK() {
  Pen->DeleteObject();

  UINT style = DrawableProperty::PenStyles[StyleComboBox.GetCurSel()].first;

  TCHAR widthStr[EditSize];
  WidthEdit.GetLine(0, widthStr);
  try {
    int width = CStringToInt(CString(widthStr));
    UpdateWarningVisibility(width);
    Pen->CreatePen(style, width, PenColor);
    CDialogEx::OnOK();
  } catch (std::invalid_argument) {
    MessageBox(_T("Incorrect width"), _T("Error"), MB_OK | MB_ICONERROR);
  }
}

void PenPickerDialog::DoDataExchange(CDataExchange *pDX) {
  CDialogEx::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_EDIT1, WidthEdit);
  DDX_Control(pDX, IDC_COMBO1, StyleComboBox);
  DDX_Control(pDX, IDC_MFCBUTTON1, ColoredButton);
  DDX_Control(pDX, IDC_WARNINGSIGN, StaticPicture);
  DDX_Control(pDX, IDC_WARNINGLABEL, StaticLabel);
}

void PenPickerDialog::OnColorDialogButtonClicked() {
  CColorDialog dialog(PenColor);
  if (dialog.DoModal() == IDOK) {
    PenColor = dialog.GetColor();
    ColoredButton.SetFaceColor(PenColor);
  }
}

void PenPickerDialog::UpdateWarningVisibility(int width) {
  StaticPicture.ShowWindow(width > 1);
  StaticLabel.ShowWindow(width > 1);
}

void PenPickerDialog::OnWidthEditChanged() {
  TCHAR widthStr[EditSize];
  WidthEdit.GetLine(0, widthStr);
  try {
    int width = CStringToInt(CString(widthStr));
    UpdateWarningVisibility(width);
  } catch (std::invalid_argument) {
  }
}
