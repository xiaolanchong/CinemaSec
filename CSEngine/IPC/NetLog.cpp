
#include "stdafx.h"
#include "NetLog.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

NetLog::NetLog(const char* szFileName)
{
	m_f = fopen( szFileName, "w" );
	InitializeCriticalSection(&cs);
}

NetLog::~NetLog()
{
	DeleteCriticalSection(&cs);
	fclose(m_f);
}



BOOL NetLog::LogMessage( const char *szMsg)
{
	time(&ltime);

	if( (!strlen(szMsg)))
		return FALSE;

	EnterCriticalSection(&cs);
	if(m_f != NULL)				
	{
		newtime = localtime(&ltime);
		strftime(szDT, 128, "%a, %d %b %Y %H:%M:%S", newtime);
		
		sprintf(szMessage, "%s - %s.\n", szDT, szMsg);

		size_t n = fwrite(szMessage, sizeof(char), strlen(szMessage), m_f);
		if(n != strlen(szMessage))
		{
			LeaveCriticalSection(&cs);
			fclose(m_f);
			return FALSE;
		}
		LeaveCriticalSection(&cs);
		return TRUE;
	}

	LeaveCriticalSection(&cs);
	return FALSE;
}



BOOL NetLog::ClearLog(const char *szFileName)
{
	if(!strlen(szFileName))
		return FALSE;
		
	return 	DeleteFileA(szFileName);
}
