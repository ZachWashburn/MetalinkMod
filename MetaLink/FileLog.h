#pragma once
//#define FILE_LOG
//#ifdef _DEBUG 

// 7/10/2021 -- Strange Bug Where It crashes logging netchannel creation, no clue why. FIXME!

#if 1
#ifdef FILE_LOG
#define FILE_LOG
#define FILE_LOG_DEFINED
#include <fstream>
class FileLogger
{
public:
	FileLogger(const char* szFileName)
	{
		strcpy_s(m_szFileName, _MAX_PATH, szFileName);
		m_ofLog.open(m_szFileName, std::ios::out);
	}

	void LogMessage(const char* szMessage);
	void Close() { m_ofLog.close(); }
	void OnNewFame();
	double m_dbLastWriteTime = 0.0;
	char m_szFileName[_MAX_PATH];
	std::ofstream m_ofLog;
};

inline FileLogger g_FileLog("GoldenGun.log");
inline FileLogger* g_pFileLog = &g_FileLog;

#define LOG(VAR) g_pFileLog->LogMessage(VAR);
#define LOGCRITICAL(VAR) {g_pFileLog->LogMessage(VAR); g_pFileLog->Close();}

#endif // _DEBUG
#endif // FILE_LOG

#ifndef FILE_LOG_DEFINED

#define LOG(VAR) //
#define LOGCRITICAL(VAR)


#endif