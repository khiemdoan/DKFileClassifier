
// author: Khiêm Đoàn Hoà - 2016-03-19

#include "stdafx.h"
#include "DKFileIdentifier.h"

FileType fileTypes[] = {
	
	// application
	{ _T(".swf"), _T("swf"), 3, { 'C', 'W', 'S' } },
	{ _T(".swf"), _T("swf"), 3, { 'F', 'W', 'S' } },
	{ _T(".elf"), _T("elf"), 4, { 0x7F, 'E', 'L', 'F' } },
	{ _T(".class"), _T("Java bytecode"), 4, { 0xCA, 0xFE, 0xBA, 0xBE } },

	// compression
	{ _T(".zip"), _T("zip"), 4,{ 'P', 'K', 0x03, 0x04 } },
	{ _T(".rar"), _T("rar"), 6,{ 'R', 'a', 'r', '!', 0x1A, 0x07 } },
	{ _T(".7z"), _T("7z"), 6,{ '7', 'z', 0xBC, 0xAF, 0x27, 0x1C } },
	{ _T(".cab"), _T("cab"), 4,{ 'M', 'S', 'C', 'F' } },

	// image file
	{ _T(".jpg"), _T("images"), 3,{ 0xFF, 0xD8, 0xFF } },
	{ _T(".png"), _T("images"), 16,{ 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44, 0x52 }},
	{ _T(".ico"), _T("images"), 3,{ 0x00, 0x00, 0x01 } },
	{ _T(".gif"), _T("images"), 4,{ 'G', 'I', 'F', '8' } },
	{ _T(".bmp"), _T("images"), 2,{ 'B', 'M' } },

	// document
	{ _T(".doc"), _T("document"), 9,{ 0xD0, 0xCF, 0x11, 0xE0, 0xA1, 0xB1, 0x1A, 0xE1, 0x00 } },
	{ _T(".pdf"), _T("document"), 7, { '%', 'P', 'D', 'F', '-', '1', '.' } },

	// audio
	{ _T(".mp3"), _T("audio"), 3,{ 'I', 'D', '3' } },
	{ _T(".m3u"), _T("audio"), 9,{ '#', 'E', 'X', 'T', 'M', '3', 'U', 0x0D, 0x0A } },
	{ _T(".m3u8"), _T("audio"), 12,{ 0xEF, 0xBB, 0xBF, '#', 'E', 'X', 'T', 'M', '3', 'U', 0x0D, 0x0A } },
	{ _T(".flac"), _T("audio"), 8,{ 'f', 'L', 'a', 'C', 0x00, 0x00, 0x00, 0x22 } },
	

	// video
	{ _T(".mp4"), _T("video"), 3,{ 0x00, 0x00, 0x00 } },	// also m4a, m4p, 3gp
	{ _T(".flv"), _T("video"), 4,{ 'F', 'L', 'V', 0x01 } },
	{ _T(".mkv"), _T("video"), 28,{ 0x1A, 0x45,  0xDF, 0xA3, 0x93, 0x42,  0x82, 0x88, 0x6D, 0x61, 0x74, 0x72, 0x6F, 0x73, 0x6B, 0x61, 0x42, 0x87, 0x81, 0x01, 0x42, 0x85, 0x81, 0x01, 0x18,  0x53 , 0x80 , 0x67 } },
	{ _T(".vob"), _T("video"), 4,{ 0x00, 0x00, 0x01, 0xBA } },
	{ _T(".ogg"), _T("video"), 14,{ 0x4F , 0x67 , 0x67 , 0x53 , 0x00 , 0x02 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 } },
	{ _T(".avi"), _T("video"), 4,{ 'R', 'I', 'F', 'F' } },	// also mav
	{ _T(".mov"), _T("video"), 1,{ 'm' } },
	{ _T(".wmv"), _T("video"), 16,{ 0x30 , 0x26 , 0xB2 , 0x75 , 0x8E, 0x66, 0xCF , 0x11, 0xA6 , 0xD9 , 0x00 , 0xAA , 0x00 , 0x62 , 0xCE , 0x6C } },
	{ _T(".rm"), _T("video"), 9,{ '.', 'R', 'M', 'F', 0x00, 0x00, 0x00, 0x12, 0x00 } },
	{ _T(".3g2"), _T("video"), 4,{ 'f', 't', 'y', 'p' } },
};

FileType exe = { _T(".dll"), _T("exe"), 0, NULL };
FileType dll = { _T(".dll"), _T("dll"), 0, NULL };

FileType exe64 = { _T(".dll"), _T("exe64"), 0, NULL };
FileType dll64 = { _T(".dll"), _T("dll64"), 0, NULL };

FileType exenet = { _T(".dll"), _T("exe .net"), 0, NULL };
FileType dllnet = { _T(".dll"), _T("dll .net"), 0, NULL };

FileType pe = { _T(".dll"), _T("pe"), 0, NULL };

FileType html = { _T(".html"), _T("html"), 0, NULL };
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
	if (memcmp(&fileType, &other, sizeof(FileType)) != 0)
	{
		return fileType;
	}
	
	return other;
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

	if (file.FindSubStringA(0, 20, "html") != -1)
		return html;
	if (file.FindSubStringW(0, 20, L"html") != -1)
		return html;

	return other;
}
