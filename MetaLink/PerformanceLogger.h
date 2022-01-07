#pragma once
#include <memory>
#include <mutex>
#include "GlobalHelpers.h"
#define __LOG_PERFORMANCE

#ifdef __LOG_PERFORMANCE
#define NUMBER_OF_TIME_SAMPLES 256
#define MAX_NUMBER_OF_MESSAGES 256

enum CodeSection
{
	__SECTION_INVALID,
	__SECTION_FRAMEINPUT,
	__SECTION_CREATEMOVE,
	__SECTION_BEGINFRAME,
	__SECTION_PRESENT,
	__SECTION_CL_MOVE,
	__SECTION_RENDER,
	__SECTION_RENDERFRAMEUPDATE,
	__SECTION_DRAWINDEXEDPRIMITIVE,
	__SECTION_VERTEXANDINDEXBUFFER,
	__SECTION_RENDER3D,
	__SECTION_ALL2DRENDERING,
	__SECTION_ALL3DRENDERING,

	__CODESECTIONS,
	__CODESECTION_MAX = __CODESECTIONS - 1,
};


class Section {
public:
	Section()
	{
		memset(m_arrMessages, 0, MAX_NUMBER_OF_MESSAGES);
		memset(m_dbSamples, 0, NUMBER_OF_TIME_SAMPLES);
	};

	void __forceinline AddWarning(char* cWarning)
	{
		m_muLock.lock();
		if (m_nNumOfMessages == MAX_NUMBER_OF_MESSAGES)
		{
			free(m_arrMessages[MAX_NUMBER_OF_MESSAGES - 1]);
			for (int i = MAX_NUMBER_OF_MESSAGES - 1; i > 0; i--)
			{
				m_arrMessages[i] = m_arrMessages[i-1];
			}
			m_arrMessages[0] = cWarning;
		}
		else {
			m_arrMessages[m_nNumOfMessages] = cWarning;
			m_nNumOfMessages++;
		}
		m_muLock.unlock();
	}

	void __forceinline AddSample(double Sample)
	{
		m_muLock.lock();

		if (Sample > m_dbHighestTime)
			m_dbHighestTime = Sample;
		else if (Sample < m_dbLowestTime)
			m_dbLowestTime = Sample;

		if (m_nNumOfSamples == NUMBER_OF_TIME_SAMPLES)
		{
			for (int i = NUMBER_OF_TIME_SAMPLES - 1; i > 0; i--)
			{
				m_dbSamples[i] = m_dbSamples[i - 1];
			}
			m_dbSamples[0] = Sample;
		}
		else {
			m_dbSamples[m_nNumOfSamples] = Sample;
			m_nNumOfSamples++;
		}
		m_muLock.unlock();
	}

	double __forceinline CalculateAverageTime()
	{
		m_muLock.lock();
		double dbAvg = 0;
		for (int i = 0; i < m_nNumOfSamples; i++)
		{
			dbAvg += m_dbSamples[i];
		}
		m_muLock.unlock();
		return dbAvg / m_nNumOfSamples;
	}

	double __forceinline GetHighestTime()
	{
		return m_dbHighestTime;
	}

	double __forceinline GetLowestTime()
	{
		return m_dbLowestTime;
	}

	void __forceinline EnterSection()
	{
		m_muLock.lock();
		m_dbStartTime = GetFloatTime();
		m_muLock.unlock();
	}

	void __forceinline ExitSection()
	{
		m_muLock.lock();
		m_dbLastSectionTime = GetFloatTime() - m_dbStartTime;
		m_muLock.unlock();
		AddSample(m_dbLastSectionTime);
	}

	void __forceinline SetSectionName(const char* szSectionName)
	{
		int nMessageSize = min(strlen(szSectionName), 255);
		memcpy(m_SectionName, szSectionName, nMessageSize);
		m_SectionName[nMessageSize] = '\0'; //  Yeah ik, whatever.
	}

	const char* GetSectionName()
	{
		return m_SectionName;
	}

private:
	char m_SectionName[256];
	double m_dbStartTime = 0;
	double m_dbLastSectionTime = 0;
	double m_dbLowestTime = 9999.9;
	double m_dbHighestTime = 0;
	double m_dbSamples[NUMBER_OF_TIME_SAMPLES];
	int m_nNumOfSamples = 0;
	char* m_arrMessages[MAX_NUMBER_OF_MESSAGES];
	int m_nNumOfMessages = 0;
	std::mutex m_muLock;
};

#define STR(str1, str2) 
#define SECTIONSWITCH(_Section) case _Section##:{ m_Sections[i]->SetSectionName(#_Section"\0");} break;
class PerformanceLogger
{
public:

	PerformanceLogger()
	{
		for (int i = 0; i < __CODESECTIONS; i++)
		{
			m_Sections[i] = new Section();
			switch (i)
			{
				SECTIONSWITCH(__SECTION_INVALID)
				SECTIONSWITCH(__SECTION_FRAMEINPUT)
				SECTIONSWITCH(__SECTION_CL_MOVE)
				SECTIONSWITCH(__SECTION_CREATEMOVE)
				SECTIONSWITCH(__SECTION_BEGINFRAME)
				SECTIONSWITCH(__SECTION_PRESENT)
				SECTIONSWITCH(__SECTION_RENDER)
				SECTIONSWITCH(__SECTION_RENDERFRAMEUPDATE);
				SECTIONSWITCH(__SECTION_DRAWINDEXEDPRIMITIVE);
				SECTIONSWITCH(__SECTION_VERTEXANDINDEXBUFFER);
				SECTIONSWITCH(__SECTION_RENDER3D);
				SECTIONSWITCH(__SECTION_ALL2DRENDERING);
				SECTIONSWITCH(__SECTION_ALL3DRENDERING);
			}
		}
	}

	void EnterSection(CodeSection Section);
	void ExitSection(CodeSection Section);
	void LogMessage(const char* szMessage);
	void LogAction(const char* Action) { m_szLastAction = Action; } // MAKE SURE ACTION IS A CONST CHAR*
	const char* GetCurrentSectionName() { return m_Sections[m_csLastEnteredSection]->GetSectionName(); }
	double GetSectionHighestTime(CodeSection csSec) { return m_Sections[csSec]->GetHighestTime(); }
	double GetSectionLowestTime(CodeSection csSec) { return m_Sections[csSec]->GetLowestTime(); }
	double GetSectionAverageTime(CodeSection csSec) { return m_Sections[csSec]->CalculateAverageTime(); }
	const char* GetSectionName(CodeSection csSec) { return m_Sections[csSec]->GetSectionName(); }
	CodeSection GetCurrentSection() { return m_csLastEnteredSection; }
private:
	Section* m_Sections[__CODESECTIONS];
	CodeSection m_csSectionHierarchy[__CODESECTIONS];
	CodeSection m_csLastEnteredSection = __SECTION_INVALID;
	std::mutex m_muLock;
	const char* m_szLastAction = 0;
};






inline PerformanceLogger g_Logger;
inline PerformanceLogger* g_pLogger = &g_Logger;

#define SECTION(_Section) g_pLogger->EnterSection(_Section);
#define SCOPESECTION(_Section) ScopeSection __SCOPE_SECTION(_Section);
#define ENDSECTION(_Section) g_pLogger->ExitSection(_Section);
#define LOGMESSAGE(MESSAGE) g_pLogger->LogMessage(MESSAGE);
#define LOGACTION(ACTION) g_pLogger->LogAction(ACTION);
class ScopeSection
{
public:
	ScopeSection(CodeSection sec) : m_csSec(sec)
	{
		SECTION(sec);
	}

	~ScopeSection()
	{
		ENDSECTION(m_csSec);
	}
private:
	CodeSection m_csSec = __SECTION_INVALID;
};


#else
#define SECTION(_Section)
#define SCOPESECTION(_Section) 
#define ENDSECTION(_Section) 
#define LOGMESSAGE(MESSAGE) 
#define LOGACTION(ACTION)
#endif // #ifdef __LOG_PERFORMANCE


