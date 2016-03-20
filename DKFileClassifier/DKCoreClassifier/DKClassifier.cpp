
// author: Khiêm Đoàn Hoà - 2016-03-19

#include "stdafx.h"
#include "DKClassifier.h"

DKClassifier* DKClassifier::m_instance = nullptr;

DKClassifier * DKClassifier::GetInstance()
{
	if (m_instance == nullptr)
	{
		m_instance = new DKClassifier();
	}
	return m_instance;
}

DKClassifier::DKClassifier()
{
	m_hFinishEvent = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, FALSE, _T("Local\\Finish"));
	DWORD dw = 20;
	dw++;
	dw++;
}

DKClassifier::~DKClassifier()
{
	if (m_hFinishEvent != NULL)
	{
		CloseHandle(m_hFinishEvent);
		m_hFinishEvent = NULL;
	}
	m_instance = nullptr;
}

VOID DKClassifier::SetInputFolder(LPTSTR szInputFolder)
{
	_tcsncpy_s(m_sInputFolder, _countof(m_sInputFolder), szInputFolder, _TRUNCATE);
}

VOID DKClassifier::SetOutputFolder(LPTSTR szOutputFolder)
{
	_tcsncpy_s(m_sOutputFolder, _countof(m_sOutputFolder), szOutputFolder, _TRUNCATE);
}

VOID DKClassifier::SetCopyFile(BOOL bCopy = TRUE)
{
	m_bCopy = bCopy;
}

VOID DKClassifier::Run()
{
	m_bRunning = TRUE;
	HANDLE hThread = CreateThread(NULL, 0, &DKClassifier::Classify, this, 0, NULL);
	if (hThread != NULL)
	{
		CloseHandle(hThread);
		hThread = NULL;
	}
}

VOID DKClassifier::Stop()
{
	m_bRunning = FALSE;
}

BOOL DKClassifier::IsRunning()
{
	return m_bRunning;
}

DWORD WINAPI DKClassifier::Classify(LPVOID lpThisClass)
{
	TCHAR				sFilePath[MAX_PATH + 1] = { 0 };
	HANDLE				hFind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA		w32Data;
	DKClassifier*		thisclass = (DKClassifier*)lpThisClass;
	TCHAR				szInputFolder[MAX_PATH + 1] = { 0 };
	TCHAR				szOutputFolder[MAX_PATH + 1] = { 0 };

	_tcsncpy_s(szInputFolder, MAX_PATH, thisclass->m_sInputFolder, _TRUNCATE);
	_tcsncpy_s(szOutputFolder, MAX_PATH, thisclass->m_sOutputFolder, _TRUNCATE);

	__try
	{
		_sntprintf_s(sFilePath, _countof(sFilePath), _TRUNCATE, _T("%s\\*.*"), szInputFolder);
		hFind = FindFirstFile(sFilePath, &w32Data);
		if (hFind == INVALID_HANDLE_VALUE) __leave;

		do
		{
			if (thisclass->m_bRunning == FALSE)
				__leave;

			if (!_tcscmp(w32Data.cFileName, _T(".")) || !_tcscmp(w32Data.cFileName, _T("..")))
				continue;
			if ((w32Data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
				continue;

			_sntprintf_s(sFilePath, _countof(sFilePath), _TRUNCATE,
				_T("%s\\%s"), szInputFolder, w32Data.cFileName);
			
			FileType fileType = thisclass->GetType(sFilePath);
			thisclass->SendToNewFolder(sFilePath, fileType);

		} while (FindNextFile(hFind, &w32Data));

		if (thisclass->m_hFinishEvent != NULL)
		{
			SetEvent(thisclass->m_hFinishEvent);
		}
	}
	__finally
	{
		if (hFind != INVALID_HANDLE_VALUE)
		{
			FindClose(hFind);
			hFind = INVALID_HANDLE_VALUE;
		}
	}

	thisclass->m_bRunning = TRUE;
	return EXIT_SUCCESS;
}

FileType DKClassifier::GetType(LPTSTR szFilePath)
{
	DKFileIdentifier	fileId(szFilePath);
	return fileId.Check();
}

VOID DKClassifier::SendToNewFolder(LPTSTR szFilePath, FileType fileType)
{
	TCHAR szNewDir[MAX_PATH + 1] = { 0 };
	LPTSTR szDirName = fileType.folder;
	LPTSTR szExt = fileType.ext;

	_tcsncpy_s(szNewDir, _countof(szNewDir), m_sOutputFolder, _TRUNCATE);
	PathAppend(szNewDir, szDirName);
	if (PathIsDirectory(szNewDir) == FALSE)
	{
		CreateDirectoryRecursively(szNewDir);
	}

	TCHAR *szFileName = PathFindFileName(szFilePath);
	if (szFileName == szFilePath)
		return;

	if (PathFileExists(szFilePath) == TRUE)
	{
		TCHAR szNewFile[MAX_PATH + 1] = { 0 };
		_sntprintf_s(szNewFile, _countof(szNewFile), _TRUNCATE, _T("%s\\%s"), szNewDir, szFileName);
		
		if (_tcsicmp(szNewFile + _tcslen(szNewFile) - _tcslen(szExt), szExt) != 0)
		{
			_tcsncat_s(szNewFile, _countof(szNewFile), szExt, _TRUNCATE);
		}

		if (m_bCopy == TRUE)
			CopyFile(szFilePath, szNewFile, FALSE);
		else
			MoveFile(szFilePath, szNewFile);
	}
}

VOID DKClassifier::CreateDirectoryRecursively(LPTSTR pszDir)
{
	if (::GetFileAttributes(pszDir) == INVALID_FILE_ATTRIBUTES)
	{
		TCHAR pszParentDir[MAX_PATH + 1] = { 0 };
		LPTSTR pLastBackslash = _tcsrchr(pszDir, _T('\\'));
		_tcsncpy_s(pszParentDir, pLastBackslash - pszDir + 1, pszDir, _TRUNCATE);
		this->CreateDirectoryRecursively(pszParentDir);
		::CreateDirectory(pszDir, NULL);
	}
}
