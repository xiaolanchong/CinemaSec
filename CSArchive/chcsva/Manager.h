// Manager.h: interface for the CManager class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "Stream.h"
#include "StreamServerEvents.h"

#include "list"

class CVidEncoder;
class CVidDecoder;
class CStreamServer;
class CStreamClient;
class CArchiveServer;
class CArchiveClient;

#define WM_DEVICE_EVENT			WM_APP+1
#define WM_TRAYICON				WM_USER+1
#define WM_UPDATE_STREAM_LIST	WM_USER+2

#define DEVICEEVENTWND_CLASS  TEXT("DeviceEventClass")

//////////////////////////////////////////////////////////////////////
// CManager
//////////////////////////////////////////////////////////////////////

class CManager
	: public CStreamServerEvents
	, public Elvees::CThread
	, private Elvees::CUsesWinsock
{
public:
	explicit CManager(DWORD dwMode);
	virtual ~CManager();

	using Elvees::CThread::Start;

	bool IsStarted();
	void WaitStarted();

	void ShutDown();

	CStream* StreamByID(long lStreamID);
	CStream* StreamByUID(UUID& StreamUID);

	bool GetStreams(CAUUID* pUIDs);

	// 
	//

	bool IsClientStreamStarted(ULONG uConn);
	void ClientStreamStarted(ULONG uConn);
	void ClientStreamStopped(ULONG uConn);

	void StartClientStream(LPCTSTR lpAddress);

	//
	//

	bool CreateArchiveStream(CHCS::IStream** ppStream, long lSourceID, INT64 startPos);

	// Archive settings
	//

	bool IsArchiveEnabled();

	long    GetVideoRate();
	long    GetVideoMaxSize();
	LPCTSTR GetVideoBasePath();

	CVidEncoder* CreateCompressor(LPBITMAPINFO lbiInput);

	// CStreamServerEvents
	virtual long OnConnectionEstablished(LPCTSTR stAddress);
	virtual void OnConnectionClosed(long lSession);

protected:
	bool LoadSettings();
	void SaveSettings();

	bool InitRegisteredDevices(HWND hEvent, bool bDevice);
	void FindAndInstallDevices();

	// Check basepath folder (create if needed)
	bool PrepareArchive(bool bUseDefaultOnFailed = false);

	// Read Stream ID from database
	bool ReadStreamsFromDB(bool bShowDialogOnFailed = false);
	
	//
	void PrepareStreams(bool bSetIDManually);

	// Start proper server
	bool StartStreamServer(unsigned short port = 5005, unsigned long address = INADDR_ANY);
	void StopStreamServer();

	bool StartStreamClients();
	void StopStreamClients();

	bool StartArchiveServer(unsigned short port = 5006, unsigned long address = INADDR_ANY);
	void StopArchiveServer();

	bool StartArchiveClients();
	void StopArchiveClients();
	
	CStream* StartRemoteStream(LPCTSTR lpAddress, UUID& streamUID);

public:
	bool SendFrame(Elvees::CIOBuffer *pData);

private:
	virtual int Run();

	bool RegisterDevice(UUID& streamID, LPCTSTR stMoniker);

private:
	DWORD m_dwMode;

	// Settings
	//

	TCHAR m_stBasePath[MAX_PATH];	// Archive base path
	TCHAR m_stDBInitString[4096];	// DB Initialization string

	// Archive settings

	DWORD m_CodecFCC;				// Codec FOURCC

	long m_lVideoFPM;				// Video Frame per minute
	long m_lVideoMaxSize;			// Video max size in seconds

	long m_lCodecQuality;			//
	long m_lCodecKeyRate;			// Forced KeyRate
	long m_lCodecDataRate;			// Forced DataRate

	long  m_lCodecData;				// Codec state size
	void *m_pCodecData;				// Codec state

	// Network settings

	long m_sessionNumber;
	long m_activeSessions;
	const unsigned m_maxSessions;

	CStreamServer  *m_pStreamServer;		//
	CStreamClient  *m_pStreamClient;		// Client connection manager
	CArchiveServer *m_pArchiveServer;		//
	CArchiveClient *m_pArchiveClient;		//

	// Internal data
	//

	CStreamList m_listStreams;
	Elvees::CCriticalSection m_listSection;

	std::list<ULONG> m_listConnections;
	Elvees::CCriticalSection m_connSection;

	static unsigned int __stdcall ThreadConnect(void *pV);

	HANDLE m_hDeviceOwner;

	//
	//
	
	bool  m_bUpdateSettings;		// Update flag

	Elvees::CManualResetEvent m_startedEvent;
	Elvees::CManualResetEvent m_shutdownEvent;

	static BOOL CALLBACK ManagerWndProc(HWND, UINT, WPARAM, LPARAM);

	HWND CreateDeviceEventWnd();
	static LRESULT CALLBACK DeviceEventWndProc(HWND, UINT, WPARAM, LPARAM);
	
private:
	Elvees::CLibrary xvidcore;
	Elvees::CLibrary xvidvfw;

	// No copies do not implement
	CManager(const CManager &rhs);
	CManager &operator=(const CManager &rhs);
};
