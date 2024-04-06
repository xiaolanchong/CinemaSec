// ArchiveClient.cpp: implementation of the CArchiveClient class.
//
//////////////////////////////////////////////////////////////////////

#include "chcs.h"
#include "ArchiveClient.h"

//#include "VidDecoder.h"
#include "NetProtocol.h"
#include "StreamArchive.h"

///////////////////////////////////////////////////////////////////////////////
// CArchiveClient
///////////////////////////////////////////////////////////////////////////////

CArchiveClient::CArchiveClient(
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
// CSocketServer notifications

void CArchiveClient::OnConnectionEstablished(
	Elvees::CSocketServer::Socket *pSocket,
	Elvees::CIOBuffer *pAddress)
{
	// No input connection allowed
	pSocket->Shutdown();
}

bool CArchiveClient::OnConnectionClosing(
	Elvees::CSocketServer::Socket *pSocket)
{
	// Socket final release called
	pSocket->Close();

	return true;
}

void CArchiveClient::OnConnectionClosed(
	Elvees::CSocketServer::Socket *pSocket)
{
	// Release per connection data
	//

	CStreamData *pData = GetStreamData(pSocket);
	
	if(pData)
	{
		delete pData;
		SetStreamData(pSocket, NULL);
	}

	Elvees::Output(Elvees::TInfo, TEXT("Archive session closed"));
}

///////////////////////////////////////////////////////////////////////////////
// Handle read buffers

void CArchiveClient::ReadCompleted(
	Elvees::CSocketServer::Socket *pSocket,
	Elvees::CIOBuffer *pBuffer)
{
	try
	{
		CStreamData *pData = GetStreamData(pSocket);

		pBuffer->AddRef();

		// Ensure in order buffer reads
		pBuffer = pData->GetNextBuffer(pBuffer);

		while(pBuffer)
		{
			ProcessDataStream(pSocket, pBuffer);

			pBuffer->Release();
   			pBuffer = 0;

			pBuffer = pData->ProcessAndGetNext();
		}

		pSocket->Read();
	}
	catch(const Elvees::CException& e)
	{
		Elvees::OutputF(Elvees::TError, TEXT("CStreamClient::ReadCompleted %s %s"),
			e.GetWhere(), e.GetMessage());
		pSocket->Shutdown();
	}
	catch(...)
	{
		Elvees::OutputF(Elvees::TError, TEXT("CStreamClient::ReadCompleted unexpected exception"));
		pSocket->Shutdown();
	}
}

void CArchiveClient::ProcessDataStream(
	Elvees::CSocketServer::Socket *pSocket,
	Elvees::CIOBuffer *pBufferIn)
{
	CStreamData *pData = GetStreamData(pSocket);
	
	// Create and initialize if needed..
	Elvees::CIOBuffer *pBuffer = pData->GetTransferBuffer();
	
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

			pData->RemoveTransferBuffer();

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

unsigned CArchiveClient::GetMessageSize(
   const Elvees::CIOBuffer *pBuffer) const
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
			
			// Message to big lets drop it...
			break;
		}
	}

	// synchro not found drop data
	// last 3 can be synchro part
	return used - 3;
}

void CArchiveClient::ProcessCommand(
	Elvees::CSocketServer::Socket *pSocket,
	Elvees::CIOBuffer *pBuffer)
{
	const BYTE *pData = pBuffer->GetBuffer();
	const unsigned used = pBuffer->GetUsed();

	CStreamData *pStreamData = GetStreamData(pSocket);
	PCNetPacketHeader pHeader = reinterpret_cast<PCNetPacketHeader>(pData);

	// Filter spam
	//

	if(used < sizeof(NetPacketHeader))
		return;

	if(pHeader->uSynchro != CHCS_NET_SYNCHRO)
		return;

	if(pHeader->uLength != used)
		return;

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
			else
			{
				LPCVOID pCompressed = reinterpret_cast<LPCVOID>(pDataInfo + 1);

				pStreamData->m_pStream->DecompressData(
					pCompressed,
					pDataInfo->uFrameSize,
					pDataInfo->uFrameSeq,
					pHeader->uTime);
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

			pStreamData->m_pStream->InitDecompressor(
				pStreamInfo->uFOURCC,
				reinterpret_cast<LPBITMAPINFO>(&bihOutput),
				const_cast<LPBITMAPINFO>(pbiInput));
		}
	}
	else
	{
		Elvees::Output(Elvees::TWarning, TEXT("CStreamClient - Unknown packet type"));
	}
}

///////////////////////////////////////////////////////////////////////////////
// CArchiveClient static functions
///////////////////////////////////////////////////////////////////////////////

void CArchiveClient::SetStreamData(
	Elvees::CSocketServer::Socket *pSocket, 
	CArchiveClient::CStreamData *pData)
{
	SetServerDataPtr(pSocket, reinterpret_cast<void*>(pData));
}
      
CArchiveClient::CStreamData* CArchiveClient::GetStreamData(
	const Elvees::CSocketServer::Socket *pSocket)
{
	return reinterpret_cast<CArchiveClient::CStreamData*>(GetServerDataPtr(pSocket));
}

bool CArchiveClient::StartStream(CStreamArchive* pStream, unsigned long address, unsigned short port)
{
	Elvees::CSocketServer::Socket* pClntSocket = NULL;
	Elvees::CSocket::InternetAddress addr(ntohl(address), port);

	bool bStarted = false;

	try
	{
		pClntSocket = Connect(addr);

		if(pClntSocket)
		{
			CStreamData* pData = new CStreamData(pStream, m_criticalSection, m_allocator);
			
			if(pData)
			{
				SetStreamData(pClntSocket, pData);

				pStream->SetConnection(pClntSocket);

				pClntSocket->Read();
				bStarted = true;
			}
			else
			{
				Elvees::Output(Elvees::TError, TEXT("Fail to allocate StreamData"));
				pClntSocket->Shutdown();
			}

			pClntSocket->Release();
		}
	}
	catch(const Elvees::CException& e)
	{
		Elvees::OutputF(Elvees::TError, TEXT("Connection failed: %s - %s"),
			e.GetWhere(), e.GetMessage());
	}
	catch(...)
	{
		Elvees::OutputF(Elvees::TError, TEXT("Connection failed: Unexpected exception"));
	}

	return bStarted;
}

///////////////////////////////////////////////////////////////////////////////
// CStreamClient::CStreamData
///////////////////////////////////////////////////////////////////////////////

CArchiveClient::CStreamData::CStreamData(
	CStreamArchive* pStream,
	Elvees::CCriticalSection &criticalSection,
	Elvees::CIOBuffer::Allocator &allocator)
	: m_pTransferBuffer(0)
	, m_allocator(allocator)
	, m_bufferSequence(criticalSection)
	, m_pStream(pStream)
{
}

CArchiveClient::CStreamData::~CStreamData()
{
	try
	{
		if(m_pTransferBuffer)
			m_pTransferBuffer->Release();
	}
	catch(...)
	{
	}
}

Elvees::CIOBuffer* CArchiveClient::CStreamData::GetTransferBuffer()
{
	if(!m_pTransferBuffer)
		m_pTransferBuffer = m_allocator.Allocate();

	return m_pTransferBuffer;
}

void CArchiveClient::CStreamData::RemoveTransferBuffer()
{
	m_pTransferBuffer = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// BufferList operations
///////////////////////////////////////////////////////////////////////////////

Elvees::CIOBuffer* CArchiveClient::CStreamData::GetNextBuffer(
	Elvees::CIOBuffer* pBuffer)
{
	return m_bufferSequence.GetNext(pBuffer);
}

Elvees::CIOBuffer* CArchiveClient::CStreamData::ProcessAndGetNext()
{
	return m_bufferSequence.ProcessAndGetNext();
}