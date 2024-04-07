#if !defined(AFX_LOG_H__82043CA7_5940_4F7E_A316_7F91096B2008__INCLUDED_)
#define AFX_LOG_H__82043CA7_5940_4F7E_A316_7F91096B2008__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include <string>

using namespace std;

#define MAX_MSG_SIZE	1024

class NetLog  
{
public:
	BOOL ClearLog(const char* szFileName);
	BOOL LogMessage(const char* szMessage );
	NetLog(const char* szFileName);
	virtual ~NetLog();
private:
	FILE *m_f;
	char szLogFilePath[MAX_PATH];
	char szMessage[MAX_MSG_SIZE];
	char szDT[128];
	struct tm *newtime;
	time_t ltime;
	CRITICAL_SECTION cs;
};

#endif // !defined(AFX_LOG_H__82043CA7_5940_4F7E_A316_7F91096B2008__INCLUDED_)
