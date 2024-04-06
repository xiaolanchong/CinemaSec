// DlgConfigure.cpp: implementation of the CDlgConfigure class.
//
//////////////////////////////////////////////////////////////////////

#include "chcs.h"
#include "resource.h"

#include "DlgConfigure.h"
#include "DlgCompressor.h"

#include <shlobj.h>

CDlgConfigure::CDlgConfigure()
{
	ZeroMemory(&m_Codec, sizeof(COMPVARS));
	m_Codec.cbSize = sizeof(COMPVARS);
	m_Codec.fccType  = ICTYPE_VIDEO;
	m_Codec.dwFlags &= ~ICMF_COMPVARS_VALID;
	m_Codec.lQ = 10000;

	m_lVideoFPM     = 300L;
	m_lVideoMaxSize = 300L;

	lstrcpy(m_stBasePath, TEXT(""));
	lstrcpy(m_stDBInitString, TEXT(""));

	m_bUpdateBasePath = false;
	m_bUpdateDBString = false;
	m_bUpdateCodec = false;
}

CDlgConfigure::~CDlgConfigure()
{
	FreeCompressor();
}

LRESULT CDlgConfigure::OnInitDialog(UINT, WPARAM, LPARAM, BOOL&)
{
	// center the dialog on the screen
	CenterWindow(GetParent());

	if(LoadSettings())
		InitCompressor();

	SetDlgItemText(IDC_DATABASE, m_stDBInitString);
	SetDlgItemText(IDC_BASEPATH, m_stBasePath);

	SetDlgItemInt(IDC_VIDEO_FPM, m_lVideoFPM);
	SetDlgItemInt(IDC_VIDEO_MAXRATE, m_lVideoMaxSize);

	return TRUE;
};

LRESULT CDlgConfigure::OnSelectCodec(WORD wNotifyCode, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CDlgCompressor dlg;
	
	if(dlg.DoModal(m_hWnd, (LPARAM)&m_Codec) == IDOK)
	{
		UpdateCodecName();
		m_bUpdateCodec = true;
	}

	return 0;
}

bool CDlgConfigure::LoadSettings()
{
	LONG  lRes;
	DWORD dwValue;

	TCHAR stKey[MAX_PATH];
	CRegKey keyCinema;
	CRegKey keyStore;

	// Set default settings
	//

	m_lVideoFPM     = 300L;
	m_lVideoMaxSize = 300L;

	FreeCompressor();
	
	bool bLoaded = false;

	do
	{
		// Load from registry

		if(::LoadString(_Module.GetResourceInstance(), 
			IDS_CHCS_REGISTRY_KEY, stKey, countof(stKey)) == 0)
		{
			Elvees::Output(Elvees::TError, TEXT("Fail load default registry key"));
			break;
		}

		// Read cinema settings
		//

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
			lstrcpy(m_stDBInitString, TEXT(""));

		// Read archive settings
		//

		lstrcpy(stKey, TEXT("Storage"));

		lRes = keyStore.Open(keyCinema, stKey);
		if(lRes != ERROR_SUCCESS)
		{
			Elvees::Output(Elvees::TError, TEXT("Fail open archive setting registry key"));
			break;
		}

		lRes = keyStore.QueryDWORDValue(TEXT("Codec_FCC"), dwValue);
		if(lRes == ERROR_SUCCESS)
			m_Codec.fccHandler = dwValue;

		lRes = keyStore.QueryDWORDValue(TEXT("Codec_Quality"), dwValue);
		if(lRes == ERROR_SUCCESS)
			m_Codec.lQ = dwValue;

		lRes = keyStore.QueryDWORDValue(TEXT("Codec_KeyRate"), dwValue);
		if(lRes == ERROR_SUCCESS)
			m_Codec.lKey = dwValue;

		lRes = keyStore.QueryDWORDValue(TEXT("Codec_DataRate"), dwValue);
		if(lRes == ERROR_SUCCESS)
			m_Codec.lDataRate = dwValue;

		lRes = keyStore.QueryDWORDValue(TEXT("Video_FMP"), dwValue);
		if(lRes == ERROR_SUCCESS)
			m_lVideoFPM = dwValue;

		lRes = keyStore.QueryDWORDValue(TEXT("Video_MaxSize"), dwValue);
		if(lRes == ERROR_SUCCESS)
			m_lVideoMaxSize = dwValue;

		// Load codec state

		if(m_Codec.lpState)
			free(m_Codec.lpState);

		m_Codec.cbState = 0;
		m_Codec.lpState = NULL;

		dwValue = 0;

		lRes = keyStore.QueryBinaryValue(TEXT("Codec_State"), NULL, &dwValue);
		if(lRes != ERROR_SUCCESS)
		{
			Elvees::Output(Elvees::TError, TEXT("Fail to load codec state"));
			break;
		}

		if(dwValue > 0)
		{
			m_Codec.lpState = malloc(dwValue);
			if(!m_Codec.lpState)
			{
				Elvees::Output(Elvees::TError, TEXT("Memory operation failed"));
				break;
			}

			m_Codec.cbState = dwValue;

			lRes = keyStore.QueryBinaryValue(TEXT("Codec_State"), m_Codec.lpState, &dwValue);
			if(lRes != ERROR_SUCCESS)
			{
				Elvees::Output(Elvees::TError, TEXT("Fail to load codec state"));

				free(m_Codec.lpState);

				m_Codec.cbState = 0;
				m_Codec.lpState = NULL;

				break;
			}
		}

		bLoaded = true;
	}
	while(false);

#ifdef _DEBUG
	BYTE bFCC[4];
	bFCC[0] = (BYTE)( m_Codec.fccHandler & 0x000000FF);
	bFCC[1] = (BYTE)((m_Codec.fccHandler & 0x0000FF00) >> 0x08);
	bFCC[2] = (BYTE)((m_Codec.fccHandler & 0x00FF0000) >> 0x10);
	bFCC[3] = (BYTE)((m_Codec.fccHandler & 0xFF000000) >> 0x18);

	if(!isprint(bFCC[0])) bFCC[0] = '?';
	if(!isprint(bFCC[1])) bFCC[1] = '?';
	if(!isprint(bFCC[2])) bFCC[2] = '?';
	if(!isprint(bFCC[3])) bFCC[3] = '?';

	Elvees::OutputF(Elvees::TTrace, TEXT("Configure: FMP=%ld MS=%ld FCC=0x%08lX (%c%c%c%c) Q=%ld KR=%ld DR=%ld SS=%ld"),
		m_lVideoFPM,
		m_lVideoMaxSize,
		m_Codec.fccHandler, bFCC[0], bFCC[1], bFCC[2], bFCC[3],
		m_Codec.lQ,
		m_Codec.lKey,
		m_Codec.lDataRate,
		m_Codec.cbState);
#endif

	return bLoaded;
}

bool CDlgConfigure::SaveSettings()
{
	LONG  lRes;
	DWORD dwValue;

	TCHAR stKey[MAX_PATH];
	CRegKey keyCinema;
	CRegKey keyStore;

	// Set default settings
	//

	Elvees::Output(Elvees::TInfo, TEXT("Saving params"));
	
	bool bSaved = false;

	do
	{
		// Save to registry

		if(::LoadString(_Module.GetResourceInstance(), 
			IDS_CHCS_REGISTRY_KEY, stKey, countof(stKey)) == 0)
		{
			Elvees::Output(Elvees::TError, TEXT("Fail load default registry key"));
			break;
		}

		lRes = keyCinema.Open(HKEY_LOCAL_MACHINE, stKey);

		if(lRes != ERROR_SUCCESS)
			lRes = keyCinema.Create(HKEY_LOCAL_MACHINE, stKey);

		if(lRes != ERROR_SUCCESS)
		{
			Elvees::Output(Elvees::TError, TEXT("Fail open default registry key"));
			break;
		}

		if(m_bUpdateBasePath)
		{
			lRes = keyCinema.SetStringValue(TEXT("ArchivePath"), m_stBasePath);
			if(lRes != ERROR_SUCCESS)
			{
				Elvees::OutputF(Elvees::TWarning, TEXT("Fail to update ArchivePath parameter lRes=%ld"), lRes);
			//	break;
			}
		}

		if(m_bUpdateDBString)
		{
			lRes = keyCinema.SetStringValue(TEXT("DBConnectionString"), m_stDBInitString);
			if(lRes != ERROR_SUCCESS)
			{
				Elvees::OutputF(Elvees::TWarning, TEXT("Fail to update DBConnectionString parameter lRes=%ld"), lRes);
			//	break;
			}
		}

		lstrcat(stKey, TEXT("\\Storage"));

		lRes = keyStore.Open(HKEY_LOCAL_MACHINE, stKey);

		if(lRes != ERROR_SUCCESS)
			lRes = keyStore.Create(HKEY_LOCAL_MACHINE, stKey);

		if(lRes != ERROR_SUCCESS)
		{
			Elvees::Output(Elvees::TError, TEXT("Fail open archive setting registry key"));
			break;
		}

		// Write archive settings
		//

		dwValue = m_lVideoFPM;
		lRes = keyStore.SetDWORDValue(TEXT("Video_FMP"), dwValue);
		if(lRes != ERROR_SUCCESS)
		{
			Elvees::Output(Elvees::TError, TEXT("Fail save FMP"));
			//	break;
		}

		dwValue = m_lVideoMaxSize;
		lRes = keyStore.SetDWORDValue(TEXT("Video_MaxSize"), dwValue);
		if(lRes != ERROR_SUCCESS)
		{
			Elvees::Output(Elvees::TError, TEXT("Fail save MaxSize"));
			//	break;
		}

		// Write codec settings
		//

		if(m_bUpdateCodec)
		{
			dwValue = m_Codec.fccHandler;
			lRes = keyStore.SetDWORDValue(TEXT("Codec_FCC"), dwValue);
			if(lRes != ERROR_SUCCESS)
			{
				Elvees::Output(Elvees::TError, TEXT("Fail save FCC"));
			//	break;
			}

			dwValue = m_Codec.lQ;
			lRes = keyStore.SetDWORDValue(TEXT("Codec_Quality"), dwValue);
			if(lRes != ERROR_SUCCESS)
			{
				Elvees::Output(Elvees::TError, TEXT("Fail save Quality"));
			//	break;
			}

			dwValue = m_Codec.lKey;
			lRes = keyStore.SetDWORDValue(TEXT("Codec_KeyRate"), dwValue);
			if(lRes != ERROR_SUCCESS)
			{
				Elvees::Output(Elvees::TError, TEXT("Fail save KeyRate"));
			//	break;
			}

			dwValue = m_Codec.lDataRate;
			lRes = keyStore.SetDWORDValue(TEXT("Codec_DataRate"), dwValue);
			if(lRes != ERROR_SUCCESS)
			{
				Elvees::Output(Elvees::TError, TEXT("Fail save DataRate"));
			//	break;
			}

			// Save codec state

			if(m_Codec.lpState && m_Codec.cbState > 0)
			{
				dwValue = m_Codec.cbState;

				lRes = keyStore.SetBinaryValue(TEXT("Codec_State"), m_Codec.lpState, dwValue);
				if(lRes != ERROR_SUCCESS)
				{
					Elvees::Output(Elvees::TError, TEXT("Fail to save codec state"));
				//	break;
				}
			}
			else
			{
				dwValue = 0;

				lRes = keyStore.SetBinaryValue(TEXT("Codec_State"), NULL, dwValue);
				if(lRes != ERROR_SUCCESS)
				{
					Elvees::Output(Elvees::TError, TEXT("Fail to save codec state"));
				//	break;
				}
			}
		}

		bSaved = true;
	}
	while(false);

	return bSaved;
}

void CDlgConfigure::InitCompressor()
{
	LRESULT	lRes;
	ICINFO info;
	ICCOMPRESSFRAMES icf;

	do
	{
		if(m_Codec.fccHandler == 0)
		{
			Elvees::Output(Elvees::TError, TEXT("Codec not selected"));
			break;
		}

		m_Codec.hic = ICOpen(ICTYPE_VIDEO, m_Codec.fccHandler, ICMODE_COMPRESS);
		if(!m_Codec.hic)
		{
			Elvees::Output(Elvees::TError, TEXT("Cant open compressor"));
			break;
		}

		if(m_Codec.lpState && m_Codec.cbState > 0)
		{
			lRes = ICSetState(m_Codec.hic, m_Codec.lpState, m_Codec.cbState);
			if(lRes != ICERR_OK && lRes != m_Codec.cbState)
			{
				Elvees::OutputF(Elvees::TError, TEXT("Cant set compressor state lRes=%ld"), lRes);
				break;
			}
		}

		// Retrieve compressor information.
		//

		ZeroMemory(&info, sizeof(info));
		info.dwSize = sizeof(info);

		lRes = ICGetInfo(m_Codec.hic, &info, sizeof(info));
		if(!lRes)
		{
			Elvees::Output(Elvees::TError, TEXT("Unable to get codec info"));
			break;
		}

		// Set quality
		if( !(info.dwFlags & VIDCF_QUALITY))
			m_Codec.lQ = 0;

		// Set DataRate
		ZeroMemory(&icf, sizeof(icf));

		icf.dwFlags		= (DWORD)(LONG_PTR)&icf.lKeyRate;
		icf.lStartFrame = 0;
		icf.lFrameCount = MAXLONG;
		icf.lQuality	= m_Codec.lQ;
		icf.lDataRate	= m_Codec.lDataRate;
		icf.lKeyRate	= m_Codec.lKey;
		icf.dwRate		= 1000000;
		icf.dwScale		= m_lVideoFPM > 0 ? 60000000/m_lVideoFPM : 60000000;

		ICSendMessage(m_Codec.hic, ICM_COMPRESS_FRAMES_INFO, (LPARAM)(LPVOID)&icf, sizeof(ICCOMPRESSFRAMES));
		
		info.szDescription[127] = 0;
		UpdateCodecName(info.szDescription);

		m_Codec.dwFlags = info.dwFlags | ICMF_COMPVARS_VALID;
	}
	while(false);

	if(!(m_Codec.dwFlags & ICMF_COMPVARS_VALID))
	{
		FreeCompressor();
		UpdateCodecName();
	}
}

void CDlgConfigure::FreeCompressor()
{
	if(m_Codec.hic)
	{
		ICClose(m_Codec.hic);
		m_Codec.hic = NULL;
	}

	if(m_Codec.lpState)
	{
		free(m_Codec.lpState);

		m_Codec.lpState = NULL;
		m_Codec.cbState = 0;
	}

	ZeroMemory(&m_Codec, sizeof(COMPVARS));
	m_Codec.cbSize = sizeof(COMPVARS);
	m_Codec.fccType = ICTYPE_VIDEO;
	m_Codec.dwFlags &= ~ICMF_COMPVARS_VALID;
	m_Codec.lQ = 10000;
}

void CDlgConfigure::UpdateCodecName(LPCWSTR szDriverName)
{
	ICINFO info;
	TCHAR stCodecName[128];

	if(szDriverName)
	{
		wsprintf(stCodecName, TEXT("%lS"), szDriverName);
	}
	else
	{
		lstrcpy(stCodecName, TEXT("Not selected"));

		if(m_Codec.dwFlags & ICMF_COMPVARS_VALID)
		{
			if(ICGetInfo(m_Codec.hic, &info, sizeof(info)))
			{
				info.szDescription[127] = 0;
				wsprintf(stCodecName, TEXT("%lS"), info.szDescription);
			}
		}
	}

	if(lstrlen(stCodecName) == 0)
		lstrcpy(stCodecName, TEXT("Unknown"));

	SetDlgItemText(IDC_DRIVER_NAME, stCodecName);
}

LRESULT CDlgConfigure::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(wID);
	return 0;
}

LRESULT CDlgConfigure::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_lVideoFPM = GetDlgItemInt(IDC_VIDEO_FPM, NULL, FALSE);

	if(m_lVideoFPM < 10 || m_lVideoFPM > 60*30)
	{
		MessageBox(TEXT("Set valid frame rate"), TEXT("Invalid frame rate"), MB_OK | MB_ICONSTOP);
		::SetFocus(GetDlgItem(IDC_VIDEO_FPM));
		return 0;
	}

	m_lVideoMaxSize = GetDlgItemInt(IDC_VIDEO_MAXRATE, NULL, FALSE);

	if(m_lVideoMaxSize < 10)
	{
		MessageBox(TEXT("Set valid video part size"), TEXT("Invalid video size"), MB_OK | MB_ICONSTOP);
		::SetFocus(GetDlgItem(IDC_VIDEO_MAXRATE));
		return 0;
	}

	if(!(m_Codec.dwFlags & ICMF_COMPVARS_VALID) || !m_Codec.hic)
	{
		MessageBox(TEXT("Select video compressor"), TEXT("No codec selected"), MB_OK | MB_ICONSTOP);
		::SetFocus(GetDlgItem(IDC_VIDEO_MAXRATE));
		return 0;
	}

	// All params properly setted, save to registry
	//

	if(!SaveSettings())
	{
		MessageBox(TEXT("Fail to save settings"), TEXT("Save settings"), MB_OK | MB_ICONSTOP);
		return 0;
	}

	EndDialog(wID);
	return 0;
}

LRESULT CDlgConfigure::OnBrowseDatabase(WORD wNotifyCode, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	HRESULT hr, hrOpen;
	CDataSource dbsource;

	hrOpen = dbsource.Open();
	if(FAILED(hrOpen))
	{
		if(hrOpen != DB_E_CANCELED)
		{
			TCHAR stMessage[1024];
			int   nMessageLen = 0;

			ULONG i, cRecords;
			CComBSTR bstrDesc;
			CDBErrorInfo ErrorInfo;
			
			hr = ErrorInfo.GetErrorRecords(&cRecords);
			if(FAILED(hr) && ErrorInfo.m_spErrorInfo == NULL)
			{
				wsprintf(stMessage,	TEXT("Unknown error hr=0x%x"), hrOpen);
			}
			else
			{
				LCID lcLocale = GetSystemDefaultLCID();

				for(i = 0; i < cRecords; i++)
				{
					hr = ErrorInfo.GetAllErrorInfo(i, lcLocale, &bstrDesc);
					
					if(FAILED(hr))
					{
						wsprintf(&stMessage[nMessageLen], 
							TEXT("OLE DB Error Record dump retrieval failed hr=0x%x"), hr);
						break;
					}

					wsprintf(&stMessage[nMessageLen], TEXT("%lS\n"), bstrDesc);
					nMessageLen = lstrlen(stMessage);

					bstrDesc.Empty();
				}
			}
		
			MessageBox(stMessage, TEXT("Select database"), MB_OK | MB_ICONSTOP);
		}

		return 0;
	}

	BSTR bstrDBInit;
	hr = dbsource.GetInitializationString(&bstrDBInit, true);

	if(FAILED(hr))
	{
		MessageBox(TEXT("GetInitializationString failed"),
			TEXT("Select database"), MB_OK | MB_ICONSTOP);
		return 0;
	}

#ifdef _UNICODE
	lstrcpy(m_stDBInitString, bstrDBInit);
#else
	WideCharToMultiByte(CP_ACP, 0, bstrDBInit, (int)wcslen(bstrDBInit) + 1,
		m_stDBInitString, sizeof(m_stDBInitString), NULL, NULL);
#endif
	SysFreeString(bstrDBInit);

	SetDlgItemText(IDC_DATABASE, m_stDBInitString);
	m_bUpdateDBString = true;

	return 0;
}

LRESULT CDlgConfigure::OnBrowseBasepath(WORD wNotifyCode, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	HRESULT hr;
	ITEMIDLIST *pItemList;
	BROWSEINFO browseinfo;
	TCHAR stPath[MAX_PATH];

	IMalloc *pMalloc = NULL;
	hr = SHGetMalloc(&pMalloc);
	if(FAILED(hr))
	{
		MessageBox(TEXT("Can't retrieve system's IMalloc interface"),
			TEXT("Browse basepath"), MB_OK | MB_ICONSTOP);
		return 0;
	}

	ZeroMemory(&browseinfo, sizeof(BROWSEINFO));
	
	lstrcpy(stPath, m_stBasePath);

	browseinfo.hwndOwner = m_hWnd;
	browseinfo.pszDisplayName = stPath;
	browseinfo.ulFlags = BIF_NEWDIALOGSTYLE | BIF_RETURNONLYFSDIRS;
	browseinfo.lpszTitle = TEXT("Select BasePath");

	pItemList = ::SHBrowseForFolder(&browseinfo);
	if(pItemList)
	{
		::SHGetPathFromIDList(pItemList, m_stBasePath);
		pMalloc->Free(pItemList);

		SetDlgItemText(IDC_BASEPATH, m_stBasePath);
		m_bUpdateBasePath = true;
	}

	pMalloc->Release();
	pMalloc = NULL;

	return 0;
}