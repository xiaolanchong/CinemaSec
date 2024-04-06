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
#include "stdafx.h"
#include "DnsPool.h"
#include "../../CSChair/dmpstuff/ExceptionHandler.h"
//======================================================================================//
//                                    class DnsPool                                     //
//======================================================================================//
DnsPool::DnsPool()
{
}

DnsPool::~DnsPool()
{
}

void	DnsPool::Start()
{

}

void	DnsPool::Stop()
{

}

unsigned	__stdcall DnsPool::Handled_ThreadProc( void * pParam)
{
	__try
	{
		return ThreadProc( pParam );
	}
	__except(RecordExceptionInfo(GetExceptionInformation(), _T("DNS pool thread")), EXCEPTION_EXECUTE_HANDLER )
	{
		return ~0UL;
	}
}

unsigned	__stdcall DnsPool::ThreadProc( void * pParam)
{
	DnsPool* pThis = ( DnsPool* )pParam;
	return 0;
}