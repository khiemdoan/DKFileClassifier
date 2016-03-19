
// author: Khiêm Đoàn Hoà - 2016-03-19

#include "stdafx.h"
#include "DKFileIdentifier.h"

FileType fileTypes[] = {
	{ _T(".swf"), _T("swf"), 3,{ 'C', 'W', 'S' } },
	{ _T(".swf"), _T("swf"), 3,{ 'F', 'W', 'S' } },
	{ _T(".zip"), _T("zip"), 4,{ 'P', 'K', 0x03, 0x04 } },
	{ _T(".rar"), _T("rar"), 6,{ 'R', 'a', 'r', '!', 0x1A, 0x07 } },
	{ _T(".7z"), _T("7z"), 6,{ '7', 'z', 0xBC, 0xAF, 0x27, 0x1C } },
	{ _T(".cab"), _T("cab"), 4,{ 'M', 'S', 'C', 'F' } },
	{ _T(".doc"), _T("ms_doc"), 9,{ 0xD0, 0xCF, 0x11, 0xE0, 0xA1, 0xB1, 0x1A, 0xE1, 0x00 } },
	{ _T(".elf"), _T("elf"), 4,{ 0x7F, 'E', 'L', 'F' } },
	{ _T(".class"), _T("Java bytecode"), 4,{ 0xCA, 0xFE, 0xBA, 0xBE } }
};

FileType exe = { _T(".dll"), _T("exe"), 0, NULL };
FileType dll = { _T(".dll"), _T("dll"), 0, NULL };

FileType exe64 = { _T(".dll"), _T("exe64"), 0, NULL };
FileType dll64 = { _T(".dll"), _T("dll64"), 0, NULL };

FileType exenet = { _T(".dll"), _T("exe .net"), 0, NULL };
FileType dllnet = { _T(".dll"), _T("dll .net"), 0, NULL };

FileType pe = { _T(".dll"), _T("pe"), 0, NULL };

FileType other = { _T(""), _T("other"), 0, NULL };

DKFileIdentifier::DKFileIdentifier(LPTSTR szFilePath)
{
	m_sFilePath = szFilePath;
}

FileType DKFileIdentifier::Check()
{
	
	FileType fileType;

	fileType = GetTypePe();
	if (memcmp(&fileType, &other, sizeof(FileType)) != 0)
	{
		return fileType;
	}

	fileType = GetTypeByHeader();
	return fileType;
}

FileType DKFileIdentifier::GetTypePe()
{
	DKPeFile pPeFile((LPTSTR)m_sFilePath.c_str());

	if (pPeFile.CheckPe() == FALSE)
	{
		return other;
	}

	if (pPeFile.Check64() == FALSE)
	{
		if (pPeFile.CheckDll())
		{
			return dll;
		}
		else
		{
			return exe;
		}
	}
	else
	{
		if (pPeFile.CheckDll())
		{
			return dll64;
		}
		else
		{
			return exe64;
		}
	}
	
	return pe;
}

FileType DKFileIdentifier::GetTypeByHeader()
{
	DKFile file((LPTSTR)m_sFilePath.c_str());

	int sumTypes = sizeof(fileTypes) / sizeof(FileType);
	for (int i = 0; i < sumTypes; i++)
	{
		if (file.CmpBytes(0, fileTypes[i].sig_legth, fileTypes[i].sig) == TRUE)
		{
			return fileTypes[i];
		}
	}

	return other;
}
