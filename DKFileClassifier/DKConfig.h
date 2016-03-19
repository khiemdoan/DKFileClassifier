
// author: Khiêm Đoàn Hoà - 2016-03-19

#pragma once

#include "windows.h"

#include "fstream"
#include "map"
using namespace std;

#include "DKCoreClassifier\DKString.h"

class DKConfig
{
public:
	DKConfig();
	~DKConfig();

	_tstring	GetConfig(_tstring sKey);
	VOID		SetConfig(_tstring sKey, _tstring sValue);
	
	VOID		SetCopy(BOOL bCopy);
	BOOL		IsCopy();

private:
	string				sConfigFilePath = "config.ini";
	map<string, string>	m_mapConfig;
};
