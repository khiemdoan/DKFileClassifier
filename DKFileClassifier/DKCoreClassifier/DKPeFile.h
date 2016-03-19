
// author: Khiêm Đoàn Hoà - 2016-03-19

#ifndef _DK_PE_FILE_H_
#define _DK_PE_FILE_H_

#include "map"
#include "set"
#include "list"
using namespace std;

#include "DKFile.h"
#include "DKString.h"

class DKPeFile : public DKFile
{
public:
	DKPeFile(LPTSTR szFilePath);

	BOOL	CheckPe();
	BOOL	Check64();
	BOOL	CheckDll();

	BOOL	IsImportDll(LPTSTR szDllName);
	BOOL	IsImportFunction(LPTSTR szFunction);

	list<_tstring>	GetListDlls();
	list<_tstring>	GetListFunctionsByDll(LPTSTR szDllName);

private:
	BOOL	m_bPeFile = FALSE;
	BOOL	m_bFile64 = FALSE;

	DWORD	Rva2Offset(DWORD dwRva);

	BOOL	GetImports();
	map<string, set<string>>	m_import;
};

#endif // !_DK_PE_FILE_H_
