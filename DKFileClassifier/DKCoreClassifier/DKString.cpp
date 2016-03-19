
// author: Khiêm Đoàn Hoà - 2016-03-19

#include "stdafx.h"
#include "DKString.h"

std::wstring s2ws(std::string s)
{
	std::wstring ws;
	ws.assign(s.begin(), s.end());
	return ws;
}

std::string ws2s(std::wstring ws)
{
	std::string s;
	s.assign(ws.begin(), ws.end());
	return s;
}
