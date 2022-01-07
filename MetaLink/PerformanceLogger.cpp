#include "PerformanceLogger.h"
#include "FileLog.h"
#ifdef __LOG_PERFORMANCE

void PerformanceLogger::EnterSection(CodeSection csSec)
{
	m_muLock.lock();
	m_csLastEnteredSection = csSec;
	m_Sections[csSec]->EnterSection();
	
	for (int i = 0; i < __CODESECTIONS; i++)
	{
		//__asm int 3
		if (m_csSectionHierarchy[i] == __SECTION_INVALID)
		{
			m_csSectionHierarchy[i] = csSec;
		}
	}
	
	m_muLock.unlock();
}



void PerformanceLogger::ExitSection(CodeSection csSec)
{
	m_muLock.lock();
	m_Sections[csSec]->ExitSection();

	for (int i = 0; i < __CODESECTIONS; i++)
	{
		if (m_csSectionHierarchy[i] == __SECTION_INVALID)
		{

			if (i != 0)
			{
				Assert(m_csSectionHierarchy[i - 1] == csSec);
				m_csSectionHierarchy[i - i] = __SECTION_INVALID;
			} 
		}
	}


	m_muLock.unlock();
}


void PerformanceLogger::LogMessage(const char* szMessage)
{
	m_muLock.lock();
	int nMessageSize = min(strlen(szMessage), 255);
	char* pMessage = (char*)malloc(nMessageSize);
	strcpy_s(pMessage, nMessageSize, szMessage);
	m_Sections[m_csLastEnteredSection]->ExitSection();

	m_muLock.unlock();
}


#endif // #ifdef __LOG_PERFORMANCE