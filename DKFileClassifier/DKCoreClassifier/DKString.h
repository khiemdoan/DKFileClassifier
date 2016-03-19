
// author: Khiêm Đoàn Hoà - 2016-03-19

#ifndef _DK_STRING_H_
#define _DK_STRING_H_

#include "string"
#include "iostream"

#ifndef	_tstring
#ifdef	UNICODE
#define		_tstring	std::wstring
#else
#define		_tstring	std::string
#endif
#endif

#ifndef	_tcout
#ifdef	UNICODE
#define		_tcout	std::wcout
#else
#define		_tcout	std::cout
#endif
#endif

std::wstring s2ws(std::string s);
std::string ws2s(std::wstring ws);

#endif // !_DK_STRING_H_
