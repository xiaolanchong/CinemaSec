// StreamServer.cpp: implementation of the CStreamServer class.
//
//////////////////////////////////////////////////////////////////////

#include "chcs.h"
#include "StreamServer.h"
#include "StreamServerEvents.h"

///////////////////////////////////////////////////////////////////////////////
// CStreamServer
///////////////////////////////////////////////////////////////////////////////

CStreamServer::CStreamServer(
	CStreamServerEvents* pCallback,
	unsigned long addressToListenOn,
	unsigned short portToListenOn,
	unsigned maxFreeSockets,
	unsigned maxFreeBuffers,
	unsigned bufferSize,
	unsigned numThreads)
	: Elvees::CSocketServer(
		addressToListenOn,
		portToListenOn,
		maxFreeSockets,
		maxFreeBuffers,
		bufferSize,
		numThreads)
	, m_pCallback(pCallback)
{
}

void CStreamServer::OnConnectionEstablished(
	Elvees::CSocketServer::Socket *pSocket,
	Elvees::CIOBuffer *pAddress)
{
	// Allocate per connection data
	//

	long lSessionID = 0;

	if(m_pCallback)
	{
		const sockaddr_in* pSockAddrIn = reinterpret_cast<const sockaddr_in*>(pAddress->GetBuffer());

		LPCSTR stAddress = ::inet_ntoa(pSockAddrIn->sin_addr);

		if(stAddress)
		{
		#ifndef UNICODE
			lSessionID = m_pCallback->OnConnectionEstablished(stAddress);
		#else
			WCHAR szAddress[16];

			MultiByteToWideChar(CP_ACP, 0, stAddress, lstrlenA(stAddress) + 1,
					szAddress, sizeof(szAddress));

			lSessionID = m_pCallback->OnConnectionEstablished(
				const_cast<LPCWSTR>(szAddress));
		#endif
		}
	}
	
	if(lSessionID < 0)
	{
		LPSTR strResponse = 
			(lSessionID == SERVER_FULL) ? "101 Server full\r\n" :
				"100 Internal Server Error\r\n";

		pSocket->Write(strResponse, lstrlenA(strResponse));
		pSocket->Shutdown();
		return;
	}

	pSocket->SetSessionID(lSessionID);
	pSocket->Read();
}

bool CStreamServer::OnConnectionClosing(
	Elvees::CSocketServer::Socket *pSocket)
{
	// We'l perform a lingering close on this thread
	pSocket->Close();

	return true;      // We'l handle the close on a worker thread
}

void CStreamServer::OnConnectionClosed(
	Elvees::CSocketServer::Socket *pSocket)
{
	m_pCallback->OnConnectionClosed(pSocket->GetSessionID());
	pSocket->SetSessionID(-1);
}

void CStreamServer::ReadCompleted(
	Elvees::CSocketServer::Socket *pSocket,
	Elvees::CIOBuffer *pBuffer)
{
	try
	{
		pBuffer = ProcessDataStream(pSocket, pBuffer);
		pSocket->Read(pBuffer);
	}
	catch(const Elvees::CException& e)
	{
		Elvees::OutputF(Elvees::TError, TEXT("S[%ld] : ReadCompleted  Exception %s - %s"),
			pSocket->GetSessionID(),
			e.GetWhere(), e.GetMessage());
		
		pBuffer->Empty();
		pSocket->Shutdown();
	}
	catch(...)
	{
		Elvees::OutputF(Elvees::TError, TEXT("S[%ld] : ReadCompleted - Unexpected exception"), pSocket->GetSessionID());

		pBuffer->Empty();
		pSocket->Shutdown();
	}
}

//////////////////////////////////////////////////////////////////////////
// Process
//////////////////////////////////////////////////////////////////////////

unsigned CStreamServer::GetMessageSize(const Elvees::CIOBuffer *pBuffer) const
{
	const BYTE *pData = pBuffer->GetBuffer();
	const unsigned used = pBuffer->GetUsed();

	for(unsigned i = 0; i < used - 1; ++i)
	{
		if(pData[i] == '\r' && pData[i + 1] == '\n')
		{
			// the end of the message is i+1
			// we actually want a count of characters, not a zero based
			// index, so we have to add 1...
			return i + 2;
		}
	}

	return 0;
}

Elvees::CIOBuffer* CStreamServer::ProcessDataStream(
	Elvees::CSocketServer::Socket *pSocket,
	Elvees::CIOBuffer *pBuffer)
{
	bool done;

	do
	{
		done = true;

		const unsigned used = pBuffer->GetUsed();

		if(used >= 4)
		{
			const unsigned messageSize = GetMessageSize(pBuffer);

			if(messageSize == 0)
			{
				// haven't got a complete message yet.
				// we null terminate our messages in the buffer, so we need to reserve
				// a byte of the buffer for this purpose...

				if(used == (pBuffer->GetSize() - 1))
				{
				//	Handle_Error(pSocket);
					pSocket->Shutdown();

					// throw the rubbish away
					pBuffer->Empty();

					done = true;
				}
			}
			else if(used == messageSize)
			{
				pBuffer->AddData(0);   // null terminate the command string;

				ProcessCommand(pSocket, pBuffer);

				pBuffer->Empty();

				done = true;
			}
			else if(used > messageSize)
			{
				// allocate a new buffer, copy the extra data into it and try again...
				Elvees::CIOBuffer *pMessage = pBuffer->SplitBuffer(messageSize);

				pMessage->AddData(0);   // null terminate the command string;

				ProcessCommand(pSocket, pMessage);

				pMessage->Release();

				// loop again, we may have another complete message in there...

				done = false;
			}
		}
	}
	while(!done);

	// not enough data in the buffer, re-issue a read into the same buffer to collect more data
	return pBuffer;
}

void CStreamServer::ProcessCommand(
	Elvees::CSocketServer::Socket *pSocket,
	Elvees::CIOBuffer *pBuffer)
{
	if(strncmp((char*)pBuffer->GetBuffer(), "quit", 4) == 0)
	{
		pSocket->Write("bye\r\n", 5);
		pSocket->Shutdown();
	}
	else if(strncmp((char*)pBuffer->GetBuffer(), "printsockstatus", 15) == 0)
	{
		DEBUG_ONLY(Elvees::OutputF(Elvees::TTrace, TEXT("S[%ld] %ld (%ld) write pending"),
			pSocket->GetSessionID(),
			pSocket->GetWriteCount(),
			pSocket->GetWriteBytesCount()));
	}
}

bool CStreamServer::SendFrame(Elvees::CIOBuffer *pData)
{
	BroadcastMessage(pData);
	return true;
}