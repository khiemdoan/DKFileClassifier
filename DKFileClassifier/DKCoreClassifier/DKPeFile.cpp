
// author: Khiêm Đoàn Hoà - 2016-03-19

#include "stdafx.h"
#include "DKPeFile.h"

DKPeFile::DKPeFile(LPTSTR szFilePath) : DKFile(szFilePath)
{
	CheckPe();
	Check64();
	CheckDll();
	GetImports();
}

BOOL DKPeFile::CheckPe()
{
	if (m_dwFileSize < sizeof(IMAGE_DOS_HEADER))
		return FALSE;

	PIMAGE_DOS_HEADER	pDosHeader = NULL;
	pDosHeader = (PIMAGE_DOS_HEADER)m_lpAddress;
	if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
		return FALSE;

	if ((m_dwFileSize - pDosHeader->e_lfanew) < sizeof(IMAGE_NT_HEADERS))
		return FALSE;

	PIMAGE_NT_HEADERS	pNtHeader = NULL;
	pNtHeader = (PIMAGE_NT_HEADERS)((PBYTE)m_lpAddress + pDosHeader->e_lfanew);
	if (pNtHeader->Signature != IMAGE_NT_SIGNATURE)
		return FALSE;

	m_bPeFile = TRUE;
	return TRUE;
}

BOOL DKPeFile::Check64()
{
	if (m_bPeFile == FALSE)
		return FALSE;

	PIMAGE_NT_HEADERS	pNtHeader = NULL;
	pNtHeader = (PIMAGE_NT_HEADERS)((PBYTE)m_lpAddress + PIMAGE_DOS_HEADER(m_lpAddress)->e_lfanew);

	WORD wMachine = PIMAGE_NT_HEADERS(pNtHeader)->FileHeader.Machine;
	if ((wMachine & IMAGE_FILE_MACHINE_AMD64) == IMAGE_FILE_MACHINE_AMD64)
	{
		m_bFile64 = TRUE;
		return TRUE;
	}

	return FALSE;
}

BOOL DKPeFile::CheckDll()
{
	if (m_bPeFile == FALSE)
		return FALSE;

	PIMAGE_NT_HEADERS	pNtHeader = NULL;
	pNtHeader = (PIMAGE_NT_HEADERS)((PBYTE)m_lpAddress + PIMAGE_DOS_HEADER(m_lpAddress)->e_lfanew);

	WORD wCharacteristics = pNtHeader->FileHeader.Characteristics;
	if ((wCharacteristics & IMAGE_FILE_DLL) == IMAGE_FILE_DLL)
		return TRUE;

	return FALSE;
}

BOOL DKPeFile::IsImportDll(LPTSTR szDllName)
{
	// convert to std::string
#ifdef UNICODE
	string sDllName = ws2s(szDllName);
#else
	string sDllName = szDllName;
#endif

	// find dll in import
	auto find = m_import.find(sDllName);
	if (find != m_import.end())
		return TRUE;

	return FALSE;
}

BOOL DKPeFile::IsImportFunction(LPTSTR szFunction)
{
	// convert to std::string
#ifdef UNICODE
	string sFun = ws2s(szFunction);
#else
	string sFun = szFunction;
#endif

	BOOL	bReturn = FALSE;

	// find function in import
	auto it = m_import.begin();
	for (it = m_import.begin(); it != m_import.end(); it++)
	{
		auto it2 = it->second.find(sFun);
		if (it2 != it->second.end())
		{
			bReturn = TRUE;
			break;
		}
	}

	return bReturn;
}

list<_tstring> DKPeFile::GetListDlls()
{
	list<_tstring> listDlls;
	auto it = m_import.begin();
	for (it = m_import.begin(); it != m_import.end(); it++)
	{
#ifdef UNICODE
		listDlls.push_back(s2ws(it->first));
#else
		listDlls.push_back(it->first);
#endif
	}
	return listDlls;
}

list<_tstring> DKPeFile::GetListFunctionsByDll(LPTSTR szDllName)
{
	// convert to std::string
#ifdef UNICODE
	string sDll = ws2s(szDllName);
#else
	string sDll = szDllName;
#endif

	list<_tstring> listFunctions;

	// find dll
	auto find = m_import.find(sDll);
	if (find != m_import.end())
	{
		auto it = find->second.begin();
		for (it; it != find->second.end(); it++)
		{
#ifdef UNICODE
			listFunctions.push_back(s2ws(*it));
#else
			listFunctions.push_back(*it);
#endif
		}
	}

	return listFunctions;
}

DWORD DKPeFile::Rva2Offset(DWORD dwRva)
{
	PIMAGE_NT_HEADERS	pNtHeader = NULL;
	pNtHeader = (PIMAGE_NT_HEADERS)((PBYTE)m_lpAddress + PIMAGE_DOS_HEADER(m_lpAddress)->e_lfanew);

	PIMAGE_SECTION_HEADER pSech = IMAGE_FIRST_SECTION(pNtHeader);

	int i = 0;
	for (i = 0; i < pNtHeader->FileHeader.NumberOfSections; i++)
	{
		if (dwRva >= pSech->VirtualAddress && dwRva < pSech->VirtualAddress + pSech->Misc.VirtualSize)
		{
			break;
		}
		pSech++;
	}

	if (i >= pNtHeader->FileHeader.NumberOfSections)
		return 0;

	return dwRva - pSech->VirtualAddress + pSech->PointerToRawData;
}

BOOL DKPeFile::GetImports()
{
	if (m_bPeFile == FALSE)
		return FALSE;

	PIMAGE_NT_HEADERS	pNtHeader = NULL;
	pNtHeader = (PIMAGE_NT_HEADERS)((PBYTE)m_lpAddress + PIMAGE_DOS_HEADER(m_lpAddress)->e_lfanew);

	if (pNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size == 0)
		return FALSE;

	DWORD	dwVirAddress = 0;
	dwVirAddress = pNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;

	DWORD dwOffset = Rva2Offset(dwVirAddress);

	PIMAGE_IMPORT_DESCRIPTOR pImportDescriptor = NULL;
	pImportDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)((PBYTE)m_lpAddress + dwOffset);

	while (pImportDescriptor->OriginalFirstThunk)
	{
		DWORD dwOffsetName = Rva2Offset(pImportDescriptor->Name);
		string sDllName = (PCHAR)((PBYTE)m_lpAddress + dwOffsetName);

		DWORD dwOriginalFirstThunkOffset = Rva2Offset(pImportDescriptor->OriginalFirstThunk);
		
		PIMAGE_THUNK_DATA pImageThunkData = NULL;
		pImageThunkData = (PIMAGE_THUNK_DATA)((PBYTE)m_lpAddress + dwOriginalFirstThunkOffset);

		set<string>	setFunctions;

		while (pImageThunkData->u1.Function)
		{
			DWORD dwFunctionName = Rva2Offset(pImageThunkData->u1.AddressOfData);
			if (dwFunctionName != 0)
			{
				PIMAGE_IMPORT_BY_NAME pImageImportByName = NULL;
				pImageImportByName = (PIMAGE_IMPORT_BY_NAME)((PBYTE)m_lpAddress + dwFunctionName);
				setFunctions.insert((char*)pImageImportByName->Name);
			}
			pImageThunkData++;
		}
		
		m_import[sDllName] = setFunctions;

		pImportDescriptor++;
	}

	return TRUE;
}
