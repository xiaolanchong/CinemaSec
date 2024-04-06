#pragma once

#include <queue>
unsigned int __stdcall LauncherProc( LPVOID pParam );

/////////////////////////////////////////////////////////////////////////////
// Macroses

#define countof(x) (sizeof(x)/sizeof((x)[0]))

/////////////////////////////////////////////////////////////////////////////
// Output

void Output(LPCTSTR stText);
void OutputF(LPCTSTR stFormat, ...);

class MessagePipe
{
public:
	typedef	std::queue<int>	MsgQueue_t;
private:
	MsgQueue_t			m_MsgQueue;
	CRITICAL_SECTION	m_cs;
public:
	MessagePipe()
	{
		InitializeCriticalSection( &m_cs );
	}
	~MessagePipe()
	{
		DeleteCriticalSection( &m_cs );
	}
	
	enum
	{
		MsgExit,
		MsgRestart,
		MsgHide,
		MsgShow
	};

	void		SendMessage( int msg);
	MsgQueue_t*	LockQueue(bool bOnlyTry);
	void		UnlockQueue();

};

MessagePipe&	GetMessagePipe();