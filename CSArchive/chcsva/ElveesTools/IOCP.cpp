#include "IOCP.h"
#include "Exceptions.h"

///////////////////////////////////////////////////////////////////////////////
// Namespace: Elvees
///////////////////////////////////////////////////////////////////////////////

namespace Elvees {

///////////////////////////////////////////////////////////////////////////////
// CIOCP
///////////////////////////////////////////////////////////////////////////////

CIOCP::CIOCP(unsigned maxConcurrency)
	: m_iocp(::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, DWORD(maxConcurrency)))
{
	if(m_iocp == 0)
	{
		throw CWin32Exception(TEXT("CIOCP::CIOCP() - CreateIoCompletionPort"), ::GetLastError());
	}
}

CIOCP::~CIOCP() 
{
	if(m_iocp)
		::CloseHandle(m_iocp);
}

void CIOCP::AssociateDevice(HANDLE hDevice, DWORD completionKey)
{
	if(m_iocp != ::CreateIoCompletionPort(hDevice, m_iocp, completionKey, 0))
	{
		throw CWin32Exception(TEXT("CIOCP::AssociateDevice() - CreateIoCompletionPort"), ::GetLastError());
	}
}

void CIOCP::PostStatus(
	DWORD completionKey, 
	DWORD dwNumBytes /* = 0 */, 
	OVERLAPPED *pOverlapped /* = 0 */) 
{
	if(0 == ::PostQueuedCompletionStatus(m_iocp, dwNumBytes, completionKey, pOverlapped))
	{
		throw CWin32Exception(TEXT("CIOCP::PostStatus() - PostQueuedCompletionStatus"), ::GetLastError());
	}
}

bool CIOCP::GetStatus(
	DWORD *pCompletionKey, 
	PDWORD pdwNumBytes,
	OVERLAPPED **ppOverlapped, 
	DWORD dwMilliseconds /* = INFINITE */)
{
	bool ok = true;

	if(0 == ::GetQueuedCompletionStatus(m_iocp, pdwNumBytes, pCompletionKey, ppOverlapped, dwMilliseconds))
	{
		DWORD lastError = ::GetLastError();

		if(lastError != WAIT_TIMEOUT || dwMilliseconds == INFINITE)
		{
			throw CWin32Exception(TEXT("CIOCP::GetStatus() - GetQueuedCompletionStatus"), lastError);
		}

		ok = false;
	}

	return ok;
}

///////////////////////////////////////////////////////////////////////////////
// Namespace: Elvees
///////////////////////////////////////////////////////////////////////////////

} // End of namespace Elvees
