
// Vocker.cpp : Defines the class behaviors for the application.
//

#include "Vocker.h"
#include "MainFrm.h"
#include "afxdialogex.h"
#include "afxwinappex.h"
#include "stdafx.h"

#include "VockerDoc.h"
#include "VockerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CVockerApp

BEGIN_MESSAGE_MAP(CVockerApp, CWinAppEx)
ON_COMMAND(ID_APP_ABOUT, &CVockerApp::OnAppAbout)
// Standard file based document commands
ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
END_MESSAGE_MAP()

// CVockerApp construction

CVockerApp::CVockerApp() noexcept {
  m_bHiColorIcons = TRUE;

  // TODO: replace application ID string below with unique ID string;
  // recommended format for string is
  // CompanyName.ProductName.SubProduct.VersionInformation
  SetAppID(_T("Vocker.AppID.NoVersion"));

  // TODO: add construction code here,
  // Place all significant initialization in InitInstance
}

// The one and only CVockerApp object

CVockerApp theApp;

// CVockerApp initialization

BOOL CVockerApp::InitInstance() {
  CWinAppEx::InitInstance();

  EnableTaskbarInteraction(FALSE);

  // AfxInitRichEdit2() is required to use RichEdit control
  // AfxInitRichEdit2();

  // Standard initialization
  // If you are not using these features and wish to reduce the size
  // of your final executable, you should remove from the following
  // the specific initialization routines you do not need
  // Change the registry key under which our settings are stored
  // TODO: You should modify this string to be something appropriate
  // such as the name of your company or organization
  SetRegistryKey(_T("Vocker"));
  LoadStdProfileSettings(4); // Load standard INI file options (including MRU)

  InitContextMenuManager();

  InitKeyboardManager();

  InitTooltipManager();
  CMFCToolTipInfo ttParams;
  ttParams.m_bVislManagerTheme = TRUE;
  theApp.GetTooltipManager()->SetTooltipParams(
      AFX_TOOLTIP_TYPE_ALL, RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

  // Register the application's document templates.  Document templates
  //  serve as the connection between documents, frame windows and views
  CSingleDocTemplate *pDocTemplate;
  pDocTemplate =
      new CSingleDocTemplate(IDR_MAINFRAME, RUNTIME_CLASS(CVockerDoc),
                             RUNTIME_CLASS(CMainFrame), // main SDI frame window
                             RUNTIME_CLASS(CVockerView));
  if (!pDocTemplate)
    return FALSE;
  AddDocTemplate(pDocTemplate);

  // Parse command line for standard shell commands, DDE, file open
  CCommandLineInfo cmdInfo;
  ParseCommandLine(cmdInfo);

  // Enable DDE Execute open
  EnableShellOpen();
  RegisterShellFileTypes(TRUE);

  // Dispatch commands specified on the command line.  Will return FALSE if
  // app was launched with /RegServer, /Register, /Unregserver or /Unregister.
  if (!ProcessShellCommand(cmdInfo))
    return FALSE;

  // The one and only window has been initialized, so show and update it
  m_pMainWnd->ShowWindow(SW_SHOW);
  m_pMainWnd->UpdateWindow();
  // call DragAcceptFiles only if there's a suffix
  //  In an SDI app, this should occur after ProcessShellCommand
  // Enable drag/drop open
  m_pMainWnd->DragAcceptFiles();
  return TRUE;
}

// CVockerApp message handlers

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx {
public:
  CAboutDlg() noexcept;

// Dialog Data
#ifdef AFX_DESIGN_TIME
  enum { IDD = IDD_ABOUTBOX };
#endif

protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support

  // Implementation
protected:
  DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() noexcept : CDialogEx(IDD_ABOUTBOX) {}

void CAboutDlg::DoDataExchange(CDataExchange *pDX) {
  CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// App command to run the dialog
void CVockerApp::OnAppAbout() {
  CAboutDlg aboutDlg;
  aboutDlg.DoModal();
}

// CVockerApp customization load/save methods

void CVockerApp::PreLoadState() {
  BOOL bNameValid;
  CString strName;
  bNameValid = strName.LoadString(IDS_EDIT_MENU);
  ASSERT(bNameValid);
  GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
}

void CVockerApp::LoadCustomState() {}

void CVockerApp::SaveCustomState() {}

// CVockerApp message handlers
