
// DKFileClassifierDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DKFileClassifier.h"
#include "DKFileClassifierDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CDKFileClassifierDlg dialog



CDKFileClassifierDlg::CDKFileClassifierDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DKFILECLASSIFIER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CDKFileClassifierDlg::~CDKFileClassifierDlg()
{
	if (m_pClassifier != nullptr)
	{
		delete m_pClassifier;
		m_pClassifier = nullptr;
	}

	if (m_hFinishEvent != NULL)
	{
		CloseHandle(m_hFinishEvent);
		m_hFinishEvent = NULL;
	}
}

void CDKFileClassifierDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_SCAN_FOLDER, m_editScanFolder);
	DDX_Control(pDX, IDC_EDIT_OUTPUT_FOLDER, m_editOutputFolder);
	DDX_Control(pDX, IDC_RADIO_COPY_FILE, m_radioCopyFile);
	DDX_Control(pDX, IDC_BUTTON_RUN, m_buttonRun);
}

BEGIN_MESSAGE_MAP(CDKFileClassifierDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_RUN, &CDKFileClassifierDlg::OnBnClickedButtonRun)
END_MESSAGE_MAP()


// CDKFileClassifierDlg message handlers

BOOL CDKFileClassifierDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	if (m_config.IsCopy())
		m_radioCopyFile.SetCheck(BST_CHECKED);
	else
		m_radioCopyFile.SetCheck(BST_UNCHECKED);
	m_editScanFolder.SetWindowText(m_config.GetConfig(_T("input")).c_str());
	m_editOutputFolder.SetWindowText(m_config.GetConfig(_T("output")).c_str());

	m_hFinishEvent = CreateEvent(NULL, FALSE, FALSE, _T("Local\\Finish"));
	HANDLE hThread = CreateThread(NULL, 0, &CDKFileClassifierDlg::ShowFinishThread, this, 0, NULL);
	CloseHandle(hThread);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDKFileClassifierDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDKFileClassifierDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDKFileClassifierDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

DWORD WINAPI CDKFileClassifierDlg::ShowFinishThread(LPVOID lpParam)
{
	CDKFileClassifierDlg* thisclass = (CDKFileClassifierDlg*)lpParam;

	while (TRUE)
	{
		if (thisclass->m_hFinishEvent == NULL)
			break;

		DWORD dwWait = WaitForSingleObject(thisclass->m_hFinishEvent, INFINITE);
		if (dwWait == WAIT_OBJECT_0)
		{
			AfxMessageBox(_T("Finish classify!!!"), MB_OK);
			thisclass->m_bRunning = FALSE;
			thisclass->m_buttonRun.SetWindowText(_T("Run"));
		}
	}

	return EXIT_SUCCESS;
}

void CDKFileClassifierDlg::OnBnClickedButtonRun()
{
	// TODO: Add your control notification handler code here
	CString	sScanFolder;
	CString	sOutputFolder;
	BOOL	bCopy = TRUE;

	if (m_bRunning == FALSE)
	{
		m_editScanFolder.GetWindowText(sScanFolder);
		m_editOutputFolder.GetWindowText(sOutputFolder);
		if (m_radioCopyFile.GetCheck() == BST_CHECKED)
			bCopy = TRUE;
		else
			bCopy = FALSE;

		m_pClassifier = DKClassifier::GetInstance();
		m_pClassifier->SetInputFolder(sScanFolder.GetBuffer());
		m_pClassifier->SetOutputFolder(sOutputFolder.GetBuffer());
		m_pClassifier->SetCopyFile(bCopy);
		m_pClassifier->Run();

		m_config.SetConfig(_T("input"), sScanFolder.GetBuffer());
		m_config.SetConfig(_T("output"), sOutputFolder.GetBuffer());
		m_config.SetCopy(bCopy);

		m_buttonRun.SetWindowText(_T("Stop"));
		m_bRunning = TRUE;
	}
	else
	{
		m_pClassifier = DKClassifier::GetInstance();
		m_pClassifier->Stop();

		m_buttonRun.SetWindowText(_T("Run"));
		m_bRunning = FALSE;
	}
}
