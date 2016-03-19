
// author: Khiêm Đoàn Hoà - 2016-03-19

#include "stdafx.h"
#include "DKConfig.h"

DKConfig::DKConfig()
{
	fstream	fConfig;
	fConfig.open(sConfigFilePath, ios_base::in);
	
	if (fConfig.good() == TRUE)
	{
		while (fConfig.eof() == FALSE)
		{
			string line;
			getline(fConfig, line);
			string::size_type find = line.find('=');
			if (find != string::npos)
			{
				string key(line, 0, find);
				string value(line, find + 1);
				m_mapConfig[key] = value;
			}
		}
	}

	fConfig.close();
}

DKConfig::~DKConfig()
{
	fstream	fConfig;
	fConfig.open(sConfigFilePath, ios_base::out);

	for (auto it = m_mapConfig.begin(); it != m_mapConfig.end(); it++)
	{
		string line = it->first + "=" + it->second + "\n";
		fConfig.write(line.c_str(), line.size());
	}

	fConfig.close();
}

_tstring DKConfig::GetConfig(_tstring sKey)
{
	_tstring sValue;
	string k;
	string v;

#ifdef UNICODE
	k = ws2s(sKey);
#else
	k = sKey;
#endif

	auto find = m_mapConfig.find(k);
	if (find != m_mapConfig.end())
	{
		v = find->second;
	}

#ifdef UNICODE
	sValue = s2ws(v);
#else
	sValue = v;
#endif

	return sValue;
}

VOID DKConfig::SetConfig(_tstring sKey, _tstring sValue)
{
	string k;
	string v;

#ifdef UNICODE
	k = ws2s(sKey);
	v = ws2s(sValue);
#else
	k = sKey;
	v = sValue;
#endif

	m_mapConfig[k] = v;
}

VOID DKConfig::SetCopy(BOOL bCopy)
{
	if (bCopy == TRUE)
		m_mapConfig["copy"] = "true";
	else
		m_mapConfig["copy"] = "false";
}

BOOL DKConfig::IsCopy()
{
	BOOL bReturn = TRUE;

	auto find = m_mapConfig.find("copy");
	if (find != m_mapConfig.end())
	{
		if (find->second.compare("false") == 0)
		{
			bReturn = FALSE;
		}
	}

	return bReturn;
}
