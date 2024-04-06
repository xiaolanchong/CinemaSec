// ArchiveServer.cpp: implementation of the CArchiveServer class.
//
//////////////////////////////////////////////////////////////////////

#include "chcs.h"
#include "ArchiveServer.h"
#include "ArchiveReader.h"

//////////////////////////////////////////////////////////////////////
// CArchiveServer
//////////////////////////////////////////////////////////////////////

CArchiveServer::CArchiveServer(
	unsigned long addressToListenOn,
	unsigned short portToListenOn,
	unsigned maxMessageSize,
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
	, m_lSessionRef(0)
	, m_videoFrames(maxMessageSize, 20) // ~= 4 connection
{
}

CArchiveServer::~CArchiveServer()
{
}

void CArchiveServer::OnConnectionEstablished(
	Elvees::CSocketServer::Socket *pSocket,
	Elvees::CIOBuffer *pAddress)
{
	const sockaddr_in* pSockAddrIn = reinterpret_cast<const sockaddr_in*>(pAddress->GetBuffer());

	long lSessionID  = InterlockedIncrement(&m_lSessionRef);
	LPCSTR stAddress = ::inet_ntoa(pSockAddrIn->sin_addr);

	if(stAddress)
	{
		Elvees::OutputF(Elvees::TInfo, TEXT("A[%ld] created (%hs)"),
				lSessionID, stAddress);
	}
	else
	{
		Elvees::OutputF(Elvees::TInfo, TEXT("A[%ld] created (UNK)"), lSessionID);
	}

	// Create reader for this session
	//

	CArchiveReader *pReader = new CArchiveReader(this, pSocket);

	if(pReader)
	{
		SetServerDataPtr(pSocket, reinterpret_cast<void*>(pReader));

		pSocket->SetSessionID(lSessionID);
		pSocket->Read();
	}
	else
	{
		static const char strError[] = "100 Internal Server Error\r\n";

		pSocket->Write(strError, lstrlenA(strError));
		pSocket->Shutdown();
	}
}

bool CArchiveServer::OnConnectionClosing(
	Elvees::CSocketServer::Socket *pSocket)
{
	// We'l perform a lingering close on this thread
	pSocket->Close();

	return true; // We'l handle the close on a worker thread
}

void CArchiveServer::OnConnectionClosed(
	Elvees::CSocketServer::Socket *pSocket)
{
	Elvees::OutputF(Elvees::TInfo, TEXT("A[%ld] closing"),
		pSocket->GetSessionID());

	CArchiveReader *pReader = reinterpret_cast<CArchiveReader*>(GetServerDataPtr(pSocket));

	if(pReader)
	{
		try
		{
			pReader->Shutdown();
			delete pReader;
		}
		catch(const Elvees::CException& e)
		{
			Elvees::OutputF(Elvees::TError,
				TEXT("A[%ld] OnConnectionClosed - %s - %s"),
				pSocket->GetSessionID(), e.GetWhere(), e.GetMessage());
		}
		catch(...)
		{
			Elvees::OutputF(Elvees::TError,
				TEXT("A[%ld] OnConnectionClosed - Unexpected exception"),
				pSocket->GetSessionID());
		}

		SetServerDataPtr(pSocket, NULL);
	}

	Elvees::OutputF(Elvees::TInfo, TEXT("A[%ld] closed"),
		pSocket->GetSessionID());

	pSocket->SetSessionID(-1);
}

void CArchiveServer::ReadCompleted(
	Elvees::CSocketServer::Socket *pSocket,
	Elvees::CIOBuffer *pBuffer)
{
	try
	{
		DEBUG_ONLY(Elvees::OutputF(Elvees::TTrace, TEXT("A[%ld] msg(%lX, %u)"),
			pSocket->GetSessionID(),
			pBuffer->GetSequenceNumber(),
			pBuffer->GetUsed()));

		pBuffer = ProcessDataStream(pSocket, pBuffer);
		pSocket->Read(pBuffer);
	}
	catch(const Elvees::CException& e)
	{
		Elvees::OutputF(Elvees::TError, TEXT("A[%ld] : ReadCompleted  Exception %s - %s"),
			pSocket->GetSessionID(),
			e.GetWhere(), e.GetMessage());
		
		pBuffer->Empty();
		pSocket->Shutdown();
	}
	catch(...)
	{
		Elvees::OutputF(Elvees::TError, TEXT("A[%ld] : ReadCompleted - Unexpected exception"), pSocket->GetSessionID());

		pBuffer->Empty();
		pSocket->Shutdown();
	}
}

//////////////////////////////////////////////////////////////////////
// Process
//////////////////////////////////////////////////////////////////////

unsigned CArchiveServer::GetMessageSize(const Elvees::CIOBuffer *pBuffer) const
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

Elvees::CIOBuffer* CArchiveServer::ProcessDataStream(
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
					static const char strError[] = "200 Invalid message size\r\n";

					pSocket->Write(strError, lstrlenA(strError));
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

void CArchiveServer::ProcessCommand(
	Elvees::CSocketServer::Socket *pSocket,
	Elvees::CIOBuffer *pBuffer)
{
	DEBUG_ONLY(Elvees::OutputF(Elvees::TTrace, TEXT("A[%ld] message(%u)"),
		pSocket->GetSessionID(),
		pBuffer->GetUsed()));

	CArchiveReader *pReader = 
		reinterpret_cast<CArchiveReader*>(GetServerDataPtr(pSocket));

	if(pReader)
	{
		try
		{
			pReader->ProcessCommand(
				reinterpret_cast<LPCSTR>(pBuffer->GetBuffer()),
				pBuffer->GetUsed());
		}
		catch(const Elvees::CException& e)
		{
			Elvees::OutputF(Elvees::TError,
				TEXT("A[%ld] : ProcessCommand  Exception %s - %s"),
				pSocket->GetSessionID(), e.GetWhere(), e.GetMessage());
		}
		catch(...)
		{
			Elvees::OutputF(Elvees::TError,
				TEXT("A[%ld] : ProcessCommand - Unexpected exception"),
				pSocket->GetSessionID());
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

LPCTSTR CArchiveServer::GetVideoBasePath()
{
	return GetManager()->GetVideoBasePath();
}

Elvees::CIOBuffer* CArchiveServer::AllocateFrame()
{
	return m_videoFrames.Allocate();
}