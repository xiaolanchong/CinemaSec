//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Transfer a file through a client socket
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 26.07.2005
//                                                                                      //
//======================================================================================//
#ifndef _FILE_TRANSFER_SERVER_7132727046346769_
#define _FILE_TRANSFER_SERVER_7132727046346769_

#include "../../CSChair/debug_int.h"
#include "../IPC/NetAddr.h"
#include "../IPC/NetTcpLink.h"
#include "ThreadError.h"
#include "FileArchiveProvider.h"
//======================================================================================//
//                               class FileTransferServer                               //
//======================================================================================//

struct IFileTransferState;

class FileTransferServer
{
	friend class FileTransferStateSend;
	friend class FileTransferStatePrepare;
	friend class FileTransferStateFinish;

	MyDebugOutputImpl		m_Debug;
	ThreadSync				m_Thread;
	netadr_t				m_NetAddr;
	INT64					m_nBeginTime, m_nEndTime;

	unsigned int			FileTransferProc();

	// for states
	
	LONG			m_nSentCameras;
	TCP::NetClient	m_ClientSocket;

	std::vector<int>	m_CameraIDs;
	typedef	std::vector< boost::shared_ptr<IFileTransferState> >	StateArr_t;
	void					CancellAll( StateArr_t& States );
	bool					ProcessAll( StateArr_t& States );

	std::map<int, boost::shared_ptr<IArchiveProvider> >		m_FileMap;

	static LPCWSTR	ErrorDesc(int nResCode);
public:
	enum
	{
		Res_Ok = 0,
		Res_Exception,
		Res_NotWholeDataHaveBeenSent
	};

	FileTransferServer();
	virtual ~FileTransferServer();

	HRESULT		Start(	IDebugOutput* pDbgInt, netadr_t NetAddr, 
						const std::vector<int>& CameraIDs, 
						INT64 nBeginTime, INT64 nEndTime );
	bool		IsWorking();
	HRESULT		Stop();
	void		SendServerBusy(netadr_t NetAddr);
	void		SendWrongTime(netadr_t NetAddr);
	void		SendWrongCamera(netadr_t NetAddr);
private:
	template <class T> void SendReply( netadr_t NetAddr, LPCWSTR szReplyDesc )
	{
	try
	{
		T Packet;
		TCP::NetSendClient cl( NetAddr, &Packet.m_Message[0], Packet.m_Message.size() );
	}
	catch(NetException &ex)
	{
		m_Debug.PrintW( IDebugOutput::mt_error, L"NetException while trying to send \"%ws\", err=%hs", szReplyDesc, ex.what() );
	}
	}
};

#endif // _FILE_TRANSFER_SERVER_7132727046346769_