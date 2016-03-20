
// author: Khiêm Đoàn Hoà - 2016-03-19

#include "stdafx.h"
#include "DKFile.h"

DKFile::DKFile(LPTSTR szFilePath)
{
	DWORD	dwFileSizeHigh = 0;

	__try
	{
		m_hFile = CreateFile(szFilePath, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (m_hFile == INVALID_HANDLE_VALUE) __leave;

		m_dwFileSize = GetFileSize(m_hFile, &dwFileSizeHigh);
		if (m_dwFileSize == INVALID_FILE_SIZE) __leave;

		m_hMap = CreateFileMapping(m_hFile, NULL, PAGE_READWRITE, 0, 0, NULL);
		if (m_hMap == NULL) __leave;

		m_lpAddress = MapViewOfFile(m_hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		if (m_lpAddress == NULL) __leave;

	}
	__finally
	{
		m_dwErrorCode = GetLastError();
	}
}

DKFile::~DKFile()
{
	if (m_lpAddress != NULL)
	{
		UnmapViewOfFile(m_lpAddress);
		m_lpAddress = NULL;
	}

	if (m_hMap != NULL)
	{
		CloseHandle(m_hMap);
		m_hMap = NULL;
	}

	if (m_hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}
}

DWORD DKFile::GetLastError()
{
	return m_dwErrorCode;
}

BOOL DKFile::CmpBytes(DWORD dwOffset, DWORD dwSize, LPVOID lpData)
{
	if (dwOffset + dwSize > m_dwFileSize)
		return FALSE;

	if (memcmp((LPVOID)((DWORD)m_lpAddress + dwOffset), lpData, dwSize) == 0)
		return TRUE;

	return FALSE;
}

BOOL DKFile::SetData(DWORD dwOffset, DWORD dwSize, LPVOID lpData)
{
	if (dwOffset + dwSize > m_dwFileSize)
		return FALSE;

	if (memcpy((LPVOID)((DWORD)m_lpAddress + dwOffset), lpData, dwSize) == 0)
		return FALSE;

	return TRUE;
}

DWORD DKFile::FindSubStringA(DWORD dwPos, DWORD dwCount, LPCH szSubString)
{
	std::string str((char*)m_lpAddress, dwPos, dwCount);
	std::transform(str.begin(), str.end(), str.begin(), [](char c) { return std::tolower(c); });
	std::string::size_type find = str.find(szSubString);
	if (find == std::string::npos)
		return -1;
	return find;
}

DWORD DKFile::FindSubStringW(DWORD dwPos, DWORD dwCount, LPCWCHAR szSubString)
{
	std::wstring str((wchar_t*)m_lpAddress, dwPos, dwCount);
	std::transform(str.begin(), str.end(), str.begin(), [](wchar_t c) { return std::tolower(c); });
	std::wstring::size_type find = str.find(szSubString);
	if (find == std::wstring::npos)
		return -1;
	return find;
}
