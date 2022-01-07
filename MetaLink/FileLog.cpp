#include "FileLog.h"
#include "../BaseIncludes.h"
#ifdef FILE_LOG_DEFINED
void FileLogger::OnNewFame()
{
	double dbTime = GetFloatTime();
	if (dbTime > (m_dbLastWriteTime += (60.0 * 1000.0)))
	{
		m_dbLastWriteTime = dbTime;
		m_ofLog.close();
		m_ofLog.open(m_szFileName, std::ios::out);
	}
}

void FileLogger::LogMessage(const char* szMessage)
{
	char Buffer[4096];
	const char* Formatter =
	{
		" [%d : %d : %d] %s\n"
	};
	snprintf(Buffer, 4096, Formatter, 0, 0, 0, szMessage);
	m_ofLog.write(Buffer, min((int)strlen(Buffer), 4096));
}
#endif