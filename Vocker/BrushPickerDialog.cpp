#include "BrushPickerDialog.h"
#include "DrawableProperty.h"
#include "Vocker.h"
#include "afxdialogex.h"
#include "stdafx.h"

IMPLEMENT_DYNAMIC(BrushPickerDialog, CDialogEx)

BEGIN_MESSAGE_MAP(BrushPickerDialog, CDialogEx)
ON_BN_CLICKED(IDC_MFCBUTTON1, &BrushPickerDialog::OnColorDialogButtonClicked)
END_MESSAGE_MAP()

BrushPickerDialog::BrushPickerDialog(CWnd *pParent, CBrush *brush)
    : CDialogEx(IDD_BRUSHPICKERBOX, pParent), Brush(brush) {}

BOOL BrushPickerDialog::OnInitDialog() {
  CDialogEx::OnInitDialog();

  HICON hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
  SetIcon(hIcon, TRUE);
  SetIcon(hIcon, FALSE);

  LOGBRUSH brushInfo;
  Brush->GetLogBrush(&brushInfo);

  UINT brushId = DrawableProperty::GetBrushIdentifier(brushInfo);

  StyleComboBox.SetMinVisibleItems(10);

  for (size_t i = 0; i < DrawableProperty::BrushStyles.size(); ++i) {
    StyleComboBox.InsertString(i, DrawableProperty::BrushStyles[i].second);
    if (brushId == DrawableProperty::BrushStyles[i].first) {
      StyleComboBox.SetCurSel(i);
    }
  }

  ColoredButton.EnableWindowsTheming(FALSE);
  ColoredButton.SetFaceColor(brushInfo.lbColor);

  BrushColor = brushInfo.lbColor;

  return TRUE;
}

void BrushPickerDialog::OnOK() {
  Brush->DeleteObject();

  UINT style = DrawableProperty::BrushStyles[StyleComboBox.GetCurSel()].first;

  if (style != DrawableProperty::SolidBrushStyle) {
    Brush->CreateHatchBrush(style, BrushColor);
  } else {
    Brush->CreateSolidBrush(BrushColor);
  }

  CDialogEx::OnOK();
}

void BrushPickerDialog::DoDataExchange(CDataExchange *pDX) {
  CDialogEx::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_COMBO2, StyleComboBox);
  DDX_Control(pDX, IDC_MFCBUTTON1, ColoredButton);
}

void BrushPickerDialog::OnColorDialogButtonClicked() {
  CColorDialog dialog(BrushColor);
  if (dialog.DoModal() == IDOK) {
    BrushColor = dialog.GetColor();
    ColoredButton.SetFaceColor(BrushColor);
  }
}
