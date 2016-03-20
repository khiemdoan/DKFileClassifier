
// author: Khiêm Đoàn Hoà - 2016-03-19

#pragma once

#include "windows.h"
#include "tchar.h"

#include "DKString.h"
#include "DKPeFile.h"

// define struct about file
struct FileType
{
	TCHAR*	ext;
	TCHAR*	folder;
	int		sig_legth;
	byte	sig[30];
};

class DKFileIdentifier
{
public:
	DKFileIdentifier(LPTSTR szFilePath);
	
	FileType	Check();

private:
	FileType	GetTypePe();
	FileType	GetTypeByHeader();
	FileType	GetTypeByCompareString();

	_tstring	m_sFilePath;
};