
// DKFileClassifierDlg.h : header file
//

#pragma once

#include "DKCoreClassifier\DKClassifier.h"
#include "DKConfig.h"

// CDKFileClassifierDlg dialog
class CDKFileClassifierDlg : public CDialogEx
{
// Construction
public:
	CDKFileClassifierDlg(CWnd* pParent = NULL);	// standard constructor
	~CDKFileClassifierDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DKFILECLASSIFIER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	CEdit			m_editScanFolder;
	CEdit			m_editOutputFolder;
	CButton			m_radioCopyFile;
	CButton			m_buttonRun;
	DKConfig		m_config;

	DKClassifier*	m_pClassifier = nullptr;
	BOOL			m_bRunning = FALSE;
	HANDLE			m_hFinishEvent = NULL;

	static DWORD	WINAPI	ShowFinishThread(LPVOID lpParam);

public:
	afx_msg void OnBnClickedButtonRun();
};
