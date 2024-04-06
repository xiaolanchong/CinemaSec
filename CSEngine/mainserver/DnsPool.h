//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                              Copyright by ElVEES, 2005                               //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	DNS cache for unknown hosts
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 05.05.2005
//                                                                                      //
//======================================================================================//
#ifndef _DNS_POOL_3552911734350360_
#define _DNS_POOL_3552911734350360_

#include "../IPC/NetAddr.h"
#include "ThreadError.h"
//======================================================================================//
//                                    class DnsPool                                     //
//======================================================================================//

class DnsPool
{
	ThreadSync			m_Thread;
	struct Record
	{
		std::wstring	m_Name;
		netadr_t		m_NetAddr;
		INT64			m_Time;
	};

	std::list< Record >			m_NameCache;
	std::queue< std::wstring >	m_NameQueue;
	
	SyncCriticalSection			m_Sync;
	static unsigned	__stdcall ThreadProc( void * pParam);  
	static unsigned	__stdcall Handled_ThreadProc( void * pParam);  
public:
	DnsPool();
	virtual ~DnsPool();

	void	Start();
	void	Stop();
};

#endif // _DNS_POOL_3552911734350360_