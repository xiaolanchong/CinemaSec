// Manager.cpp: implementation of the CManager class.
//
//////////////////////////////////////////////////////////////////////

#include "chcs.h"
#include "Manager.h"

#define NO_DSHOW_STRSAFE
#include <dshow.h>

#include "DBAccessors.h"
#include "resource.h"

#include "StreamDevice.h"
#include "StreamArchive.h"
#include "StreamMMF.h"

#include "VidEncoder.h"
#include "StreamServer.h"
#include "StreamClient.h"
#include "ArchiveServer.h"
#include "ArchiveClient.h"

#include "DlgShowStream.h"

// Windows sends this message when the taskbar is created. This can 
// happen if it crashes and Windows has to restart it.
const UINT WM_TASKBARCREATED = ::RegisterWindowMessage(TEXT("TaskbarCreated"));

#define MAX_NET_CLIENTS		100

CManager::CManager(DWORD dwMode)
	: m_dwMode(dwMode)
	, m_shutdownEvent(false)
	, m_startedEvent(false)
	, m_maxSessions(MAX_NET_CLIENTS)
	, xvidcore(TEXT("xvidcore.dll"))
	, xvidvfw(TEXT("xvidvfw.dll"))

{
	// Default parameters

	m_hDeviceOwner = NULL;

	m_lVideoFPM     = 300L;
	m_lVideoMaxSize = 300L;

	m_lCodecQuality  = 10000L;
	m_lCodecKeyRate  = 0L;
	m_lCodecDataRate = 0L;

	m_CodecFCC   = 0;
	m_lCodecData = 0;
	m_pCodecData = NULL;

	lstrcpy(m_stBasePath, TEXT(""));
	lstrcpy(m_stDBInitString, TEXT(""));

	m_sessionNumber  = 0;
	m_activeSessions = 0;
	m_bUpdateSettings = false;
	m_pStreamServer = NULL;
	m_pStreamClient = NULL;
	m_pArchiveServer = NULL;
	m_pArchiveClient = NULL;
}

CManager::~CManager()
{
	if(m_hDeviceOwner)
		::CloseHandle(m_hDeviceOwner);
}

//////////////////////////////////////////////////////////////////////////
//

bool CManager::LoadSettings()
{
	LONG  lRes;
	DWORD dwValue;
	
	CRegKey keyCinema;
	TCHAR stKey[MAX_PATH];

	// Load from registry

	if(::LoadString(_Module.GetResourceInstance(), 
		IDS_CHCS_REGISTRY_KEY, stKey, countof(stKey)) == 0)
	{
		Elvees::Output(Elvees::TError, TEXT("Fail load default registry key"));
		return false;
	}
	
	lRes = keyCinema.Open(HKEY_LOCAL_MACHINE, stKey);
	if(lRes != ERROR_SUCCESS)
	{
		Elvees::Output(Elvees::TError, TEXT("Fail open default registry key"));
		return false;
	}

	dwValue = countof(m_stBasePath);
	lRes = keyCinema.QueryStringValue(TEXT("ArchivePath"), m_stBasePath, &dwValue);
	if(lRes != ERROR_SUCCESS)
		lstrcpy(m_stBasePath, TEXT(""));

	dwValue = countof(m_stDBInitString);
	lRes = keyCinema.QueryStringValue(TEXT("DBConnectionString"), m_stDBInitString, &dwValue);
	if(lRes != ERROR_SUCCESS)
		lstrcpy(m_stBasePath, TEXT(""));

	DEBUG_ONLY(Elvees::OutputF(Elvees::TTrace,
		TEXT("LoadSettings BasePath=\"%s\""), m_stBasePath));

	// Read archive settings
	//

	do
	{	
		CRegKey keyStore;

		lRes = keyStore.Open(keyCinema, TEXT("Storage"));
		if(lRes != ERROR_SUCCESS)
		{
			Elvees::Output(Elvees::TError, TEXT("Fail open archive setting registry key"));
			break;
		}

		lRes = keyStore.QueryDWORDValue(TEXT("Codec_FCC"), dwValue);
		if(lRes == ERROR_SUCCESS)
			m_CodecFCC = dwValue;

		lRes = keyStore.QueryDWORDValue(TEXT("Codec_Quality"), dwValue);
		if(lRes == ERROR_SUCCESS)
			m_lCodecQuality = dwValue;

		lRes = keyStore.QueryDWORDValue(TEXT("Codec_KeyRate"), dwValue);
		if(lRes == ERROR_SUCCESS)
			m_lCodecKeyRate = dwValue;

		lRes = keyStore.QueryDWORDValue(TEXT("Codec_DataRate"), dwValue);
		if(lRes == ERROR_SUCCESS)
			m_lCodecDataRate = dwValue;

		lRes = keyStore.QueryDWORDValue(TEXT("Video_FMP"), dwValue);
		if(lRes == ERROR_SUCCESS)
			m_lVideoFPM = dwValue;

		lRes = keyStore.QueryDWORDValue(TEXT("Video_MaxSize"), dwValue);
		if(lRes == ERROR_SUCCESS)
			m_lVideoMaxSize = dwValue;

		// Load codec state

		if(m_pCodecData)
			free(m_pCodecData);
		
		m_lCodecData = 0;
		m_pCodecData = NULL;

		dwValue = 0;

		lRes = keyStore.QueryBinaryValue(TEXT("Codec_State"), NULL, &dwValue);
		if(lRes != ERROR_SUCCESS)
		{
			Elvees::Output(Elvees::TError, TEXT("Fail to load codec state"));
			break;
		}
		
		if(dwValue > 0)
		{
			m_pCodecData = malloc(dwValue);
			if(!m_pCodecData)
			{
				Elvees::Output(Elvees::TError, TEXT("Memory operation failed"));
				break;
			}

			m_lCodecData = dwValue;

			lRes = keyStore.QueryBinaryValue(TEXT("Codec_State"), m_pCodecData, &dwValue);
			if(lRes != ERROR_SUCCESS)
			{
				Elvees::Output(Elvees::TError, TEXT("Fail to load codec state"));

				free(m_pCodecData);

				m_lCodecData = 0;
				m_pCodecData = NULL;
			}
		}
	}
	while(false);

#ifdef _DEBUG
	BYTE bFCC[4];
	bFCC[0] = (BYTE)((m_CodecFCC & 0x000000FF));
	bFCC[1] = (BYTE)((m_CodecFCC & 0x0000FF00) >> 0x08);
	bFCC[2] = (BYTE)((m_CodecFCC & 0x00FF0000) >> 0x10);
	bFCC[3] = (BYTE)((m_CodecFCC & 0xFF000000) >> 0x18);

	if(!isprint(bFCC[0])) bFCC[0] = '?';
	if(!isprint(bFCC[1])) bFCC[1] = '?';
	if(!isprint(bFCC[2])) bFCC[2] = '?';
	if(!isprint(bFCC[3])) bFCC[3] = '?';

	Elvees::OutputF(Elvees::TTrace, TEXT("Store: FMP=%ld MS=%ld FCC=0x%08lX (%c%c%c%c) Q=%ld KR=%ld DR=%ld SS=%ld"),
		m_lVideoFPM,
		m_lVideoMaxSize,
		m_CodecFCC, bFCC[0], bFCC[1], bFCC[2], bFCC[3],
		m_lCodecQuality,
		m_lCodecKeyRate,
		m_lCodecDataRate,
		m_lCodecData);
#endif

	return true;
}

void CManager::SaveSettings()
{
	if(!m_bUpdateSettings)
		return;

	LONG  lRes;
	TCHAR stKey[MAX_PATH];
	CRegKey keyCinema;	

	if(::LoadString(_Module.GetResourceInstance(), IDS_CHCS_REGISTRY_KEY, stKey, countof(stKey)) == 0)
	{
		Elvees::Output(Elvees::TError, TEXT("Fail load default registry key"));
		return;
	}

	lRes = keyCinema.Open(HKEY_LOCAL_MACHINE, stKey);
	if(lRes != ERROR_SUCCESS)
	{
		Elvees::Output(Elvees::TError, TEXT("Fail open default registry key"));
		return;
	}

	lRes = keyCinema.SetStringValue(TEXT("ArchivePath"), m_stBasePath);
	if(lRes != ERROR_SUCCESS)
		Elvees::OutputF(Elvees::TWarning, TEXT("Fail to update ArchivePath parameter lRes=%ld"), lRes);

	lRes = keyCinema.SetStringValue(TEXT("DBConnectionString"), m_stDBInitString);
	if(lRes != ERROR_SUCCESS)
		Elvees::OutputF(Elvees::TWarning, TEXT("Fail to update DBConnectionString parameter lRes=%ld"), lRes);
}

bool CManager::PrepareArchive(bool bUseDefaultOnFailed /*= false*/)
{
	::SetLastError(0);

	if(!::CreateDirectory(m_stBasePath, NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
	{
		if(!bUseDefaultOnFailed)
		{
			Elvees::Output(Elvees::TError, TEXT("Fail to create archive base directory"));
			return false;
		}

		//  Create directory in dll directory
		::GetModuleFileName(_Module.GetModuleInstance(), m_stBasePath, countof(m_stBasePath));

		for(int i=lstrlen(m_stBasePath); m_stBasePath[i] != '\\'; i--);
		
		m_stBasePath[i+1] = 0;
		lstrcat(m_stBasePath, TEXT("Archive"));

		::SetLastError(0);

		if(!::CreateDirectory(m_stBasePath, NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
		{
			Elvees::Output(Elvees::TError, TEXT("Fail to create archive directory"));
			return false;
		}

		m_bUpdateSettings = true;
	}

	return true;
}

bool CManager::InitRegisteredDevices(HWND hEventWnd, bool bDevice)
{
	Elvees::Output(Elvees::TInfo, TEXT("> Create registered streams"));

	LONG  lRes;
	DWORD dwIndex, dwValue;
	TCHAR stKeyName[MAX_PATH];

	// Stream settings
	UUID streamID;
	bool bBWMode;
	long lWidth, lHeight, lFrameRate, lInput, lVStandart;
	TCHAR stMoniker[MAX_PATH];

	CStream *pStream;
	CRegKey keyDevices;

	long lStreamID = 0;
	
	if(::LoadString(_Module.GetResourceInstance(), IDS_CHCS_REGISTRY_KEY, stKeyName, countof(stKeyName)) == 0)
	{
		Elvees::Output(Elvees::TError, TEXT("Fail load default registry key"));
		return false;
	}
	
	lstrcat(stKeyName, TEXT("\\Devices"));

	lRes = keyDevices.Open(HKEY_LOCAL_MACHINE, stKeyName);
	if(lRes != ERROR_SUCCESS)
	{
		Elvees::Output(Elvees::TError, TEXT("Fail open default registry key"));
		return false;
	}

	dwIndex = 0;
	
	while(RegEnumKey(keyDevices, dwIndex++, stKeyName, countof(stKeyName)) ==  ERROR_SUCCESS)
	{
		// Set stream ID in registry order
		lStreamID--;

		// Get camera UIDs
		//

		streamID = GUID_NULL;
		lstrcpy(stMoniker, TEXT(""));

		// Default settings
		//

		lWidth  = 0;
		lHeight = 0;
		lInput  = 0;
		lFrameRate = 0;
		lVStandart = 0;
		bBWMode = false;
		
		if(!StringToUuid(stKeyName, &streamID))
		{
			Elvees::OutputF(Elvees::TWarning, TEXT("Invalid stream UID = \"%s\""), stKeyName);
			continue;
		}

		// Read device parameters
		//

		do
		{
			CRegKey keyDevice;

			lRes = keyDevice.Open(keyDevices, stKeyName);
			if(lRes != ERROR_SUCCESS)
			{
				Elvees::OutputF(Elvees::TError, TEXT("Fail to open registry key \"%s\""), stKeyName);
				break;
			}

			dwValue = countof(stMoniker);

			lRes = keyDevice.QueryStringValue(TEXT("Device_DisplayName"), stMoniker, &dwValue);
			if(lRes != ERROR_SUCCESS)
			{
				Elvees::OutputF(Elvees::TError, TEXT("Fail to read moniker for device \"%s\""), stKeyName);
				break;
			}

			lRes = keyDevice.QueryDWORDValue(TEXT("Device_Width"), dwValue);
			if(lRes != ERROR_SUCCESS)
				Elvees::OutputF(Elvees::TWarning, TEXT("Using default width for device \"%s\""), stKeyName);
			else
				lWidth = dwValue;

			lRes = keyDevice.QueryDWORDValue(TEXT("Device_Height"), dwValue);
			if(lRes != ERROR_SUCCESS)
				Elvees::OutputF(Elvees::TWarning, TEXT("Using default height for device \"%s\""), stKeyName);
			else
				lHeight = dwValue;

			lRes = keyDevice.QueryDWORDValue(TEXT("Device_Input"), dwValue);
			if(lRes != ERROR_SUCCESS)
				Elvees::OutputF(Elvees::TWarning, TEXT("Using default input for device \"%s\""), stKeyName);
			else
				lInput = dwValue;

			lRes = keyDevice.QueryDWORDValue(TEXT("Device_BWMode"), dwValue);
			if(lRes != ERROR_SUCCESS)
				Elvees::OutputF(Elvees::TWarning, TEXT("Using default BWMode for device \"%s\""), stKeyName);
			else
				bBWMode = (dwValue == 0) ? false : true;

			lRes = keyDevice.QueryDWORDValue(TEXT("Device_Standard"), dwValue);
			if(lRes != ERROR_SUCCESS)
				Elvees::OutputF(Elvees::TWarning, TEXT("Using default VStandard for device \"%s\""), stKeyName);
			else
				lVStandart = dwValue;

			lRes = keyDevice.QueryDWORDValue(TEXT("Device_FrameRate"), dwValue);
			if(lRes != ERROR_SUCCESS)
				Elvees::OutputF(Elvees::TWarning, TEXT("Using default FrameRate for device \"%s\""), stKeyName);
			else
				lFrameRate = dwValue;


			// Create and init stream
			//

			Elvees::OutputF(Elvees::TInfo, TEXT("Create stream {%ld} \"%s\" "), -lStreamID, stKeyName);

			if(bDevice)
			{
				pStream = CStreamDevice::CreateStream(
					streamID, stMoniker,
					lWidth,	lHeight, lFrameRate,
					lInput, lVStandart,
					bBWMode);

				if(hEventWnd)
					((CDevice*)(CStreamDevice*)pStream)->SetEventHWND(hEventWnd, WM_DEVICE_EVENT);
			}
			else
			{
				pStream = CStreamMMF::CreateStream(streamID, stMoniker);
			}

			if(pStream)
			{
				pStream->SetID(lStreamID);
				m_listStreams.PushLastNode(pStream);
			}
			else
			{
				Elvees::Output(Elvees::TError, TEXT("Fail to create stream"));
			}
		}
		while(false);
	}

	return true;
}

void CManager::FindAndInstallDevices()
{
	Elvees::Output(Elvees::TInfo, TEXT("> Searching for new devices"));

	HRESULT hr;
	ULONG cFetched;

	IMalloc *pMalloc = NULL;
	ICreateDevEnum *pDevEnum = NULL;
	IEnumMoniker *pClassEnum = NULL;

	IMoniker *pMoniker = NULL;
	IPropertyBag *pBag = NULL;

	TCHAR stMoniker[MAX_PATH];
	TCHAR stFriendlyName[MAX_PATH];

	BSTR    szDisplayName;
	VARIANT varFriendlyName;

	do
	{
		hr = CoGetMalloc(1, &pMalloc);
		if(FAILED(hr))
		{
			Elvees::Output(Elvees::TError, TEXT("Fail to get alloc interface"));
			break;
		}

		// Create the system device enumerator
		hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC,
			IID_ICreateDevEnum, (void **)&pDevEnum);
		if(FAILED(hr))
		{
			Elvees::Output(Elvees::TError, TEXT("Fail to create system device enumerator"));
			break;
		}

		// Create an enumerator for the video capture devices
		hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pClassEnum, 0);
		if(FAILED(hr))
		{
			Elvees::Output(Elvees::TError, TEXT("Fail to create an enumerator for the video input devices"));
			break;
		}

		// If there are no enumerators for the requested type, then 
		// CreateClassEnumerator will succeed, but pClassEnum will be NULL.
		if(pClassEnum == NULL)
		{
			Elvees::Output(Elvees::TError, TEXT("There are no enumerator for the video input devices"));
			break;
		}

		while(S_OK == pClassEnum->Next(1, &pMoniker, &cFetched))
		{
			if(!pMoniker)
				continue;

			hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void**)&pBag);
			if(SUCCEEDED(hr) && pBag)
			{
				// Device display name
				//

				szDisplayName = NULL;
				hr = pMoniker->GetDisplayName(NULL, NULL, &szDisplayName);

				// Dont work with VfW devices
				if(SUCCEEDED(hr) && wcsncmp(szDisplayName, L"@device:cm:", 11) != 0)
				{
				#ifdef _UNICODE
					lstrcpy(stMoniker, szDisplayName);
				#else
					WideCharToMultiByte(CP_ACP, 0, szDisplayName, (int)wcslen(szDisplayName) + 1,
						stMoniker, sizeof(stMoniker), NULL, NULL);
				#endif
					pMalloc->Free(szDisplayName);

					// Device friendly name
					//

					VariantInit(&varFriendlyName);
					hr = pBag->Read(L"FriendlyName", &varFriendlyName, NULL);

					if(SUCCEEDED(hr))
					{
					#ifdef _UNICODE
						lstrcpy(stFriendlyName, varFriendlyName.bstrVal);
					#else
						WideCharToMultiByte(CP_ACP, 0,
							varFriendlyName.bstrVal, (int)wcslen(varFriendlyName.bstrVal) + 1,
							stFriendlyName, sizeof(stFriendlyName), NULL, NULL);
					#endif
					}
					else
					{
						lstrcpy(stFriendlyName, TEXT("Unknown"));
					}

					VariantClear(&varFriendlyName); 

					// Update device settings

					bool bNewDevice = true;
					CStream *pStream = m_listStreams.Head();

					while(pStream)
					{
						if(pStream->IsDeviceString(stMoniker))
						{
							bNewDevice = false;
							break;
						}

						pStream = m_listStreams.Next(pStream);
					}

					if(bNewDevice)
					{
						Elvees::OutputF(Elvees::TInfo, TEXT("New device \"%s\" found"), stFriendlyName);

						UUID streamID = GUID_NULL;
						::CoCreateGuid(&streamID);

						CStream *pStream = CStreamDevice::CreateStream(
							streamID, stMoniker);

						if(pStream)
						{
							if(RegisterDevice(streamID, stMoniker))
							{
								m_listStreams.PushLastNode(pStream);
							}
							else
							{
								Elvees::Output(Elvees::TError, TEXT("Fail to register device"));
								pStream->Release();
							}
						}
						else
						{
							Elvees::Output(Elvees::TError, TEXT("Fail to init device"));
						}
					}
				}

				pBag->Release();
			}

			pMoniker->Release();
			pMoniker = NULL;
		}
	}
	while(0);

	if(pMalloc)
		pMalloc->Release();

	if(pDevEnum)
		pDevEnum->Release();

	if(pClassEnum)
		pClassEnum->Release();
}

bool CManager::RegisterDevice(UUID& streamID, LPCTSTR stMoniker)
{
	TCHAR stDevice[64];
	UuidString pszUuid = 0;
	
	if(::UuidToString(&streamID, &pszUuid) == RPC_S_OK && pszUuid)
	{
		wsprintf(stDevice, TEXT("{%s}"), pszUuid);
		RpcStringFree(&pszUuid);
	}
	else
	{
		Elvees::Output(Elvees::TError, TEXT("UUID conversion failed"));
		return false;
	}

	LONG  lRes;
	TCHAR stKeyName[MAX_PATH];

	CRegKey keyDevices;

	if(::LoadString(_Module.GetResourceInstance(), IDS_CHCS_REGISTRY_KEY, stKeyName, countof(stKeyName)) == 0)
	{
		Elvees::Output(Elvees::TError, TEXT("Fail load default registry key"));
		return false;
	}

	lstrcat(stKeyName, TEXT("\\Devices"));

	lRes = keyDevices.Open(HKEY_LOCAL_MACHINE, stKeyName);
	if(lRes != ERROR_SUCCESS)
		lRes = keyDevices.Create(HKEY_LOCAL_MACHINE, stKeyName);

	if(lRes != ERROR_SUCCESS)
	{
		Elvees::Output(Elvees::TError, TEXT("Fail open default registry key"));
		return false;
	}

	do
	{
		CRegKey keyDevice;

		lRes = keyDevice.Create(keyDevices, stDevice);
		if(lRes != ERROR_SUCCESS)
			break;

		keyDevice.SetDWORDValue(TEXT("Device_BWMode"), 1);
		keyDevice.SetDWORDValue(TEXT("Device_Input"), 2);
		keyDevice.SetDWORDValue(TEXT("Device_Width"), 720);
		keyDevice.SetDWORDValue(TEXT("Device_Height"), 576);
		keyDevice.SetDWORDValue(TEXT("Device_Standard"), 16);
		keyDevice.SetDWORDValue(TEXT("Device_FrameRate"), 25);

		lRes = keyDevice.SetStringValue(TEXT("Device_DisplayName"), stMoniker);
		if(lRes != ERROR_SUCCESS)
			break;
		
		return true;
	}
	while(false);

	keyDevices.DeleteSubKey(stDevice);
	return false;
}

bool CManager::ReadStreamsFromDB(bool bShowDialogOnFailed)
{
	Elvees::Output(Elvees::TInfo, TEXT("> Read stream properties from database"));

	//////////////////////////////////////////////////////////////////////////
	//

	PHOSTENT pHost = gethostbyname(NULL);

	if(pHost)
	{
		Elvees::Output(Elvees::TInfo, TEXT("Local address(es):"));

		int nAddr = 0;

		while(pHost->h_addr_list[nAddr])
		{
			Elvees::OutputF(Elvees::TInfo, TEXT("\t%hS"),
				inet_ntoa(*(IN_ADDR*)pHost->h_addr_list[nAddr]));

			nAddr++;
		}
	}

	//////////////////////////////////////////////////////////////////////////

	HRESULT hr;
	CDataSource dbsource;
	CSession    dbsession;
	CCommand<CAccessor<CAccAllCameras> > cmdCameras;
	
	// Open and read database

	bool bDone = false;
	CStream *pStream;

	do
	{	
		// Open database

	#ifdef _UNICODE
		hr = dbsource.OpenFromInitializationString(m_stDBInitString);
	#else
		WCHAR szDBInitString[4096];

		MultiByteToWideChar(CP_ACP, 0, m_stDBInitString, lstrlen(m_stDBInitString) + 1,
			szDBInitString, countof(szDBInitString));

		hr = dbsource.OpenFromInitializationString(szDBInitString);
	#endif
		if(FAILED(hr))
		{
			if(!bShowDialogOnFailed)
			{
				Elvees::OutputF(Elvees::TError, TEXT("Fail to open database connection hr=0x%08lX"), hr);
				break;
			}

			hr = dbsource.Open();
			if(FAILED(hr))
			{
				Elvees::OutputF(Elvees::TError, TEXT("Fail to open database connection hr=0x%08lX"), hr);
				break;
			}

			BSTR bstrDBInit;
			hr = dbsource.GetInitializationString(&bstrDBInit);

			if(FAILED(hr))
			{
				Elvees::OutputF(Elvees::TError, TEXT("GetInitializationString failed hr=0x%08lX"), hr);
				break;
			}

		#ifdef _UNICODE
			lstrcpy(m_stDBInitString, bstrDBInit);
		#else
			WideCharToMultiByte(CP_ACP, 0, bstrDBInit, (int)wcslen(bstrDBInit) + 1,
				m_stDBInitString, sizeof(m_stDBInitString), NULL, NULL);
		#endif
			SysFreeString(bstrDBInit);

			m_bUpdateSettings = true;
		}

		// Open session

		hr = dbsession.Open(dbsource);
		if(FAILED(hr))
		{
			Elvees::OutputF(Elvees::TError, TEXT("Fail to open database session hr=0x%08lX"), hr);
			break;
		}

		// Save all streams to list

		hr = cmdCameras.Open(dbsession);
		if(FAILED(hr))
		{
			Elvees::OutputF(Elvees::TError, TEXT("Fail to open table hr=0x%08lX"), hr);
			break;
		}

		hr = cmdCameras.MoveFirst();

		UUID camUID;
		unsigned long camIP;

		while(hr != DB_S_ENDOFROWSET)
		{
			do
			{
				// Check UID

				if(!StringToUuid(cmdCameras.m_stCameraUID, &camUID))
				{
					Elvees::OutputF(Elvees::TError, TEXT("Invalid stream UID = \"%s\""), cmdCameras.m_stCameraUID);
					break;
				}

				// Check IP

			#ifndef _UNICODE
				camIP = inet_addr(cmdCameras.m_stCameraIP);
			#else
				char stCameraIP[56];

				WideCharToMultiByte(CP_ACP, 0, cmdCameras.m_stCameraIP,
					(int)wcslen(cmdCameras.m_stCameraIP) + 1,
					stCameraIP, sizeof(stCameraIP), NULL, NULL);

				camIP = inet_addr(stCameraIP);
			#endif

				if(INADDR_NONE == camIP)
				{
					Elvees::OutputF(Elvees::TError, TEXT("Invalid stream IP = \"%s\""), cmdCameras.m_stCameraIP);
					break;
				}

				// Store stream info

				bool bLocal = false;
				pStream = m_listStreams.Head();

				while(pStream)
				{
					if(pStream->IsStreamUID(camUID))
					{
						bLocal = true;
						pStream->SetID(cmdCameras.m_lCameraID);
						break;
					}

					pStream = m_listStreams.Next(pStream);
				}

				if(!bLocal && (m_dwMode & CHCS_MODE_NETCLIENT) )
				{
					pStream = CStreamMMF::CreateStream(camUID, cmdCameras.m_stCameraIP, false);

					if(pStream)
					{
						pStream->SetID(cmdCameras.m_lCameraID);
						m_listStreams.PushNode(pStream);
					}
				}

				Elvees::OutputF(Elvees::TInfo, TEXT(" %c%ld %s \"%s\""),
					bLocal ? TEXT('~') : TEXT('*'), cmdCameras.m_lCameraID,
					cmdCameras.m_stCameraIP, cmdCameras.m_stCameraUID);
			}
			while(false);

			hr = cmdCameras.MoveNext();
		}

		bDone = true;
	}
	while(false);

	return bDone;
}

void CManager::PrepareStreams(bool bSetIDManually)
{
	// Delete all invalid streams
	Elvees::Output(Elvees::TInfo, TEXT("> Start valid streams"));

	CStream *pStream = m_listStreams.Head(), *pStreamNext;

	while(pStream)
	{
		if(/*!pStream->IsValid() ||*/ (!bSetIDManually && pStream->GetID() < 0))
		{
			pStreamNext = m_listStreams.Next(pStream);

			try
			{
				pStream->RemoveFromList();
				pStream->Release();
			}
			catch(...)
			{
				DEBUG_ONLY(DebugBreak());
			}

			pStream = pStreamNext;
			continue;
		}

		if(bSetIDManually)
			pStream->SetID(-pStream->GetID());
		
		pStream->Start();

		pStream = m_listStreams.Next(pStream);
	}
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

CStream* CManager::StreamByID(long lStreamID)
{
	Elvees::CCriticalSection::Owner lock(m_listSection);

	CStream *pStream;

	// Is stream local?
	//

	pStream = m_listStreams.Head();

	while(pStream)
	{
		if(pStream->GetID() == lStreamID)
		{
			pStream->AddRef();
			return pStream;
		}

		pStream = m_listStreams.Next(pStream);
	}

	return NULL;
}

CStream* CManager::StreamByUID(UUID& StreamUID)
{
	Elvees::CCriticalSection::Owner lock(m_listSection);

	CStream *pStream;
	
	// Is stream local?

	pStream = m_listStreams.Head();

	while(pStream)
	{
		if(pStream->IsStreamUID(StreamUID))
		{
			pStream->AddRef();
			return pStream;
		}

		pStream = m_listStreams.Next(pStream);
	}

	return NULL;
}

bool CManager::GetStreams(CAUUID* pUIDs)
{
	Elvees::CCriticalSection::Owner lock(m_listSection);

	pUIDs->cElems = m_listStreams.Count();

	if(pUIDs->cElems == 0)
	{
		pUIDs->pElems = NULL;
		return false;
	}

	pUIDs->pElems = (GUID*)CoTaskMemAlloc(sizeof(GUID)*pUIDs->cElems);

	if(pUIDs->pElems == NULL)
		return false;
	
	long lIndex = 0;
	CStream *pStream;
	pStream = m_listStreams.Head();

	while(pStream)
	{
		pUIDs->pElems[lIndex] = pStream->GetUID();
		lIndex++;

		pStream = m_listStreams.Next(pStream);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

int CManager::Run()
{
	Elvees::Output(Elvees::TInfo, TEXT("> Manager started"));

	CoInitialize(NULL);

	// Load settings from registry
	LoadSettings();

	// Check and initialize archive
	if(m_dwMode & CHCS_MODE_STORE)
		PrepareArchive(true);

	// Start stream clients
	if(m_dwMode & CHCS_MODE_NETCLIENT)
	{
		StartStreamClients();
		StartArchiveClients();
	}

	//////////////////////////////////////////////////////////////////////////
	// Create windows

	HWND hServerWnd = NULL;
	HWND hDeviceWnd = NULL;

	InitCommonControls();

	if(m_dwMode & CHCS_MODE_GUI)
	{
		hServerWnd = CreateDialogParam(_Module.GetModuleInstance(),
			MAKEINTRESOURCE(IDD_MANAGER), NULL, ManagerWndProc, (LPARAM)this);

		if(!hServerWnd)
			Elvees::Output(Elvees::TError, TEXT("Fail create manager window"));
	}

	//////////////////////////////////////////////////////////////////////////
	// Start local device or map if it essential

	if(m_dwMode & CHCS_MODE_DEVICE)
	{
		m_hDeviceOwner = ::CreateEvent(NULL, TRUE, FALSE, TEXT("Global\\CHCSDeviceOwner"));

		if(m_hDeviceOwner && GetLastError() != ERROR_ALREADY_EXISTS)
			Elvees::Output(Elvees::TInfo, TEXT("Starting devices"));
		else
			Elvees::Output(Elvees::TWarning, TEXT("Starting devices twice"));

		// Start stream server
		if(m_dwMode & CHCS_MODE_NETSERVER)
		{
			StartStreamServer();
			StartArchiveServer();
		}

		hDeviceWnd = CreateDeviceEventWnd();
		InitRegisteredDevices(hDeviceWnd, true);

		if(m_dwMode & CHCS_MODE_DEVICENEW)
			FindAndInstallDevices();
	}

	if(m_dwMode & CHCS_MODE_DATABASE)
	{
		ReadStreamsFromDB(false);
		PrepareStreams(false);
	}
	else
	{
		PrepareStreams(true);
	}

	SaveSettings();
	SendMessage(hServerWnd, WM_UPDATE_STREAM_LIST, 0, 0);

	m_startedEvent.Set();

	//////////////////////////////////////////////////////////////////////////
	// Main loop
	
	MSG msg;
	DWORD dwRet;
	HANDLE hShutdown = m_shutdownEvent.GetEvent();

	while(1)
	{
		dwRet = MsgWaitForMultipleObjects(1, &hShutdown, FALSE, INFINITE, QS_ALLINPUT);

		if(dwRet == WAIT_OBJECT_0 + 1)
		{
			while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if(msg.message == WM_QUIT)
				{
				//	m_shutdownEvent.Set();
					break;
				}

				if(IsDialogMessage(hServerWnd, &msg) == 0)
				{
					TranslateMessage(&msg); 
					DispatchMessage(&msg);
				}
			}
		}
		else if(dwRet == WAIT_OBJECT_0) // Shutdown
		{
			if(::IsWindow(hServerWnd))
			{
				::ShowWindow(hServerWnd, SW_HIDE);
				::DestroyWindow(hServerWnd);
			}
			break;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	
	CStream* pStream = NULL;

	while(!m_listStreams.Empty())
	{
		pStream = m_listStreams.PopNode();
		pStream->Stop();
		pStream->Release();
	}

	if(hDeviceWnd && IsWindow(hDeviceWnd))
		DestroyWindow(hDeviceWnd);
	
	StopStreamServer();
	StopStreamClients();
	StopArchiveServer();
	StopArchiveClients();

	CoUninitialize();

	Elvees::Output(Elvees::TInfo, TEXT("> Manager stopped"));
	m_startedEvent.Reset();

	return 0;
}

bool CManager::IsStarted()
{
	return m_startedEvent.Wait(0);
}

void CManager::WaitStarted()
{
	HANDLE h[2];

	h[0] = m_startedEvent.GetEvent();
	h[1] = GetHandle(); // CThread;

//	WaitForMultipleObjects(2, h, FALSE, INFINITE);

	if(WaitForMultipleObjects(2, h, FALSE, 60000) == WAIT_TIMEOUT)
		Elvees::Output(Elvees::TWarning, TEXT("Manager starting too long time"));
}

void CManager::ShutDown()
{
	m_shutdownEvent.Set();
	Wait();
}

bool CManager::IsArchiveEnabled()
{
	return (m_dwMode & CHCS_MODE_STORE) ? true : false;
}

CVidEncoder* CManager::CreateCompressor(LPBITMAPINFO lbiInput)
{
	bool bOK = false;
	CVidEncoder* pEncoder = NULL;

	if(m_CodecFCC == 0)
	{
		static bool bShowError = true;

		if(bShowError)
		{
			Elvees::Output(Elvees::TCritical, TEXT("No codec selected!"));
			bShowError = false;
		}

		return NULL;
	}

	do
	{
		pEncoder = new CVidEncoder(m_CodecFCC);
		if(!pEncoder)
			break;

		pEncoder->InitCompressor(lbiInput,
			m_lVideoFPM > 0 ? 60000000/m_lVideoFPM : 40000,
			m_lCodecQuality, m_lCodecKeyRate, m_lCodecDataRate, m_pCodecData, m_lCodecData);

		if(!pEncoder->IsCompressionInited())
			break;

		pEncoder->StartCompression();

		if(!pEncoder->IsCompressionStarted())
			break;

		bOK = true;
	}
	while(false);

	if(!bOK)
	{
		delete pEncoder;
		pEncoder = NULL;
	}

	return pEncoder;
}

long CManager::GetVideoRate()
{
	return m_lVideoFPM;
}

long CManager::GetVideoMaxSize()
{
	return m_lVideoMaxSize;
}

LPCTSTR CManager::GetVideoBasePath()
{
	return m_stBasePath;
}

//////////////////////////////////////////////////////////////////////////
// manager window
//////////////////////////////////////////////////////////////////////////

BOOL CALLBACK CManager::ManagerWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	BOOL bRet = TRUE;
	CManager* pThis = (CManager*)(LONG_PTR)GetWindowLong(hWnd, GWL_USERDATA);

	// Restore taskbar icon if it crashes
	if(message == WM_TASKBARCREATED)
	{
		// Add icon to tray..
		NOTIFYICONDATA IconData;
		IconData.cbSize = sizeof(NOTIFYICONDATA);
		IconData.uID = 1;
		IconData.hWnd = hWnd;
		IconData.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
		IconData.hIcon = LoadIcon(_Module.GetResourceInstance(), MAKEINTRESOURCE(
			(pThis->m_dwMode & CHCS_MODE_NETCLIENT) ? IDI_MAIN_ICON_CLIENT : IDI_MAIN_ICON));
		IconData.uCallbackMessage = WM_TRAYICON;
		lstrcpy(IconData.szTip, TEXT("Manager"));

		::Shell_NotifyIcon(NIM_ADD, &IconData);
		return TRUE;
	}

	switch(message)
	{
	case WM_INITDIALOG:
		{
			SetWindowLong(hWnd, GWL_USERDATA, (LONG)lParam);
			pThis = reinterpret_cast<CManager*>(lParam);
	
			HICON hIcon = LoadIcon(_Module.GetResourceInstance(), MAKEINTRESOURCE(
				(pThis->m_dwMode & CHCS_MODE_NETCLIENT) ? IDI_MAIN_ICON_CLIENT : IDI_MAIN_ICON));

			::SendMessage(hWnd, WM_SETICON, FALSE, (LPARAM)hIcon);
			::SendMessage(hWnd, WM_SETICON, TRUE, (LPARAM)hIcon);

			HWND hWndStreamList = ::GetDlgItem(hWnd, IDC_STREAMS_LIST);
		
			// Ex style

			ListView_SetExtendedListViewStyle(hWndStreamList,
				ListView_GetExtendedListViewStyle(hWndStreamList) | LVS_EX_FULLROWSELECT);

			ListView_SetBkColor(hWndStreamList, GetSysColor(COLOR_MENU));
			ListView_SetTextBkColor(hWndStreamList, GetSysColor(COLOR_MENU));

			LVCOLUMN column;
			column.mask = LVCF_TEXT | LVCF_FMT | LVCF_WIDTH;
			column.fmt = LVCFMT_LEFT;

			column.cx = 25;
			column.pszText = TEXT("ID");
			ListView_InsertColumn(hWndStreamList, 0, &column);

			column.cx = 100;
			column.pszText = TEXT("IP");
			ListView_InsertColumn(hWndStreamList, 1, &column);

			column.cx = 225;
			column.pszText = TEXT("UID");
			ListView_InsertColumn(hWndStreamList, 2, &column);

			//////////////////////////////////////////////////////////////////////////

			// Add icon to tray..
			NOTIFYICONDATA IconData;
			IconData.cbSize = sizeof(NOTIFYICONDATA);
			IconData.uID = 1;
			IconData.hWnd = hWnd;
			IconData.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
			IconData.hIcon = hIcon;
			IconData.uCallbackMessage = WM_TRAYICON;
			lstrcpy(IconData.szTip, TEXT("Manager"));

			::Shell_NotifyIcon(NIM_ADD, &IconData);

			// Set minimum dimensions
			::SetWindowPos(hWnd, NULL, 0, 0, 0, 0, SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOMOVE);
		}
		break;

	case WM_UPDATE_STREAM_LIST:
		{
			Elvees::CCriticalSection::Owner lock(pThis->m_listSection);

			UUID  streamID;
			UuidString pszUuid;
			TCHAR stStreamID[64];

			CStream *pStream = pThis->m_listStreams.Head();

			LVITEM listItem;
			HWND hWndStreamList = ::GetDlgItem(hWnd, IDC_STREAMS_LIST);

			listItem.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
			listItem.iItem = 0;
			listItem.iSubItem = 0;
			listItem.iImage = 0;
			listItem.lParam = (LPARAM)0;

			ListView_DeleteAllItems(hWndStreamList);

			while(pStream)
			{
				// Insert stream ID
				wsprintf(stStreamID, TEXT("%ld"), pStream->GetID());

				listItem.pszText = stStreamID;
				ListView_InsertItem(hWndStreamList, &listItem);

				// Stream IP

				if(pStream->IsLocal())
				{
					if(Elvees::IsFileExists(pStream->m_stMoniker))
						lstrcpy(stStreamID, TEXT("Local / File"));
					else
						lstrcpy(stStreamID, TEXT("Local / Device"));
				}
				else
				{
					lstrcpy(stStreamID, pStream->m_stMoniker);
				}

				ListView_SetItemText(hWndStreamList, listItem.iItem, 1, stStreamID);

				// Stream UID
				pszUuid = 0;
				streamID = pStream->GetUID();

				if(::UuidToString(&streamID, &pszUuid) == RPC_S_OK && pszUuid)
				{
					wsprintf(stStreamID, TEXT("{%s}"), pszUuid);
					RpcStringFree(&pszUuid);
				}
				else // very unlikely
					lstrcpy(stStreamID, TEXT("{UUID conversion failed}"));

				ListView_SetItemText(hWndStreamList, listItem.iItem, 2, stStreamID);

				pStream = pThis->m_listStreams.Next(pStream);
			}
		}
		break;

	case WM_CLOSE:
		ShowWindow(hWnd, SW_HIDE);
		break;

	case WM_SYSCOMMAND:
		{
			if(wParam == SC_MINIMIZE)
				ShowWindow(hWnd, SW_HIDE);
			else
				bRet = FALSE; 
		}
		break;


	case WM_GETMINMAXINFO:
		{
			PMINMAXINFO pInfo = (PMINMAXINFO)lParam;

			pInfo->ptMinTrackSize.x = 375;
			pInfo->ptMinTrackSize.y = 150;
		}
		break;

	case WM_DESTROY:
		{
			NOTIFYICONDATA IconData = { sizeof(NOTIFYICONDATA) };
			IconData.uID = 1;
			IconData.hWnd = hWnd;
			::Shell_NotifyIcon(NIM_DELETE, &IconData);

			::PostQuitMessage(0);
		}
		break;

	case WM_SIZE:
		{
			int cx = LOWORD(lParam);
			int cy = HIWORD(lParam);

			::SetWindowPos(::GetDlgItem(hWnd, IDC_STREAMS_LIST), NULL, 0, 0, cx, cy, SWP_NOZORDER);
		}
		break;

	case WM_COMMAND:
		{
			WORD wID = LOWORD(wParam); 

			if(ID_TRAY_SHOW == wID)
			{
				if(!::IsWindowVisible(hWnd))
				{
					RECT rcWnd, rcArea;

					::GetWindowRect(hWnd, &rcWnd);
					::SystemParametersInfo(SPI_GETWORKAREA, NULL, &rcArea, NULL);

					::SetWindowPos(hWnd, NULL, 
						rcArea.right  - (rcWnd.right - rcWnd.left),
						rcArea.bottom - (rcWnd.bottom - rcWnd.top),
						0, 0, SWP_NOZORDER|SWP_NOSIZE|SWP_SHOWWINDOW);
				}

				::SetForegroundWindow(hWnd);
			}
			else if(ID_TRAY_UPDATE == wID)
			{
				::SendMessage(hWnd, WM_UPDATE_STREAM_LIST, 0, 0);
			}
			else if(ID_TRAY_EXIT == wID)
			{
				::DestroyWindow(hWnd);
			}
		}
		break;

	case WM_NOTIFY:
		{
			int idCtrl = (int)wParam;
			LPNMHDR pnmh = (LPNMHDR)lParam;

			if(idCtrl == IDC_STREAMS_LIST && pnmh->code == NM_DBLCLK)
			{
				HWND hWndStreamsList = ::GetDlgItem(hWnd, IDC_STREAMS_LIST);
				LVHITTESTINFO listInfo;

				::GetCursorPos(&(listInfo.pt));
				::ScreenToClient(hWndStreamsList, &(listInfo.pt));

				ListView_HitTest(hWndStreamsList, &listInfo);

				if(listInfo.iItem != -1)
				{
					TCHAR stStreamID[64];
					ListView_GetItemText(hWndStreamsList, listInfo.iItem, 2, 
						stStreamID, countof(stStreamID));

					UUID streamUID;
					if(StringToUuid(stStreamID, &streamUID))
					{
						CStream* pStream = pThis->StreamByUID(streamUID);
#if 1
						CDlgShowStream::Create(hWnd, pStream);
						pStream->Release();
#else
						if(pStream)
						{
							CDlgShowStream dlg(pStream);
							dlg.DoModal(hWnd);

							pStream->Release();
						}
#endif
					}

				//	MessageBox(hWnd, stStreamID, TEXT(""), MB_OK);
				}
			}
		}
		break;

	case WM_TRAYICON:
		{
			DWORD wID    = (DWORD)wParam;
			DWORD lEvent = (DWORD)lParam;

			if(wID != 1 || (lEvent != WM_RBUTTONUP && lEvent != WM_LBUTTONDBLCLK))
				return 0;

			// If there's a resource menu with the same ID as the icon, use it as
			// the right-button popup menu. TrayIcon will interprets the first
			// item in the menu as the default command for WM_LBUTTONDBLCLK

			HMENU hMenu = ::LoadMenu(_Module.GetModuleInstance(), MAKEINTRESOURCE(IDR_SYSTRAY_MENU));
			if(hMenu == NULL) return 0;

			HMENU hSubMenu = ::GetSubMenu(hMenu, 0);

			if(lEvent == WM_RBUTTONUP) {
				// Make first menu item the default (bold font)
				::SetMenuDefaultItem(hSubMenu, 0, TRUE);
				// Display the menu at the current mouse location. There's a "bug"
				// (Microsoft calls it a feature) in Windows 95 that requires calling
				// SetForegroundWindow. To find out more, search for Q135788 in MSDN.

				POINT mouse;
				::GetCursorPos(&mouse);

				::SetForegroundWindow(hWnd);
				::TrackPopupMenu(hSubMenu, 0, mouse.x, mouse.y, 0, hWnd, NULL);
				::PostMessage(hWnd, WM_NULL, 0, 0);
			} else
				// double click: execute first menu item
				::SendMessage(hWnd, WM_COMMAND, ::GetMenuItemID(hSubMenu, 0), 0);

			::DestroyMenu(hMenu);
		}
		break;

	default:
		bRet = FALSE;
	}

	return bRet;
}

//////////////////////////////////////////////////////////////////////////
// CStreamServer
//////////////////////////////////////////////////////////////////////////

bool CManager::StartStreamServer(unsigned short port, unsigned long address)
{
	bool bStarted = false;

	try
	{
		m_pStreamServer = new CStreamServer(
			this,
			address, // address to listen on
			port,    // port to listen on
			10,      // max number of sockets to keep in the pool
			10,      // max number of buffers to keep in the pool
			1024);   // buffer size

		if(m_pStreamServer)
		{
			m_pStreamServer->Start();
			m_pStreamServer->StartAcceptingConnections();

			bStarted = true;
		}
	}
	catch(const Elvees::CException& e)
	{
		Elvees::OutputF(Elvees::TError, TEXT("StartStreamServer() %s - %s"),
			e.GetWhere(), e.GetMessage());
	}
	catch(...)
	{
	}

	if(!bStarted && m_pStreamServer)
		StopStreamServer();

	return bStarted;
}

void CManager::StopStreamServer()
{
	if(m_pStreamServer)
	{
		try { m_pStreamServer->WaitForShutdownToComplete(); } catch(...) { }
		try { delete m_pStreamServer; }	catch(...) { }
		m_pStreamServer = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////
// CArchiveServer
//////////////////////////////////////////////////////////////////////////

bool CManager::StartArchiveServer(unsigned short port, unsigned long address)
{
	bool bStarted = false;

	try
	{
		m_pArchiveServer = new CArchiveServer(
			address, // address to listen on
			port,    // port to listen on
			64*1024, // max message size
			10,      // max number of sockets to keep in the pool
			10,      // max number of buffers to keep in the pool
			1024);   // buffer size

		if(m_pArchiveServer)
		{
			m_pArchiveServer->Start();
			m_pArchiveServer->StartAcceptingConnections();

			bStarted = true;
		}
	}
	catch(const Elvees::CException& e)
	{
		Elvees::OutputF(Elvees::TError, TEXT("StartStreamServer() %s - %s"),
			e.GetWhere(), e.GetMessage());
	}
	catch(...)
	{
	}

	if(!bStarted && m_pArchiveServer)
		StopArchiveServer();

	return bStarted;
}

void CManager::StopArchiveServer()
{
	if(m_pArchiveServer)
	{
		try { m_pArchiveServer->WaitForShutdownToComplete(); } catch(...) { }
		try { delete m_pArchiveServer; }	catch(...) { }
		m_pArchiveServer = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////
// CStreamClient
//////////////////////////////////////////////////////////////////////////

bool CManager::StartStreamClients()
{
	bool bStarted = false;

	try
	{
		m_pStreamClient = new CStreamClient(this, 0, 100*1024);

		if(m_pStreamClient)
		{
			m_pStreamClient->Start();
			bStarted = true;
		}
	}
	catch(const Elvees::CException& e)
	{
		Elvees::OutputF(Elvees::TError, TEXT("StartStreamServer() %s - %s"),
			e.GetWhere(), e.GetMessage());
	}
	catch(...)
	{
		Elvees::Output(Elvees::TError, TEXT("StartStreamServer() unexpected exception"));
	}

	if(!bStarted && m_pStreamClient)
		StopStreamClients();

	return bStarted;
}

void CManager::StopStreamClients()
{
	if(m_pStreamClient)
	{
		try { m_pStreamClient->WaitForShutdownToComplete(); } catch(...) { }
		try { delete m_pStreamClient; }	catch(...) { }
		m_pStreamClient = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////
// CArchiveClient
//////////////////////////////////////////////////////////////////////////

bool CManager::StartArchiveClients()
{
	bool bStarted = false;

	try
	{
		m_pArchiveClient = new CArchiveClient(this, 0, 100*1024);

		if(m_pArchiveClient)
		{
			m_pArchiveClient->Start();
			bStarted = true;
		}
	}
	catch(const Elvees::CException& e)
	{
		Elvees::OutputF(Elvees::TError, TEXT("StartStreamServer() %s - %s"),
			e.GetWhere(), e.GetMessage());
	}
	catch(...)
	{
		Elvees::Output(Elvees::TError, TEXT("StartStreamServer() unexpected exception"));
	}

	if(!bStarted && m_pArchiveClient)
		StopArchiveClients();

	return bStarted;
}

void CManager::StopArchiveClients()
{
	if(m_pArchiveClient)
	{
		try { m_pArchiveClient->WaitForShutdownToComplete(); } catch(...) { }
		try { delete m_pArchiveClient; }	catch(...) { }
		m_pArchiveClient = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////
// Main send

bool CManager::SendFrame(Elvees::CIOBuffer *pData)
{
	if(m_pStreamServer)
		return m_pStreamServer->SendFrame(pData);

	return false;
}

//////////////////////////////////////////////////////////////////////////
// CStreamServer events

long CManager::OnConnectionEstablished(LPCTSTR stAddress)
{
	unsigned activeSessions = (unsigned)::InterlockedExchange(&m_activeSessions, m_activeSessions);

	if(activeSessions >= m_maxSessions)
		return SERVER_FULL;

	::InterlockedIncrement(&m_activeSessions);

	long lSession = ::InterlockedIncrement(&m_sessionNumber);

	Elvees::OutputF(Elvees::TInfo, TEXT("Session added(%ld) : %s"), lSession, stAddress);

	return lSession;
}

void CManager::OnConnectionClosed(long lSession)
{
	if(lSession > 0)
	{
		Elvees::OutputF(Elvees::TInfo, TEXT("Session deleted(%ld)"), lSession);
		::InterlockedDecrement(&m_activeSessions);
	}
	else
	{
		Elvees::Output(Elvees::TInfo, TEXT("Session deleted"));
	}
}

//////////////////////////////////////////////////////////////////////////
// Client streams management

void CManager::StartClientStream(LPCTSTR lpAddress)
{
	ULONG addr;

#ifndef _UNICODE
	addr = inet_addr(lpAddress);
#else
	char stAddress[56];

	WideCharToMultiByte(CP_ACP, 0, lpAddress,
		(int)wcslen(lpAddress) + 1,
		stAddress, sizeof(stAddress), NULL, NULL);

	addr = inet_addr(stAddress);
#endif

	if(m_pStreamClient && !IsClientStreamStarted(addr))
	{
		Elvees::OutputF(Elvees::TInfo, TEXT("Connecting to %s"), lpAddress);

		ClientStreamStarted(addr);

		HANDLE hThread;
		unsigned threadID = 0;

		hThread = (HANDLE)::_beginthreadex(0, 0, ThreadConnect, (void*)(LONG_PTR)addr, 0, &threadID);

		if(hThread)
			CloseHandle(hThread);
		else
			ClientStreamStopped(addr);
	}
}

unsigned int __stdcall CManager::ThreadConnect(void *pV)
{
	ULONG addr = (ULONG)(LONG_PTR)pV;
	
	CManager* pManager = GetManager();

	if(pManager && pManager->m_pStreamClient)
	{
		if(pManager->m_pStreamClient->StartStream(addr, 5005))
		{
			Elvees::OutputF(Elvees::TInfo, TEXT("Client %hs started"),
				inet_ntoa(*(in_addr*)&addr));

			return 0;
		}

		pManager->ClientStreamStopped(addr);
	}

	return 1;
}

bool CManager::IsClientStreamStarted(ULONG uConn)
{
	Elvees::CCriticalSection::Owner lock(m_connSection);

	std::list<ULONG>::iterator i;
	for(i = m_listConnections.begin(); i != m_listConnections.end(); ++i)
	{
		if(*i == uConn)
			return true;
	}

	return false;
}

void CManager::ClientStreamStarted(ULONG uConn)
{
	Elvees::CCriticalSection::Owner lock(m_connSection);
	m_listConnections.push_back(uConn);
}

void CManager::ClientStreamStopped(ULONG uConn)
{
    Elvees::CCriticalSection::Owner lock(m_connSection);
	m_listConnections.remove(uConn);
}

bool CManager::CreateArchiveStream(CHCS::IStream** ppStream, long lSourceID, INT64 startPos)
{
	HRESULT hr;
	CDataSource dbsource;
	CSession    dbsession;
	CCommand<CAccessor<CAccCameraID> > cmdCamera;
	
	// Open and read database
	ULONG sourceIP;
	CStreamArchive* pStream = NULL;

	bool bDone = false;

	if(!m_pArchiveClient || !ppStream) 
		return false;

	do
	{	
		// Open database

	#ifdef _UNICODE
		hr = dbsource.OpenFromInitializationString(m_stDBInitString);
	#else
		WCHAR szDBInitString[4096];

		MultiByteToWideChar(CP_ACP, 0, m_stDBInitString, lstrlen(m_stDBInitString) + 1,
			szDBInitString, countof(szDBInitString));

		hr = dbsource.OpenFromInitializationString(szDBInitString);
	#endif
		if(FAILED(hr))
		{
			Elvees::OutputF(Elvees::TError, TEXT("Fail to open database connection hr=0x%08lX"), hr);
			break;
		}

		// Open session

		hr = dbsession.Open(dbsource);
		if(FAILED(hr))
		{
			Elvees::OutputF(Elvees::TError, TEXT("Fail to open database session hr=0x%08lX"), hr);
			break;
		}

		// Save all streams to list
		cmdCamera.m_lCameraID = lSourceID;

		hr = cmdCamera.Open(dbsession);
		if(FAILED(hr))
		{
			Elvees::OutputF(Elvees::TError, TEXT("Fail to open table hr=0x%08lX"), hr);
			break;
		}

		if(cmdCamera.MoveFirst() == S_OK)
		{
		#ifndef _UNICODE
			sourceIP = inet_addr(cmdCamera.m_stCameraIP);
		#else
			char stSourceIP[56];

			WideCharToMultiByte(CP_ACP, 0, cmdCamera.m_stCameraIP,
				(int)wcslen(cmdCamera.m_stCameraIP) + 1,
				stSourceIP, sizeof(stSourceIP), NULL, NULL);

			sourceIP = inet_addr(stSourceIP);
		#endif

			if(INADDR_NONE == sourceIP)
			{
				Elvees::OutputF(Elvees::TError, TEXT("Invalid stream IP = \"%s\""), cmdCamera.m_stCameraIP);
				break;
			}
		}

		pStream = CStreamArchive::CreatStream(lSourceID, startPos);

		if(pStream)
		{
			if(!m_pArchiveClient->StartStream(pStream, sourceIP, 5006))
			{
				pStream->Release();
				break;
			}

			pStream->Start();
			pStream->SeekTo(startPos);

			*ppStream = pStream;
			bDone = true;
		}
	}
	while(false);

	return bDone;
}

//////////////////////////////////////////////////////////////////////////
// Device callback window proc

LRESULT CALLBACK CManager::DeviceEventWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(uMsg == WM_DEVICE_EVENT)
	{
		try
		{
			CDevice* pThis = (CDevice*)lParam;
			pThis->HandleDeviceEvents();
		}
		catch(...)
		{
		}

		return 0;
	}

	return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}

HWND CManager::CreateDeviceEventWnd()
{
	WNDCLASSEX wcex;

	// Register the window class for the main window. 
	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)DeviceEventWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= NULL;
	wcex.hIcon			= 0;
	wcex.hCursor		= NULL;
	wcex.hbrBackground	= NULL;
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName	= DEVICEEVENTWND_CLASS;
	wcex.hIconSm		= 0;

	if(!RegisterClassEx(&wcex))
	{
		if(GetLastError() != 0x00000582) // already registered
			return NULL;
	}

	return ::CreateWindow(DEVICEEVENTWND_CLASS, TEXT(""), 0,
		CW_USEDEFAULT, CW_USEDEFAULT, 
		CW_USEDEFAULT, CW_USEDEFAULT, 
		NULL, 0,
		NULL, 0);
}