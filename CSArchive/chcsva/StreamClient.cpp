// StreamClient.cpp: implementation of the CStreamClient class.
//
//////////////////////////////////////////////////////////////////////

#include "chcs.h"
#include "StreamClient.h"
#include "NetProtocol.h"

///////////////////////////////////////////////////////////////////////////////
// CStreamClient
///////////////////////////////////////////////////////////////////////////////

CStreamClient::CStreamClient(
	CManager* pManager,
	unsigned nConnections,
	unsigned maxMessageSize,
	unsigned bufferSize,
	unsigned numThreads)
	: Elvees::CSocketServer(
		INADDR_ANY,	0,  // server : port
		nConnections == 0 ? 10 : nConnections,
		nConnections == 0 ? 10 : nConnections,
		max(1024, bufferSize),
		numThreads)
	, m_pManager(pManager)
	, m_maxMessageSize(maxMessageSize)
	, m_allocator(maxMessageSize, 2*nConnections)
{
}

///////////////////////////////////////////////////////////////////////////////
// ClientSocket Create/Destroy

Elvees::CSocketServer::Socket* CStreamClient::CreateSocket(SOCKET theSocket)
{
	ClientSocket* pCntSocket = new ClientSocket(
		*this, theSocket, m_criticalSection, m_allocator);

	return (Elvees::CSocketServer::Socket*)pCntSocket;
}

void CStreamClient::DestroySocket(Elvees::CSocketServer::Socket *pSocket)
{
	ClientSocket* pCntSocket = (ClientSocket*)pSocket;

	delete pCntSocket;
}

///////////////////////////////////////////////////////////////////////////////
// CSocketServer notifications

void CStreamClient::OnConnectionEstablished(
	Elvees::CSocketServer::Socket *pSocket,
	Elvees::CIOBuffer *pAddress)
{
	// No input connection allowed
	pSocket->Shutdown();
}

bool CStreamClient::OnConnectionClosing(
	Elvees::CSocketServer::Socket *pSocket)
{
	// Socket final release called
	pSocket->Close();
	return true;
}

void CStreamClient::OnConnectionClosed(
	Elvees::CSocketServer::Socket *pSocket)
{
	ClientSocket* pCntSocket = (ClientSocket*)pSocket;

	Elvees::OutputF(Elvees::TInfo, TEXT("Client %hs stopped"),
		inet_ntoa(*(in_addr*)&pCntSocket->m_address));

	m_pManager->ClientStreamStopped(pCntSocket->m_address);
	pCntSocket->ResetClientSocket();
}

///////////////////////////////////////////////////////////////////////////////
// Handle read buffers

void CStreamClient::ReadCompleted(
	Elvees::CSocketServer::Socket *pSocket,
	Elvees::CIOBuffer *pBuffer)
{
	ClientSocket* pCntSocket = (ClientSocket*)pSocket;

	try
	{
		pBuffer->AddRef();

		// Ensure in order buffer reads
		pBuffer = pCntSocket->GetNextBuffer(pBuffer);

		while(pBuffer)
		{
			ProcessDataStream(pCntSocket, pBuffer);

			pBuffer->Release();
			pBuffer = pCntSocket->ProcessAndGetNext();
		}

		pSocket->Read();
	}
	catch(const Elvees::CException& e)
	{
		Elvees::OutputF(Elvees::TError, TEXT("CStreamClient::ReadCompleted %s %s"),
			e.GetWhere(), e.GetMessage());
		pSocket->Shutdown();
	}
}

void CStreamClient::ProcessDataStream(ClientSocket* pSocket, Elvees::CIOBuffer *pBufferIn)
{
	// Create and initialize if needed..
	Elvees::CIOBuffer *pBuffer = pSocket->GetTransferBuffer();
	
	// add buffer to result buffer
	pBuffer->AddData(pBufferIn->GetBuffer(), pBufferIn->GetUsed());

	/////////////////////////////////////////////////////////////////////////

	bool done;

	do
	{
		done = true;

		const unsigned used = pBuffer->GetUsed();
		const unsigned messageSize = GetMessageSize(pBuffer);

		if(messageSize == 0)
		{
			// haven't got a complete message yet.
		}
		else if(used == messageSize)
		{
			// we have a whole, distinct, message

			ProcessCommand(pSocket, pBuffer);

			pBuffer->Release();

			pSocket->RemoveTransferBuffer();

			done = true;
		}
		else if(used > messageSize)
		{
			// we have a message, plus some more data
			// allocate a new buffer, copy the extra data into it and try again...

			Elvees::CIOBuffer *pMessage = pBuffer->SplitBuffer(messageSize);

			ProcessCommand(pSocket, pMessage);

			pMessage->Release();

			// loop again, we may have another complete message in there...
			done = false;
		}
	}
	while(!done);
}

unsigned CStreamClient::GetMessageSize(const Elvees::CIOBuffer *pBuffer) const
{
	const BYTE *pData = pBuffer->GetBuffer();
	const unsigned used = pBuffer->GetUsed();

	unsigned i, uCheckSum;
	PCNetPacketHeader pHeader;

	if(used < sizeof(NetPacketHeader))
		return 0;

	// Search for synchro symbol

	for(i = 0; i < used - 4; i++)
	{
		if( pData[i    ] == CHCS_NET_SYNCHRO_B1 &&
			pData[i + 1] == CHCS_NET_SYNCHRO_B2 &&
			pData[i + 2] == CHCS_NET_SYNCHRO_B3 &&
			pData[i + 3] == CHCS_NET_SYNCHRO_B4)
		{
			if(i != 0) // drop spam and align data
				return i;
		
			// synchro symbol found lets analyze
			// used >= sizeof(NetPacketHeader) && i=0;

			pHeader = reinterpret_cast<PCNetPacketHeader>(pData);
			
			uCheckSum =
				pHeader->uPacket +
				pHeader->uLength +
				pHeader->uTime;

			if(uCheckSum != pHeader->uCheckSum)
				continue;

			// Found real packet
			if(used >= pHeader->uLength)
			{
				// 
				return pHeader->uLength;
			}

			if(used + GetBufferSize() <= m_maxMessageSize)
				return 0; // haven't got a complete message yet.
			
			// Message too big, lets drop it...

			DEBUG_ONLY(Elvees::OutputF(Elvees::TTrace, 
				TEXT("Too big (%ld) message received."), pHeader->uLength));

			break;
		}
	}

	// synchro not found drop data
	// last 3 can be synchro part
	return used - 3;
}

void CStreamClient::ProcessCommand(ClientSocket* pSocket, Elvees::CIOBuffer *pBuffer)
{
	const BYTE *pData = pBuffer->GetBuffer();
	const unsigned used = pBuffer->GetUsed();

	PCNetPacketHeader pHeader = reinterpret_cast<PCNetPacketHeader>(pData);

	// Filter spam
	//

	if(used < sizeof(NetPacketHeader))
	{
		DEBUG_ONLY(Elvees::Output(Elvees::TTrace, TEXT("CStreamClient - Invalid packet")));
		return;
	}

	if(pHeader->uSynchro != CHCS_NET_SYNCHRO || pHeader->uLength != used)
	{
		DEBUG_ONLY(Elvees::Output(Elvees::TTrace, TEXT("CStreamClient - Broken packet header")));
		return;
	}

	// Yeah.. we have right packet
	// Handle it

	if(pHeader->uPacket == NET_PACKET_STREAMDATA)
	{
		if(pHeader->uLength > sizeof(NetPacketHeader) + sizeof(NetStreamDataInfo))
		{
			PCNetStreamDataInfo pDataInfo = reinterpret_cast<PCNetStreamDataInfo>(pHeader + 1);
			unsigned uSize = pHeader->uLength - sizeof(NetPacketHeader) - sizeof(NetStreamDataInfo);

			if(pDataInfo->uFrameSize != uSize)
				Elvees::Output(Elvees::TWarning, TEXT("Packet size mismatch!"));

			LPCVOID pCompressed = reinterpret_cast<LPCVOID>(pDataInfo + 1);

			CStream* pStream = m_pManager->StreamByUID(*const_cast<UUID*>(&pDataInfo->uStreamUID));

			if(pStream)
			{
				pStream->DecompressData(
					pCompressed,
					pDataInfo->uFrameSize,
					pDataInfo->uFrameSeq);

				pStream->Release();
			}
			else
			{
				DEBUG_ONLY(Elvees::Output(Elvees::TTrace, TEXT("CStreamClient - Unknown stream packet.")));
			}
		}
	}
	else if(pHeader->uPacket == NET_PACKET_STREAMINFO)
	{
		if(pHeader->uLength >= sizeof(NetPacketHeader) + sizeof(PNetStreamInfo))
		{
			PCNetStreamInfo pStreamInfo = reinterpret_cast<PCNetStreamInfo>(pHeader + 1);
			unsigned uSize = pHeader->uLength - sizeof(NetPacketHeader) - sizeof(NetStreamDataInfo);

			const BITMAPINFO *pbiInput = reinterpret_cast<const BITMAPINFO *>(pStreamInfo + 1);

			BITMAPINFOHEADER bihOutput = { 0 };

			bihOutput.biSize = sizeof(BITMAPINFOHEADER);
			bihOutput.biBitCount = 24;
			bihOutput.biPlanes = 1;
			bihOutput.biCompression = BI_RGB;
			bihOutput.biWidth  = pbiInput->bmiHeader.biWidth;
			bihOutput.biHeight = pbiInput->bmiHeader.biHeight;

			CStream* pStream = m_pManager->StreamByUID(*const_cast<UUID*>(&pStreamInfo->uStreamUID));

			if(pStream)
			{
				if(!pStream->IsValid())
				{
					pStream->InitDecompressor(
						pStreamInfo->uFOURCC,
						reinterpret_cast<LPBITMAPINFO>(&bihOutput),
						const_cast<LPBITMAPINFO>(pbiInput));

				#ifdef _DEBUG
					pStream->m_pSocket = pSocket;
				#endif
				}

				pStream->Release();
			}
			else
			{
				DEBUG_ONLY(Elvees::Output(Elvees::TTrace, TEXT("CStreamClient - Unknown stream info packet.")));
			}
		}
	}
	else
	{
		DEBUG_ONLY(Elvees::Output(Elvees::TTrace, TEXT("CStreamClient - Unknown packet type")));
	}
}

///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////

bool CStreamClient::StartStream(unsigned long address, unsigned short port)
{
	ClientSocket* pCntSocket = NULL;
	Elvees::CSocket::InternetAddress addr(ntohl(address), port);

	bool bStarted = false;

	try
	{
		pCntSocket = (ClientSocket*)Connect(addr);

		if(pCntSocket)
		{
			pCntSocket->m_address = address;
			m_pManager->ClientStreamStarted(address);

			pCntSocket->Read();
		#ifdef _DEBUG
			pCntSocket->Write("printsockstatus\r\n", 17);
		#endif
			bStarted = true;
		}
	}
	catch(const Elvees::CException& e)
	{
		Elvees::OutputF(Elvees::TError, TEXT("Connection failed: %s - %s"),
			e.GetWhere(), e.GetMessage());
	}
	catch(...)
	{
		Elvees::Output(Elvees::TError, TEXT("Connection failed: Unexpected exception"));
	}

	return bStarted;
}

///////////////////////////////////////////////////////////////////////////////
// CStreamClient::ClientSocket
///////////////////////////////////////////////////////////////////////////////

CStreamClient::ClientSocket::ClientSocket(
	CStreamClient& server, SOCKET socket,
	Elvees::CCriticalSection &criticalSection,
	Elvees::CIOBuffer::Allocator &allocator)
	: Elvees::CSocketServer::Socket(server, socket)
	, m_allocator(allocator)
	, m_bufferSequence(criticalSection)
{
	m_pTransferBuffer = NULL;
	m_address = 0;
}

CStreamClient::ClientSocket::~ClientSocket()
{
	if(m_pTransferBuffer)
		m_pTransferBuffer->Release();
}

void CStreamClient::ClientSocket::ResetClientSocket()
{
	if(m_pTransferBuffer)
		m_pTransferBuffer->Release();

	m_pTransferBuffer = NULL;
	m_address = 0;

	m_bufferSequence.Reset();
}

Elvees::CIOBuffer* CStreamClient::ClientSocket::GetTransferBuffer()
{
	if(!m_pTransferBuffer)
		m_pTransferBuffer = m_allocator.Allocate();

	return m_pTransferBuffer;
}

void CStreamClient::ClientSocket::RemoveTransferBuffer()
{
	m_pTransferBuffer = NULL;
}

Elvees::CIOBuffer* CStreamClient::ClientSocket::GetNextBuffer(
	Elvees::CIOBuffer* pBuffer)
{
	return m_bufferSequence.GetNext(pBuffer);
}

Elvees::CIOBuffer* CStreamClient::ClientSocket::ProcessAndGetNext()
{
	return m_bufferSequence.ProcessAndGetNext();
}