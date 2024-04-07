#include "SocketServer.h"
#include "SystemInfo.h"
#include "Socket.h"
#include "Exceptions.h"
#include "utils.h"

#pragma warning( disable : 4127 ) // conditional expression is constant

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
// CSocketServer
///////////////////////////////////////////////////////////////////////////////

CSocketServer::CSocketServer(
	unsigned long addressToListenOn,
	unsigned short portToListenOn,
	unsigned maxFreeSockets,
	unsigned maxFreeBuffers,
	unsigned bufferSize /* = 1024 */,
	unsigned numThreads /* = 0 */,
	bool postZeroByteReads /* = false */)
	: CIOBuffer::Allocator(bufferSize, maxFreeBuffers)
	, m_numThreads(CalculateNumberOfThreads(numThreads))
	, m_listeningSocket(INVALID_SOCKET)
	, m_iocp(0)
	, m_address(addressToListenOn)
	, m_port(portToListenOn)
	, m_maxFreeSockets(maxFreeSockets)
	, m_postZeroByteReads(postZeroByteReads)
{
}

CSocketServer::~CSocketServer()
{
}

void CSocketServer::StartAcceptingConnections()
{
	if(m_listeningSocket == INVALID_SOCKET)
	{
		OnStartAcceptingConnections();

		m_listeningSocket = CreateListeningSocket(m_address, m_port);
   		m_acceptConnectionsEvent.Set();
	}
}

void CSocketServer::StopAcceptingConnections()
{
	if(m_listeningSocket != INVALID_SOCKET)
	{
		m_acceptConnectionsEvent.Reset();

		if(0 != ::closesocket(m_listeningSocket))
		{
			OutputF(TError, TEXT("CSocketServer::StopAcceptingConnections() - closesocket res=%d"), ::WSAGetLastError());
		}

		m_listeningSocket = INVALID_SOCKET;

		OnStopAcceptingConnections();
	}
}

void CSocketServer::InitiateShutdown()
{
	// signal that the dispatch thread should shut down all worker threads and then exit

	StopAcceptingConnections();

	{
		CCriticalSection::Owner lock(m_listManipulationSection);

		Socket *pSocket = m_activeList.Head();

		while (pSocket)
		{
			Socket *pNext = SocketList::Next(pSocket);

			pSocket->AbortiveClose();
			pSocket = pNext;
		}
	}

	m_shutdownEvent.Set();

	OnShutdownInitiated();
}

void CSocketServer::WaitForShutdownToComplete()
{
	// if we havent already started a shut down, do so...

	InitiateShutdown();

	Wait();

	ReleaseSockets();
	Flush();
}

CSocketServer::WorkerThread *CSocketServer::CreateWorkerThread(CIOCP &iocp)
{
	return new WorkerThread(*this, iocp);
}

SOCKET CSocketServer::CreateListeningSocket(unsigned long address, unsigned short port)
{
	SOCKET s = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED); 

	if(s == INVALID_SOCKET)
		throw CWin32Exception(TEXT("CreateListeningSocket"), ::WSAGetLastError());

	CSocket listeningSocket(s);
	CSocket::InternetAddress localAddress(address, port);

	if(!listeningSocket.Bind(localAddress))
		throw CWin32Exception(TEXT("Bind"), ::WSAGetLastError());

	if(!listeningSocket.Listen(5))
		throw CWin32Exception(TEXT("Listen"), ::WSAGetLastError());

	return listeningSocket.Detatch();
}

SOCKET CSocketServer::CreateOutboundSocket(
	unsigned long address,
	unsigned short port)
{
	CSocket outboundSocket;
	CSocket::InternetAddress localAddress(address, port);

	if(outboundSocket.Create() && outboundSocket.Bind(localAddress))
		return outboundSocket.Detatch();

	return NULL;
}

int CSocketServer::Run()
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

		HANDLE handlesToWaitFor[2];

		handlesToWaitFor[0] = m_shutdownEvent.GetEvent();
		handlesToWaitFor[1] = m_acceptConnectionsEvent.GetEvent();

		while(!m_shutdownEvent.Wait(0))
		{
			DWORD waitResult = ::WaitForMultipleObjects(2, handlesToWaitFor, false, INFINITE);

			if(waitResult == WAIT_OBJECT_0)
			{
				// Time to shutdown
				break;
			}
			else if(waitResult == WAIT_OBJECT_0 + 1)
			{
				// accept connections

				while(!m_shutdownEvent.Wait(0) && m_acceptConnectionsEvent.Wait(0))
				{
					CIOBuffer *pAddress = Allocate();

					int addressSize = (int)pAddress->GetSize();

					SOCKET acceptedSocket = ::WSAAccept(m_listeningSocket, 
						reinterpret_cast<sockaddr*>(const_cast<BYTE*>(pAddress->GetBuffer())), 
						&addressSize, 
						0,
						0);

					pAddress->Use(addressSize);

					if(acceptedSocket != INVALID_SOCKET)
					{
						Socket *pSocket = AllocateSocket(acceptedSocket);

						OnConnectionEstablished(pSocket, pAddress);

						pSocket->Release();
					}
					else if(m_acceptConnectionsEvent.Wait(0))
					{
						OutputF(TError, TEXT("CSocketServer::Run() - WSAAccept res=%d"), ::WSAGetLastError());
					}

					pAddress->Release();
				}
			}
			else
			{
				OutputF(TError, TEXT("CSocketServer::Run() - WaitForMultipleObjects res=%d"), ::GetLastError());
			}
		}

		// Initialize shutdown
		for(unsigned i = 0; i < workerlist.Count(); ++i)
			m_iocp.PostStatus(0);

		while(!workerlist.Empty())
		{
			pThread = workerlist.PopLastNode();
			pThread->WaitForShutdownToComplete();

			delete pThread;
		}
	}
	catch(const CException& e)
	{
		OutputF(TCritical, TEXT("CSocketServer::Run() - Exception: %s - %s"), e.GetWhere(), e.GetMessage());
	}
	catch(...)
	{
		Output(TCritical, TEXT("CSocketServer::Run() - Unexpected exception"));
	}

	OnShutdownComplete();

	return 0;
}

CSocketServer::Socket *CSocketServer::Connect(const SOCKADDR_IN &address)
{
	CSocket outSocket;
	CSocket::InternetAddress localAddress(m_address, 0);

	if(!outSocket.Create())
		return NULL;

	if(!outSocket.Bind(localAddress))
		return NULL;

	if(!outSocket.Connect(address))
		return NULL;

	return AllocateSocket(outSocket.Detatch());
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

CSocketServer::Socket* CSocketServer::CreateSocket(SOCKET theSocket)
{
	Socket *pSocket = new Socket(*this, theSocket);
	return pSocket;
}

void CSocketServer::DestroySocket(CSocketServer::Socket *pSocket)
{
	delete pSocket;
}

CSocketServer::Socket *CSocketServer::AllocateSocket(SOCKET theSocket)
{
	CCriticalSection::Owner lock(m_listManipulationSection);

	Socket *pSocket = 0;

	if(!m_freeList.Empty())
	{
		pSocket = m_freeList.PopNode();

		pSocket->Attach(theSocket);
		pSocket->AddRef();
	}
	else
	{
		pSocket = CreateSocket(theSocket);
		OnConnectionCreated();
	}

	m_activeList.PushNode(pSocket);

	m_iocp.AssociateDevice(reinterpret_cast<HANDLE>(theSocket), (DWORD)pSocket);

	return pSocket;
}

void CSocketServer::ReleaseSocket(Socket *pSocket)
{
   if(!pSocket)
   {
      throw CException(TEXT("CSocketServer::ReleaseSocket()"), TEXT("pSocket is null"));
   }

   CCriticalSection::Owner lock(m_listManipulationSection);

   pSocket->RemoveFromList();

   if(m_maxFreeSockets == 0 || m_freeList.Count() < m_maxFreeSockets)
   {
      m_freeList.PushNode(pSocket);
   }
   else
   {
      DestroySocket(pSocket);
	  OnConnectionDestroyed();
   }
}

void CSocketServer::ReleaseSockets()
{
	CCriticalSection::Owner lock(m_listManipulationSection);

	while(m_activeList.Head())
		ReleaseSocket(m_activeList.Head());

	while(m_freeList.Head())
	{
		DestroySocket(m_freeList.PopNode());
		OnConnectionDestroyed();
	}

	if(m_activeList.Count() + m_freeList.Count() != 0)
		Output(TError, TEXT("CSocketServer::ReleaseSockets() - Leaked sockets"));
}

///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////

void CSocketServer::PostIoOperation(Socket *pSocket, CIOBuffer *pBuffer, IO_Operation operation)
{
	pBuffer->SetOperation(operation);

	pBuffer->AddRef();
	pSocket->AddRef();

	m_iocp.PostStatus((DWORD)pSocket, 0, pBuffer);
}

void CSocketServer::WriteCompleted(Socket* pSocket, CIOBuffer *pBuffer)
{
#ifdef _DEBUG
	if(pSocket->IsConnected() && !pBuffer->IsOperationCompleted())
		Output(TTrace, TEXT("CSocketServer::WorkerThread::WriteCompleted - not all data was written"));
#endif
}

///////////////////////////////////////////////////////////////////////////////
// CSocketServer broadcast message
///////////////////////////////////////////////////////////////////////////////

void CSocketServer::BroadcastMessage(CIOBuffer *pMsg)
{
	if(!pMsg) return;

	CIOBuffer *pData;
	Socket* pSocket = m_activeList.Head();

	while(pSocket)
	{
		if(pSocket->GetWriteCount() < 10)
		{
			pData = pMsg->CreateCopy();

			if(pData)
			{
				pSocket->Write(pData);
				pData->Release();
			}
		}

		pSocket = SocketList::Next(pSocket);
	}
}

///////////////////////////////////////////////////////////////////////////////
// CSocketServer static functions
///////////////////////////////////////////////////////////////////////////////

void __cdecl CSocketServer::SetServerDataPtr(Socket *pSocket, void *pData)
{
	pSocket->SetServerDataPtr(pData);
}

void* __cdecl CSocketServer::GetServerDataPtr(const Socket *pSocket)
{
	return pSocket->GetServerDataPtr();
}

///////////////////////////////////////////////////////////////////////////////
// CSocketServer default notifications handlers
///////////////////////////////////////////////////////////////////////////////

void CSocketServer::OnStartAcceptingConnections()
{
	DEBUG_ONLY(Output(TTrace, TEXT("OnStartAcceptingConnections")));
}

void CSocketServer::OnStopAcceptingConnections()
{
	DEBUG_ONLY(Output(TTrace, TEXT("OnStopAcceptingConnections")));
}

void CSocketServer::OnShutdownInitiated()
{
	DEBUG_ONLY(Output(TTrace, TEXT("OnShutdownInitiated")));
}

void CSocketServer::OnShutdownComplete()
{
	DEBUG_ONLY(Output(TTrace, TEXT("OnShutdownComplete")));
}

void CSocketServer::OnConnectionCreated()
{
	DEBUG_ONLY(Output(TTrace, TEXT("OnConnectionCreated")));
}

void CSocketServer::OnConnectionDestroyed()
{
	DEBUG_ONLY(Output(TTrace, TEXT("OnConnectionDestroyed")));
}

void CSocketServer::OnConnectionClientClose(Socket* pSocket)
{
	DEBUG_ONLY(OutputF(TTrace, TEXT("OnConnectionClientClose [%08lX]"), pSocket));
}

bool CSocketServer::OnConnectionClosing(Socket* pSocket)
{
	return false;
}

void CSocketServer::OnConnectionClosed(Socket* pSocket)
{
	DEBUG_ONLY(OutputF(TTrace, TEXT("OnConnectionClosed [%08lX]"), pSocket));
}

void CSocketServer::OnConnectionReset(Socket* pSocket)
{
	DEBUG_ONLY(OutputF(TTrace, TEXT("OnConnectionReset [%08lX]"), pSocket));
}

void CSocketServer::OnConnectionError(
									  ConnectionErrorSource source,
									  Socket *pSocket,
									  CIOBuffer *pBuffer,
									  DWORD lastError)
{
	OutputF(TError,	TEXT("%s Socket=%08lX  Buffer=%08lX"),
		(source == ZeroByteReadError ? TEXT("Zero Byte Read Error: ") :
		(source == ReadError ? TEXT("Read Error:") : TEXT("Write Error:"))),
		pSocket, pBuffer);
}

//////////////////////////////////////////////////////////////////////////////
// CSocketServer::Socket
///////////////////////////////////////////////////////////////////////////////

CSocketServer::Socket::Socket(
	CSocketServer &server,
	SOCKET theSocket)
	: m_server(server)
	, m_socket(theSocket)
	, m_ref(1)
	, m_readShutdown(false)
	, m_writeShutdown(false)
	, m_closing(false)
	, m_clientClosed(false)
	, m_outOfSequenceWrites(m_crit)
{
	if(!IsValid())
	{
		throw CException(TEXT("CSocketServer::Socket::Socket()"), TEXT("Invalid socket"));
	}

	m_outstandingBytes = 0;
	m_outstandingWrites = 0;

	m_ReadSequenceNo = 0;
	m_WriteSequenceNo = 0;

	m_pServerData = NULL;
	m_lSessionID  = -1;
}

CSocketServer::Socket::~Socket()
{
}

void CSocketServer::Socket::Attach(SOCKET theSocket)
{
	if(IsValid())
	{
		throw CException(TEXT("CSocketServer::Socket::Attach()"), TEXT("Socket already attached"));
	}

	m_socket = theSocket;

	m_readShutdown = false;
	m_writeShutdown = false;
	m_outstandingWrites = 0;
	m_closing = false;
	m_clientClosed = false;

	m_ReadSequenceNo = 0;
	m_WriteSequenceNo = 0;
	m_outOfSequenceWrites.Reset();
}

void CSocketServer::Socket::AddRef()
{
	::InterlockedIncrement(&m_ref);
}

void CSocketServer::Socket::Release()
{
	if(0 == ::InterlockedDecrement(&m_ref))
	{
		if(IsValid())
		{
			AddRef();
      
			if(!m_closing)
			{
				m_closing = true;

				if(!m_server.OnConnectionClosing(this))
				{
					AbortiveClose();
				}
			}
			else
			{
				AbortiveClose();
			}

			Release();
			return;
		}

		m_server.ReleaseSocket(this);
	}
}

void CSocketServer::Socket::Shutdown(int how /* = SD_BOTH */)
{
//	DEBUG_ONLY(OutputF(TTrace, TEXT("CSocketServer::Socket::Shutdown(%ld)"), how));
	
	if(how == SD_RECEIVE || how == SD_BOTH)
		m_readShutdown = true;
	
	if(how == SD_SEND || how == SD_BOTH)
		m_writeShutdown = true;
	
	if(::InterlockedExchange(&m_outstandingWrites, m_outstandingWrites) > 0)
	{
		// Send side will be shut down when last pending write completes...
		
		if(how == SD_BOTH)
			how = SD_RECEIVE;
		else if(how == SD_SEND)
			return;
	}
	
	if(IsValid())
	{
		if(0 != ::shutdown(m_socket, how))
			OutputF(TError, TEXT("CSocketServer::Server::Shutdown() err=0x%lx"), ::WSAGetLastError());
	}
}

bool CSocketServer::Socket::IsConnected(int how /*= SD_BOTH*/) const
{
	if(how == SD_RECEIVE)
		return !m_readShutdown;

	if(how == SD_SEND)
		return !m_writeShutdown;

	if(how == SD_BOTH)
		return (!m_writeShutdown && !m_readShutdown);

	return false;
}

void CSocketServer::Socket::Close()
{
	CCriticalSection::Owner lock(m_crit);

	if(IsValid())
	{
		InternalClose();
	}
}

void CSocketServer::Socket::AbortiveClose()
{
	// Force an abortive close.
	LINGER lingerStruct;

	lingerStruct.l_onoff = 1;
	lingerStruct.l_linger = 0;

	if(SOCKET_ERROR == ::setsockopt(m_socket, SOL_SOCKET, SO_LINGER, (char *)&lingerStruct, sizeof(lingerStruct)))
	{
		OutputF(TError,	TEXT("CSocketServer::Socket::AbortiveClose() - setsockopt(SO_LINGER) err=0x%lx"), ::WSAGetLastError());
	}

	InternalClose();
}

void CSocketServer::Socket::OnClientClose()
{
	if(0 == ::InterlockedExchange(&m_clientClosed, 1))
	{
		Shutdown(SD_RECEIVE);

		m_server.OnConnectionClientClose(this);
	}
}

void CSocketServer::Socket::OnConnectionReset()
{
	CCriticalSection::Owner lock(m_crit);

	if(IsValid())
	{
		m_server.OnConnectionReset(this);

		InternalClose();
	}
}

void CSocketServer::Socket::InternalClose()
{
	CCriticalSection::Owner lock(m_crit);

	if(IsValid())
	{
		if(0 != ::closesocket(m_socket))
		{
			OutputF(TError,	TEXT("CSocketServer::Socket::InternalClose() - closesocket err=0x%lx"), ::WSAGetLastError());
		}

		m_socket = INVALID_SOCKET;

		m_readShutdown = true;
		m_writeShutdown = true;

		m_server.OnConnectionClosed(this);
	}
}

bool CSocketServer::Socket::Read(CIOBuffer *pBuffer /* = 0 */, bool throwOnFailure /* = false*/)
{
	if(!IsValid())
	{
		if(throwOnFailure)
		{
			// Todo throw SocketClosedException();
			throw CException(TEXT("CSocketServer::Socket::Read()"), TEXT("Socket is closed"));
		}
		else
		{
			return false;
		}
	}
	// Post a read request to the iocp so that the actual socket read gets performed by
	// one of the server's IO threads...

	if(!pBuffer)
	{
		pBuffer = m_server.Allocate();
	}
	else
	{
		pBuffer->AddRef();
	}

	m_server.PostIoOperation(this, pBuffer, m_server.m_postZeroByteReads ? IO_Zero_Byte_Read_Request : IO_Read_Request);

	pBuffer->Release();

	return true;
}

bool CSocketServer::Socket::Write(const char *pData, unsigned dataLength, bool throwOnFailure /* = false*/)
{
	return Write(reinterpret_cast<const BYTE*>(pData), dataLength, throwOnFailure);
}

bool CSocketServer::Socket::Write(const BYTE *pData, unsigned dataLength, bool throwOnFailure /* = false*/)
{
	if(dataLength == 0)
		return false;

	if(!IsValid())
	{
		if(throwOnFailure)
		{
			// Todo throw SocketClosedException();
			throw CException(TEXT("CSocketServer::Socket::Write()"), TEXT("Socket is closed"));
		}
		else
		{
			return false;
		}
	}

	if(!WritePending(dataLength))
	{
		if(throwOnFailure)
		{
			// Todo throw SocketClosedException();
			throw CException(TEXT("CSocketServer::Socket::Write()"), TEXT("Socket is shutdown"));
		}
		else
		{
			return false;
		}
	}

	CIOBuffer *pBuffer = m_server.Allocate();

	pBuffer->AddData(pData, dataLength);
	pBuffer->SetSequenceNumber(GetWriteSequenceNumber());

	m_server.PostIoOperation(this, pBuffer, IO_Write_Request);

	pBuffer->Release();

	return true;
}

bool CSocketServer::Socket::Write(CIOBuffer *pBuffer,  bool throwOnFailure /* = false*/)
{
	if(pBuffer->GetUsed() == 0)
		return false;

	if(!IsValid())
	{
		if(throwOnFailure)
		{
			// Todo throw SocketClosedException();
			throw CException(TEXT("CSocketServer::Socket::Write()"), TEXT("Socket is closed"));
		}
		else
		{
			return false;
		}
	}

	if(!WritePending(pBuffer->GetUsed()))
	{
		if(throwOnFailure)
		{
			// Todo throw SocketClosedException();
			throw CException(TEXT("CSocketServer::Socket::Write()"), TEXT("Socket is shutdown"));
		}
		else
		{
			return false;
		}
	}

	pBuffer->SetSequenceNumber(GetWriteSequenceNumber());

	m_server.PostIoOperation(this, pBuffer, IO_Write_Request);

	return true;
}

CSocketServer::Socket *CSocketServer::Socket::Connect(
	const sockaddr_in &address)
{
	return m_server.Connect(address);
}

bool CSocketServer::Socket::WritePending(long lBytes)
{
	if(m_writeShutdown)
	{
		DEBUG_ONLY(Output(TTrace, TEXT("CSocketServer::Socket::WritePending() - Attempt to write after write shutdown")));

		return false;
	}

	InterlockedIncrement(&m_outstandingWrites);
	InterlockedExchangeAdd(&m_outstandingBytes, lBytes);

	return true;
}

void CSocketServer::Socket::WriteCompleted(long lBytes)
{
	InterlockedExchangeAdd(&m_outstandingBytes, -lBytes);

	if(InterlockedDecrement(&m_outstandingWrites) == 0)
	{
		if(m_writeShutdown)
		{
			// The final pending write has been completed so we can now shutdown the send side of the
			// connection.

			Shutdown(SD_SEND);
		}
	}
}

long CSocketServer::Socket::GetWriteCount() const
{
	return ::InterlockedExchange(
		const_cast<long volatile*>(&m_outstandingWrites), m_outstandingWrites);
}

long CSocketServer::Socket::GetWriteBytesCount() const
{
	return ::InterlockedExchange(
		const_cast<long volatile*>(&m_outstandingBytes), m_outstandingBytes);
}

unsigned CSocketServer::Socket::GetReadSequenceNumber()
{
	return InterlockedExchangeAdd(
		reinterpret_cast<volatile long*>(&m_ReadSequenceNo), 1);
}

unsigned CSocketServer::Socket::GetWriteSequenceNumber()
{
	return InterlockedExchangeAdd(
		reinterpret_cast<volatile long*>(&m_WriteSequenceNo), 1);
}

bool CSocketServer::Socket::IsValid()
{
	return (INVALID_SOCKET != ::InterlockedExchange(
		reinterpret_cast<long volatile*>(&m_socket), (long)m_socket));
}

void CSocketServer::Socket::OnConnectionError(
	CSocketServer::ConnectionErrorSource source,
	CIOBuffer *pBuffer,
	DWORD lastError)
{
	if(WSAESHUTDOWN == lastError)
	{
		OnClientClose();
	}
	else if(WSAECONNRESET == lastError || WSAECONNABORTED == lastError)
	{
		OnConnectionReset();
	}
	else if(!IsValid() && WSAENOTSOCK == lastError)
	{
		// Swallow this error as we expect it...
	}
	else
	{
		m_server.OnConnectionError(source, this, pBuffer, lastError);
	}
}

///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////

void CSocketServer::Socket::SetSessionID(long lSessionID)
{
	m_lSessionID = lSessionID;
}

long CSocketServer::Socket::GetSessionID() const
{
	return m_lSessionID;
}


void* CSocketServer::Socket::GetServerDataPtr() const
{
	return InterlockedExchangePointer(&(const_cast<void*>(m_pServerData)), m_pServerData);
}

void  CSocketServer::Socket::SetServerDataPtr(void *pData)
{
	InterlockedExchangePointer(&m_pServerData, pData);
}

///////////////////////////////////////////////////////////////////////////////
// CSocketServer::WorkerThread
///////////////////////////////////////////////////////////////////////////////

CSocketServer::WorkerThread::WorkerThread(CSocketServer &server, CIOCP &iocp)
	: m_server(server)
	, m_iocp(iocp)
{
}

int CSocketServer::WorkerThread::Run()
{
	try
	{
		while(true)
		{
			// continually loop to service io-completion packets

			DWORD dwIoSize = 0;
			Socket *pSocket = 0;
			CIOBuffer *pBuffer = 0;
         
			bool weClosedSocket = false;

			try
			{
				m_iocp.GetStatus((DWORD*)&pSocket, &dwIoSize, (OVERLAPPED**)&pBuffer);
			}
			catch(const CWin32Exception &e)
			{
				if(e.GetError() == ERROR_NETNAME_DELETED || e.GetError() == ERROR_CONNECTION_ABORTED)
				{
					weClosedSocket = true;
				}
				else if(e.GetError() != WSA_OPERATION_ABORTED)
				{
					throw;
				}
            
				DEBUG_ONLY(Output(TTrace, TEXT("IOCP error - client connection dropped")));
			}

			if(!pSocket)
			{
				// A completion key of 0 is posted to the iocp
				// to request us to shut down...
				break;
			}

			m_server.OnThreadBeginProcessing();
   
			HandleOperation(pSocket, pBuffer, dwIoSize, weClosedSocket);

			m_server.OnThreadEndProcessing();
		} 
	}
	catch(const CException& e)
	{
		OutputF(TCritical, TEXT("CSocketServer::WorkerThread::Run() - Exception: %s - %s"), e.GetWhere(), e.GetMessage());
	}
	catch(...)
	{
		Output(TCritical, TEXT("CSocketServer::WorkerThread::Run() - Unexpected exception"));
	}

	return 0;
}

void CSocketServer::WorkerThread::WaitForShutdownToComplete()
{
	Wait();
}

void CSocketServer::WorkerThread::HandleOperation(
	Socket *pSocket,
	CIOBuffer *pBuffer,
	DWORD dwIoSize,
	bool weClosedSocket)
{
	if(pBuffer)
	{
		const IO_Operation operation = static_cast<IO_Operation>(pBuffer->GetOperation());

		switch(operation)
		{
		case IO_Zero_Byte_Read_Request:
			{
			//	DEBUG_ONLY(Output(TEXT("ZeroByteRead:") + ToString(pBuffer)));

				ZeroByteRead(pSocket, pBuffer);

				pSocket->Release();
				pBuffer->Release();
			}
			break;

		case IO_Zero_Byte_Read_Completed:
		case IO_Read_Request:
			{
			//	DEBUG_ONLY(Output(TEXT("IO_Zero_Byte_Read_Completed|IO_Read_Request:") + ToString(pBuffer)));

				Read(pSocket, pBuffer);
            
				pSocket->Release();
				pBuffer->Release();
			}
			break;

		case IO_Write_Request:
			{
			//	DEBUG_ONLY(Output(TEXT("IO_Write_Request:") + ToString(pBuffer)));
			//	DEBUG_ONLY(Output(TEXT("  TX: ") + ToString(pBuffer->GetUsed()) + TEXT(" bytes") + TEXT(" Seq: ") + ToString(pBuffer->GetSequenceNumber()) + TEXT(" : ") + ToString(pBuffer) + TEXT("\n")));
			//	DEBUG_ONLY(Output(TEXT("  TX: ") + ToString(pBuffer->GetUsed()) + TEXT(" bytes") + TEXT(" Seq: ") + ToString(pBuffer->GetSequenceNumber()) + TEXT(" : ") + ToString(pBuffer) + TEXT("\n") + DumpData(reinterpret_cast<const BYTE*>(pBuffer->GetBuffer()), pBuffer->GetUsed(), 40)));

				Write(pSocket, pBuffer);

				pSocket->Release();
				pBuffer->Release();
			}
			break;
   
		case IO_Read_Completed:
			{
				pBuffer->Use(dwIoSize);
            
			//	DEBUG_ONLY(OutputF(TTrace, TEXT("RX:%ld Seq:%u"),
			//		pBuffer->GetUsed(), pBuffer->GetSequenceNumber()));

			//	DEBUG_ONLY(Output(TEXT("IO_Read_Completed:") + ToString(pBuffer)));
			//	DEBUG_ONLY(Output(TEXT("  RX: ") + ToString(pBuffer->GetUsed()) + TEXT(" bytes") + TEXT(" Seq: ") + ToString(pBuffer->GetSequenceNumber()) + TEXT(" : ") + ToString(pBuffer) + TEXT("\n") + DumpData(reinterpret_cast<const BYTE*>(pBuffer->GetWSABUF()->buf), dwIoSize, 40)));
            
				if(0 != dwIoSize)
				{
					m_server.ReadCompleted(pSocket, pBuffer);
				}

				if(0 == dwIoSize && !weClosedSocket)
				{
					// client connection dropped?
					pSocket->OnClientClose();
				}

				pSocket->Release();
				pBuffer->Release();
			}
			break;

		case IO_Write_Completed:
			{
			//	DEBUG_ONLY(OutputF(TEXT("IO_Write_Completed:") + ToString(pBuffer)));

				pSocket->WriteCompleted(dwIoSize);

				pBuffer->Use(dwIoSize);
				m_server.WriteCompleted(pSocket, pBuffer);
				
				pSocket->Release();
				pBuffer->Release();
			}
			break;

		default:
			{
				Output(TError, TEXT("CSocketServer::WorkerThread::Run() - Unexpected operation"));
			}
			break;
		}
	}
	else
	{
		Output(TError, TEXT("CSocketServer::WorkerThread::Run() - Unexpected - pBuffer is 0"));
	}
}

void CSocketServer::WorkerThread::ZeroByteRead(Socket *pSocket, CIOBuffer *pBuffer) const
{
	pSocket->AddRef();

	pBuffer->SetOperation(IO_Zero_Byte_Read_Completed);
	pBuffer->SetupZeroByteRead();
	pBuffer->AddRef();

	DWORD dwNumBytes = 0;
	DWORD dwFlags = 0;

	if(SOCKET_ERROR == ::WSARecv(
		pSocket->m_socket,
		pBuffer->GetWSABUF(),
		1,
		&dwNumBytes,
		&dwFlags,
		pBuffer,
		NULL))
	{
		DWORD lastError = ::WSAGetLastError();

		if(ERROR_IO_PENDING != lastError)
		{
			pSocket->OnConnectionError(ZeroByteReadError, pBuffer, lastError);
			pSocket->Release();
			pBuffer->Release();
		}
	}
}

void CSocketServer::WorkerThread::Read(Socket *pSocket, CIOBuffer *pBuffer) const
{
	pSocket->AddRef();

	pBuffer->SetOperation(IO_Read_Completed);
	pBuffer->SetupRead();
	pBuffer->AddRef();

	CCriticalSection::Owner lock(pSocket->m_crit); 

	pBuffer->SetSequenceNumber(pSocket->GetReadSequenceNumber());

	DWORD dwNumBytes = 0;
	DWORD dwFlags = 0;

	if(SOCKET_ERROR == ::WSARecv(
		pSocket->m_socket,
		pBuffer->GetWSABUF(),
		1,
		&dwNumBytes,
		&dwFlags,
		pBuffer,
		NULL))
	{
		DWORD lastError = ::WSAGetLastError();

		if(ERROR_IO_PENDING != lastError)
		{
			pSocket->OnConnectionError(ReadError, pBuffer, lastError);
                  
			pSocket->Release();
			pBuffer->Release();
		}
	}
}

void CSocketServer::WorkerThread::Write(Socket *pSocket, CIOBuffer *pBuffer) const
{
	pBuffer->SetOperation(IO_Write_Completed);
	pBuffer->SetupWrite();

	pSocket->AddRef();
	pBuffer->AddRef();

	pBuffer = pSocket->m_outOfSequenceWrites.GetNext(pBuffer);

	while(pBuffer)
	{
		DWORD dwFlags = 0;
		DWORD dwSendNumBytes = 0;

		if(SOCKET_ERROR == ::WSASend(
			pSocket->m_socket,
			pBuffer->GetWSABUF(),
			1,
			&dwSendNumBytes,
			dwFlags,
			pBuffer,
			NULL))
		{
			DWORD lastError = ::WSAGetLastError();

			if(ERROR_IO_PENDING != lastError)
			{
				pSocket->OnConnectionError(WriteError, pBuffer, lastError);
            
				// this pending write will never complete...
				pSocket->WriteCompleted(0);

				pSocket->Release();
				pBuffer->Release();
			}
		}

		pBuffer = pSocket->m_outOfSequenceWrites.ProcessAndGetNext();
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