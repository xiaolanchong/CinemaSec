#include "SocketServerUDP.h"
#include "Exceptions.h"
#include "SystemInfo.h"
#include "utils.h"

///////////////////////////////////////////////////////////////////////////////
// Namespace: Elvees
///////////////////////////////////////////////////////////////////////////////

namespace Elvees {

///////////////////////////////////////////////////////////////////////////////
// Static helper methods
///////////////////////////////////////////////////////////////////////////////

static unsigned CalculateNumberOfThreads(
   unsigned numThreads);

///////////////////////////////////////////////////////////////////////////////
// CSocketServerUDP
///////////////////////////////////////////////////////////////////////////////

CSocketServerUDP::CSocketServerUDP(
	unsigned long addressToListenOn,
	unsigned short portToListenOn,
	unsigned maxFreeSockets,
	unsigned maxFreeBuffers,
	unsigned recvsToPost,
	unsigned bufferSize,
	unsigned numThreads)
	: CIOBuffer::Allocator(bufferSize, maxFreeBuffers)
	, m_numThreads(CalculateNumberOfThreads(numThreads))
	, m_listeningSocket(INVALID_SOCKET)
	, m_iocp(0)
	, m_address(addressToListenOn)
	, m_port(portToListenOn)
	, m_maxFreeSockets(maxFreeSockets)
	, m_recvsToPost(recvsToPost)
{
}

CSocketServerUDP::~CSocketServerUDP()
{
	try
	{
		ReleaseSockets();
	}
	catch(...)
	{
	}
}

void CSocketServerUDP::ReleaseSockets()
{
	CCriticalSection::Owner lock(m_listManipulationSection);

	// release any remaining active sockets     
	while(m_activeList.Head())
	{
		ReleaseSocket(m_activeList.Head());
	}

	// destroy all free sockets
	while(m_freeList.Head())
	{
		DestroySocket(m_freeList.PopNode());
	}

	if(m_activeList.Count() + m_freeList.Count() != 0)
	{
		Output(TError, TEXT("CSocketServerUDP::ReleaseSockets() - Leaked sockets"));
	}
}

void CSocketServerUDP::ReleaseBuffers()
{
	Flush();
}

void CSocketServerUDP::StartAcceptingConnections()
{
	if(m_listeningSocket == INVALID_SOCKET)
	{
		OnStartAcceptingConnections();

		m_listeningSocket = CreateListeningSocket(m_address, m_port);

		// note: we're not really passing a pSocket here..
		m_iocp.AssociateDevice(reinterpret_cast<HANDLE>(m_listeningSocket), (DWORD)m_listeningSocket);

		m_stopAcceptingConnectionsEvent.Reset();
		m_acceptConnectionsEvent.Set();
	}
}

void CSocketServerUDP::StopAcceptingConnections()
{
	if(m_listeningSocket != INVALID_SOCKET)
	{
		m_acceptConnectionsEvent.Reset();
		m_stopAcceptingConnectionsEvent.Set();

		if(0 != ::closesocket(m_listeningSocket))
		{
			OutputF(TError, TEXT("CSocketServerUDP::StopAcceptingConnections() - closesocket err=0x%lX"), ::WSAGetLastError());
		}

		m_listeningSocket = INVALID_SOCKET;

		OnStopAcceptingConnections();
	}
}

void CSocketServerUDP::InitiateShutdown()
{
	// signal that the dispatch thread should shut down all worker threads and then exit
	StopAcceptingConnections();

	m_shutdownEvent.Set();

	OnShutdownInitiated();
}

void CSocketServerUDP::WaitForShutdownToComplete()
{
	// if we havent already started a shut down, do so...

	InitiateShutdown();
	Wait();
}

CSocketServerUDP::WorkerThread *CSocketServerUDP::CreateWorkerThread(CIOCP &iocp)
{
	return new WorkerThread(*this, iocp);
}

int CSocketServerUDP::Run()
{
	try
	{
		WorkerThread *pThread;	
		TNodeList<WorkerThread> workerlist;

		for(unsigned i = 0; i < m_numThreads; ++i)
		{
			pThread = CreateWorkerThread(m_iocp);

			workerlist.PushNode(pThread);

			pThread->Start();
		}

		while(!m_shutdownEvent.Wait(0))
		{
			HANDLE handlesToWaitFor[2];

			handlesToWaitFor[0] = m_shutdownEvent.GetEvent();
			handlesToWaitFor[1] = m_acceptConnectionsEvent.GetEvent();

			DWORD waitResult = ::WaitForMultipleObjects(2, handlesToWaitFor, false, INFINITE);

			if(waitResult == WAIT_OBJECT_0)
			{
				// Time to shutdown
				break;
			}
			else if(waitResult == WAIT_OBJECT_0 + 1)
			{
				// post X accepts on the socket and then wait for either the shutdown event or the 
				// stop accepting connections event

				for(unsigned i = 0; i < m_recvsToPost; ++i)
				{
					PostReadRequest();
				}

				handlesToWaitFor[1] = m_stopAcceptingConnectionsEvent.GetEvent();

				waitResult = ::WaitForMultipleObjects(2, handlesToWaitFor, false, INFINITE);
			}

			if(waitResult != WAIT_OBJECT_0 &&
				waitResult != WAIT_OBJECT_0 + 1)
			{
				OutputF(TError, TEXT("CSocketServerUDP::Run() - WaitForMultipleObjects err=0x%lX"), ::GetLastError());
			}
		}

		// Initialize shutdown
		for(unsigned i = 0; i < workerlist.Count(); ++i)
			m_iocp.PostStatus(0);

		while(!workerlist.Empty())
		{
			pThread = workerlist.PopLastNode();
			pThread->WaitForShutdown();

			delete pThread;
		}
	}
	catch(const CException& e)
	{
		OutputF(TCritical, TEXT("CSocketServerUDP::Run() - Exception %s - %s"), e.GetWhere(), e.GetMessage());
	}
	catch(...)
	{
		Output(TCritical, TEXT("CSocketServerUDP::Run() - Unexpected exception"));
	}

	OnShutdownComplete();

	return 0;
}

unsigned CSocketServerUDP::GetAddressSize() const
{
	return sizeof(SOCKADDR_IN);
}

CSocketServerUDP::Socket *CSocketServerUDP::AllocateSocket()
{
	CCriticalSection::Owner lock(m_listManipulationSection);

	Socket *pSocket = 0;

	if(!m_freeList.Empty())
	{
		pSocket = m_freeList.PopNode();

		pSocket->AddRef();
	}
	else
	{
		pSocket = new Socket(*this);
		OnConnectionCreated();
	}

	pSocket->Attach(m_listeningSocket);

	return pSocket;
}

void CSocketServerUDP::ReleaseSocket(Socket *pSocket)
{
	CCriticalSection::Owner lock(m_listManipulationSection);

	pSocket->RemoveFromList();
	pSocket->m_socket = INVALID_SOCKET;

	if(m_maxFreeSockets == 0 || m_freeList.Count() < m_maxFreeSockets)
	{
		m_freeList.PushNode(pSocket);
	}
	else
	{
		DestroySocket(pSocket);
	}
}

void CSocketServerUDP::DestroySocket(Socket *pSocket)
{
	delete pSocket;

	OnConnectionDestroyed();
}

void CSocketServerUDP::Write(Socket *pSocket, const char *pData, unsigned dataLength)
{
	// Post a write request to the iocp so that the actual socket write gets performed by
	// one of our IO threads...

	CIOBuffer *pBuffer = Allocate();

	pBuffer->AddData(pData, dataLength);
	pBuffer->SetOperation(IO_Write_Request);
	pBuffer->SetUserPtr(pSocket);

	pSocket->AddRef();

	m_iocp.PostStatus((DWORD)pSocket->m_socket, 0, pBuffer);
}

void CSocketServerUDP::Write(Socket *pSocket, CIOBuffer *pBuffer)
{
	// Post a write request to the iocp so that the actual socket write gets performed by
	// one of our IO threads...

	pBuffer->AddRef();
	pBuffer->SetOperation(IO_Write_Request);
	pBuffer->SetUserPtr(pSocket);

	pSocket->AddRef();

	m_iocp.PostStatus((DWORD)pSocket->m_socket, 0, pBuffer);
}

void CSocketServerUDP::PostReadRequest()
{
	// Post a read request to the iocp so that the actual socket read gets performed by
	// one of our IO threads...

	Socket    *pSocket = AllocateSocket();
	CIOBuffer *pBuffer = Allocate();

	pBuffer->SetOperation(IO_Read_Request);
	pBuffer->SetUserPtr(pSocket);

	m_iocp.PostStatus((DWORD)pSocket->m_socket, 0, pBuffer);
}

void CSocketServerUDP::ReadCompleted()
{
	PostReadRequest();
}

void CSocketServerUDP::WriteCompleted(Socket *pSocket, CIOBuffer *pBuffer)
{
#ifdef _DEBUG
	if(!pBuffer->IsOperationCompleted())
		Output(TTrace, TEXT("CSocketServerUDP::WriteCompleted - not all data was written"));
#endif
}

///////////////////////////////////////////////////////////////////////////////
// CSocketServerUDP::Socket
///////////////////////////////////////////////////////////////////////////////

CSocketServerUDP::Socket::Socket(CSocketServerUDP &server)
	: m_server(server)
	, m_socket(INVALID_SOCKET)
	, m_ref(1)
	, m_fromLen(server.GetAddressSize())
	, m_actualFromLen(m_fromLen)
	, m_pFrom(reinterpret_cast<sockaddr *>(new BYTE[m_fromLen]))
{
}

CSocketServerUDP::Socket::~Socket()
{
	delete m_pFrom;
}

void CSocketServerUDP::Socket::Attach(SOCKET theSocket)
{
	if(INVALID_SOCKET != m_socket)
	{
		throw CException(TEXT("CSocketServerUDP::Socket::Attach()"), TEXT("Socket already attached"));
	}

	m_socket = theSocket;
}

void CSocketServerUDP::Socket::AddRef()
{
	::InterlockedIncrement(&m_ref);
}

void CSocketServerUDP::Socket::Release()
{
	if(0 == ::InterlockedDecrement(&m_ref))
	{
		m_server.ReleaseSocket(this);
	}
}

void CSocketServerUDP::Socket::Write(const char *pData, 
									 unsigned dataLength)
{
	m_server.Write(this, pData, dataLength);
}

void CSocketServerUDP::Socket::Write(CIOBuffer *pBuffer)
{
	m_server.Write(this, pBuffer);
}

///////////////////////////////////////////////////////////////////////////////
// CSocketServerUDP::WorkerThread
///////////////////////////////////////////////////////////////////////////////

CSocketServerUDP::WorkerThread::WorkerThread(CSocketServerUDP &server, CIOCP &iocp)
	: m_iocp(iocp)
	, m_server(server)
{
	// All work done in initialiser list
}

CSocketServerUDP::WorkerThread::~WorkerThread()
{
}

void CSocketServerUDP::WorkerThread::WaitForShutdown()
{
	m_iocp.PostStatus(0);
	Wait();
}

int CSocketServerUDP::WorkerThread::Run()
{
	try
	{
		while(true)   
		{
			// continually loop to service IO completion packets

			bool closeSocket = false;

			DWORD  dwIoSize = 0;
			SOCKET socket = INVALID_SOCKET;

			CIOBuffer *pBuffer = 0;

			try
			{
				m_iocp.GetStatus((PDWORD_PTR)&socket, &dwIoSize, (OVERLAPPED**)&pBuffer);
			}
			catch(const CWin32Exception &e)
			{
				if( e.GetError() != ERROR_NETNAME_DELETED  &&
					e.GetError() != ERROR_PORT_UNREACHABLE &&
					e.GetError() != WSA_OPERATION_ABORTED)
				{
					throw;
				}

			//	DEBUG_ONLY(Output(TTrace, TEXT("IOCP error - client connection dropped")));
			}

			if(!socket)
			{
				// A completion key of 0 is posted to the iocp to request us to shut down...
				break;
			}

			m_server.OnThreadBeginProcessing();

			if(pBuffer)
			{
				Socket *pSocket = static_cast<Socket*>(pBuffer->GetUserPtr());

				const IO_Operation operation = static_cast<IO_Operation>(pBuffer->GetOperation());

				switch(operation)
				{
				case IO_Read_Request:
					{
						Read(pSocket, pBuffer);
					}
					break;

				case IO_Read_Completed:
					{
						if(0 != dwIoSize)
						{
							pBuffer->Use(dwIoSize);

							//	DEBUG_ONLY(Output(TEXT("RX: ") + ToString(pBuffer) + TEXT("\n") + DumpData(reinterpret_cast<const BYTE*>(pBuffer->GetWSABUF()->buf), dwIoSize, 40)));

							m_server.ReadCompleted();
							m_server.ReadCompleted(pSocket, pBuffer);
						}
						else
						{
							// client connection dropped...
							Output(TInfo, TEXT("ReadCompleted - 0 bytes - client connection dropped"));
						}

						pSocket->Release();
						pBuffer->Release();
					}
					break;

				case IO_Write_Request:
					{
						Write(pSocket, pBuffer);
					}
					break;

				case IO_Write_Completed:
					{
						pBuffer->Use(dwIoSize);

						m_server.WriteCompleted(pSocket, pBuffer);

						pSocket->Release();
						pBuffer->Release();
					}
					break;

				default:
					OutputF(TWarning, TEXT("CSocketServerUDP::WorkerThread::Run() - Unexpected operation"));
					break;
				} 
			}
			else
			{
				OutputF(TWarning, TEXT("CSocketServerUDP::WorkerThread::Run() - Unexpected - pBuffer is 0"));
			}

			m_server.OnThreadEndProcessing();
		} 
	}
	catch(const CException& e)
	{
		OutputF(TCritical, TEXT("CSocketServerUDP::WorkerThread::Run() - Exception: %s - %s"), 
			e.GetWhere(), e.GetMessage());
	}
	catch(...)
	{
		Output(TCritical, TEXT("CSocketServerUDP::WorkerThread::Run() - Unexpected exception"));
	}

	return 0;
}

void CSocketServerUDP::WorkerThread::Read(
	Socket *pSocket,
	CIOBuffer *pBuffer) const
{
	pBuffer->SetOperation(IO_Read_Completed);
	pBuffer->SetupRead();
	pBuffer->SetUserPtr(pSocket);

	pSocket->m_actualFromLen = pSocket->m_fromLen;

	DWORD dwNumBytes = 0;
	DWORD dwFlags = 0;

	if(SOCKET_ERROR == ::WSARecvFrom(
		pSocket->m_socket, 
		pBuffer->GetWSABUF(), 
		1, 
		&dwNumBytes,
		&dwFlags,
		pSocket->m_pFrom,
		&pSocket->m_actualFromLen,
		pBuffer, 
		NULL))
	{
		DWORD lastError = ::WSAGetLastError();

		if(ERROR_IO_PENDING != lastError)
		{
			OutputF(TError, TEXT("CSocketServerUDP::Read() - WSARecvFrom: %s"), GetLastErrorMessage(lastError));

			if(lastError == WSAECONNABORTED || 
				lastError == WSAECONNRESET ||
				lastError == WSAEDISCON)
			{
				/// 
			}

			pSocket->Release();
			pBuffer->Release();
		}
	}
}

void CSocketServerUDP::WorkerThread::Write(
	Socket *pSocket,
	CIOBuffer *pBuffer) const
{
	pBuffer->SetOperation(IO_Write_Completed);
	pBuffer->SetupWrite();
	pBuffer->SetUserPtr(pSocket);

	DWORD dwFlags = 0;
	DWORD dwSendNumBytes = 0;

	if(SOCKET_ERROR == ::WSASendTo(
		pSocket->m_socket,
		pBuffer->GetWSABUF(), 
		1, 
		&dwSendNumBytes,
		dwFlags,
		pSocket->m_pFrom,
		pSocket->m_fromLen,
		pBuffer, 
		NULL))
	{
		DWORD lastError = ::WSAGetLastError();

		if(ERROR_IO_PENDING != lastError)
		{
			OutputF(TError, TEXT("CSocketServerUDP::Write() - WSASend: %s"), GetLastErrorMessage(lastError));

			if( lastError == WSAECONNABORTED || 
				lastError == WSAECONNRESET ||
				lastError == WSAEDISCON)
			{
				//
			}

			pSocket->Release();
			pBuffer->Release();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// Static helper methods
///////////////////////////////////////////////////////////////////////////////

static unsigned CalculateNumberOfThreads(unsigned numThreads)
{
	if(numThreads == 0)
	{
		CSystemInfo systemInfo;

		numThreads = systemInfo.dwNumberOfProcessors * 2;
	}

	return numThreads;
}

///////////////////////////////////////////////////////////////////////////////
// Namespace: Elvees
///////////////////////////////////////////////////////////////////////////////

} // End of namespace Elvees
