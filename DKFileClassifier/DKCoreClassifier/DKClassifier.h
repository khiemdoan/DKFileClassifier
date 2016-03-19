
// author: Khiêm Đoàn Hoà - 2016-03-19

#pragma once

#include "windows.h"
#include "tchar.h"
#include "thread"
#include "Shlwapi.h"
#pragma comment (lib, "Shlwapi.lib")

#include "DKString.h"
#include "DKFileIdentifier.h"

class DKClassifier
{
public:
	static DKClassifier*	GetInstance();
	~DKClassifier();

	VOID	SetInputFolder(LPTSTR szInputFolder);
	VOID	SetOutputFolder(LPTSTR szOutputFolder);
	VOID	SetCopyFile(BOOL bCopy);
	VOID	Run();
	VOID	Stop();

	BOOL	IsRunning();

private:
	DKClassifier();
	static DKClassifier* m_instance;

	static	DWORD WINAPI	Classify(LPVOID lpThisClass);
	FileType	GetType(LPTSTR szFilePath);
	BOOL		m_bRunning = FALSE;
	HANDLE		m_hFinishEvent = NULL;

	VOID	SendToNewFolder(LPTSTR szFilePath, FileType fileType);
	VOID	CreateDirectoryRecursively(LPTSTR pszDir);

	TCHAR	m_sInputFolder[MAX_PATH + 1];
	TCHAR	m_sOutputFolder[MAX_PATH + 1];
	BOOL	m_bCopy = TRUE;
};
