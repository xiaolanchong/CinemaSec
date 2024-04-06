// DlgCompressor.cpp: implementation of the CDlgCompressor class.
//
//////////////////////////////////////////////////////////////////////

#include "chcs.h"
#include "resource.h"
#include "dlgcompressor.h"

#include <windowsx.h>

CDlgCompressor::CDlgCompressor()
{
	m_hic = 0;
	ZeroMemory(&m_info, sizeof m_info);
	
	m_pCodec = NULL;
	m_bSelected = false;
}

CDlgCompressor::~CDlgCompressor()
{
	FreeCompressor();
}

LRESULT CDlgCompressor::OnInitDialog(UINT, WPARAM, LPARAM lParam, BOOL&)
{
	m_pCodec = reinterpret_cast<COMPVARS*>(lParam);

	CenterWindow(GetParent());

	// No codec selected
	SendDlgItemMessage(IDC_QUALITY_SLIDER, TBM_SETRANGE, TRUE, MAKELONG(0, 100));

	::EnableWindow(GetDlgItem(IDC_CONFIGURE), FALSE);
	::EnableWindow(GetDlgItem(IDC_USE_DATARATE), FALSE);
	::EnableWindow(GetDlgItem(IDC_STATIC_DATARATE), FALSE);
	::EnableWindow(GetDlgItem(IDC_DATARATE), FALSE);
	::EnableWindow(GetDlgItem(IDC_USE_KEYFRAMES), FALSE);
	::EnableWindow(GetDlgItem(IDC_STATIC_KEYRATE), FALSE);
	::EnableWindow(GetDlgItem(IDC_KEYRATE), FALSE);
	::EnableWindow(GetDlgItem(IDC_EDIT_QUALITY), FALSE);
	::EnableWindow(GetDlgItem(IDC_QUALITY_SLIDER), FALSE);

	// Fill compressors list
	
	int i, ind;
	HIC hic;
	ICINFO info;
	TCHAR stInfo[128];

	HWND listCompressors = ::GetDlgItem(m_hWnd, IDC_COMP_LIST);

	if(m_pCodec && m_pCodec->hic)
	{
		lstrcpy(stInfo, TEXT("(Use selected)"));

		ind = ListBox_AddString(listCompressors, stInfo);
		if(ind != LB_ERR)
		{
			ListBox_SelectItemData(listCompressors, ind, 0);
			ListBox_SetCurSel(listCompressors, ind);
			SelectCompressor(0);
		}
	}

	BITMAPINFO bihPAL;
	bihPAL.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bihPAL.bmiHeader.biCompression = mmioFOURCC('Y','U','Y','2');
	bihPAL.bmiHeader.biWidth  = 720;
	bihPAL.bmiHeader.biHeight = 576;
	bihPAL.bmiHeader.biPlanes   = 1;
	bihPAL.bmiHeader.biBitCount = 16;
	bihPAL.bmiHeader.biSizeImage = 720*576*2;
	bihPAL.bmiHeader.biXPelsPerMeter = 0;
	bihPAL.bmiHeader.biYPelsPerMeter = 0;
	bihPAL.bmiHeader.biClrUsed = 0;
	bihPAL.bmiHeader.biClrImportant = 0;

	try
	{
		for(i=0; ICInfo(ICTYPE_VIDEO, i, &info); i++)
		{
			if(info.fccType != ICTYPE_VIDEO
	#ifdef _DEBUG
				// divx codec cause an exception in debug mode
				// may be because of firewall
				|| info.fccHandler == 2021026148
	#endif
				)
				continue;
			
			DEBUG_ONLY(Elvees::OutputF(Elvees::TTrace, TEXT("Codec(%d) [%ld]"), i, info.fccHandler));

			hic = ICOpen(info.fccType, info.fccHandler, ICMODE_COMPRESS);

			if(hic)
			{
				ICINFO ici = { sizeof(ICINFO) };

				if(ICGetInfo(hic, &ici, sizeof(ICINFO)))
				{
					ici.szDescription[127] = 0;

					if(wcslen(ici.szDescription) == 0)
						wcscpy(ici.szDescription, L"Unknown");

					try
					{
						wsprintf(stInfo, TEXT("[%s] %ls"),
							ICCompressQuery(hic, &bihPAL, NULL) == ICERR_OK ? TEXT("+") : TEXT("-"),
							ici.szDescription);

						DEBUG_ONLY(Elvees::OutputF(Elvees::TTrace, TEXT("%d \"%s\""), i, stInfo));

						ind = ListBox_AddString(listCompressors, stInfo);
						if(ind != LB_ERR)
						{
							ListBox_SetItemData(listCompressors, ind, ici.fccHandler);
						}
					}
					catch(...)
					{
						DEBUG_ONLY(Elvees::Output(Elvees::TTrace, TEXT("ups.. unexpected exception")));
					}
				}
				
				ICClose(hic);
			}
		}
	}
	catch(...)
	{
	}

	return TRUE;
};


LRESULT CDlgCompressor::OnUseDatarate(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	::EnableWindow(GetDlgItem(IDC_DATARATE), 
		IsDlgButtonChecked(IDC_USE_DATARATE));

	return 0;
}

LRESULT CDlgCompressor::OnForceKeyframes(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	::EnableWindow(GetDlgItem(IDC_KEYRATE), 
		IsDlgButtonChecked(IDC_USE_KEYFRAMES));

	return 0;
}

LRESULT CDlgCompressor::OnChangeCompressor(WORD /*wNotifyCode*/, WORD /*wID*/, HWND hWndCtl, BOOL& /*bHandled*/)
{
	int ind = (int)SendMessage(hWndCtl, LB_GETCURSEL, 0, 0);
	
	if(ind == LB_ERR)
		return 1;

	DWORD fcc = (DWORD)SendMessage(hWndCtl, LB_GETITEMDATA, ind, 0);
	SelectCompressor(fcc);

	return 0;
}

LRESULT CDlgCompressor::OnQulityEditKillFocus(WORD /*wNotifyCode*/, WORD /*wID*/, HWND hWndCtl, BOOL& /*bHandled*/)
{
	BOOL bRes;
	long lQuality;

	lQuality = (long)GetDlgItemInt(IDC_EDIT_QUALITY, &bRes, TRUE);

	if(!bRes)
	{
		MessageBeep(MB_ICONEXCLAMATION);
		::SetFocus(hWndCtl);
	}

	lQuality = max(0, lQuality);
	lQuality = min(lQuality, 100);

	SetDlgItemInt(IDC_EDIT_QUALITY, lQuality, TRUE);
	SendDlgItemMessage(IDC_QUALITY_SLIDER, TBM_SETPOS, TRUE, lQuality);

	return 0;
}

LRESULT CDlgCompressor::OnHScroll(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	if(lParam)
	{
		long lQuality = (long)SendDlgItemMessage(IDC_QUALITY_SLIDER, TBM_GETPOS, 0, 0);
		SetDlgItemInt(IDC_EDIT_QUALITY, lQuality, FALSE);
	}

	return 0;
}

LRESULT CDlgCompressor::OnConfigure(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	HIC hic = IsUsingSelected() ? (m_pCodec->hic) : m_hic;

	if(hic)
	{
		ICConfigure(hic, m_hWnd);

		if((m_info.dwFlags & VIDCF_QUALITY) == VIDCF_QUALITY)
		{
			long lQuality = 10000;

			if(ICSendMessage(hic, ICM_GETQUALITY, (DWORD_PTR)&lQuality, (DWORD_PTR)0L) == ICERR_OK)
			{
				if(lQuality < 0 || lQuality > 10000)
				{
					DEBUG_ONLY(Elvees::OutputF(Elvees::TTrace, TEXT("Quality=%ld after ICConfigure"), lQuality));
				}
				else
				{
					SetDlgItemInt(IDC_EDIT_QUALITY, lQuality, TRUE);
					SendDlgItemMessage(IDC_QUALITY_SLIDER, TBM_SETPOS, TRUE, lQuality);
				}
			}
		}
	}

	return 0;
}

void CDlgCompressor::SelectCompressor(DWORD fcc)
{
	HIC hic;
	long lValue, lDefault;
	BOOL fSupports, fEnable;
	TCHAR stInfo[128];

	if(m_bSelected)
		FreeCompressor();

	if(fcc == 0)
	{
		hic = m_pCodec ? m_pCodec->hic : NULL;

		if(!ICGetInfo(hic, &m_info, sizeof(ICINFO)))
		{
			m_info.dwFlags = 0;
			m_info.fccHandler = 0;
		}
	}
	else
	{
		do
		{	
			m_hic = ICOpen(ICTYPE_VIDEO, fcc, ICMODE_COMPRESS);
			if(!m_hic)
				break;

			if(!ICGetInfo(m_hic, &m_info, sizeof(ICINFO)))
				break;

			m_bSelected = true;
		}
		while(0);

		if(!m_bSelected)
		{
			FreeCompressor();
			m_info.dwFlags = 0;
			m_info.fccHandler = 0;
		}

		hic = m_hic;
	}

	// Show driver name

	if(hic)
	{
		m_info.szName[15] = 0;
		wsprintf(stInfo, TEXT("\"%lS\" information"), m_info.szName);

		SetDlgItemText(IDC_STATIC_INFO, stInfo);
	}
	else
	{
		SetDlgItemText(IDC_STATIC_INFO, TEXT("Information"));
	}

	// Show driver caps.
	SetDlgItemText(IDC_STATIC_DELTA, m_info.dwFlags & VIDCF_TEMPORAL ? TEXT("Yes") : TEXT("No"));

	// Show driver fourCC code.
	wsprintf(stInfo, TEXT("0x%08lX \'%c%c%c%c\'"),
		m_info.fccHandler,
		(m_info.fccHandler & 0x000000FF),
		(m_info.fccHandler & 0x0000FF00) >> 0x08,
		(m_info.fccHandler & 0x00FF0000) >> 0x10,
		(m_info.fccHandler & 0xFF000000) >> 0x18);

	SetDlgItemText(IDC_STATIC_FOURCC, stInfo);

	// Show driver
	if(hic)
	{
		m_info.szDriver[127] = 0;
		wsprintf(stInfo, TEXT("%lS"), m_info.szDriver);

		SetDlgItemText(IDC_STATIC_DRIVER, stInfo);
	}
	else
	{
		SetDlgItemText(IDC_STATIC_DRIVER, TEXT("Fail to open driver"));
	}

	// Query compressor for caps and enable buttons as appropriate.

	::EnableWindow(GetDlgItem(IDC_CONFIGURE), hic ? ICQueryConfigure(hic) : FALSE);

	fSupports = (m_info.dwFlags & VIDCF_CRUNCH) == VIDCF_CRUNCH;

	::EnableWindow(GetDlgItem(IDC_USE_DATARATE), fSupports);
	::EnableWindow(GetDlgItem(IDC_STATIC_DATARATE), fSupports);

	lValue = (fcc == 0 && m_pCodec) ? m_pCodec->lDataRate : 0;
	SetDlgItemInt(IDC_DATARATE, lValue);

	fEnable = lValue > 0;
	::EnableWindow(GetDlgItem(IDC_DATARATE), fEnable);
	CheckDlgButton(IDC_USE_DATARATE, fEnable ? BST_CHECKED : BST_UNCHECKED);

	// KeyFrameRate

	fSupports = (m_info.dwFlags & VIDCF_TEMPORAL) == VIDCF_TEMPORAL;

	::EnableWindow(GetDlgItem(IDC_USE_KEYFRAMES), fSupports);
	::EnableWindow(GetDlgItem(IDC_STATIC_KEYRATE), fSupports);
	
	lDefault = -1;
	lValue = (fSupports && hic) ? lDefault = ICGetDefaultKeyFrameRate(hic) : 0;
	lValue = (fcc == 0 && m_pCodec) ? m_pCodec->lKey : lValue;

	SetDlgItemInt(IDC_KEYRATE, lValue);

	fEnable = lDefault > 0 && lDefault != lValue;
	::EnableWindow(GetDlgItem(IDC_KEYRATE), fEnable);
	CheckDlgButton(IDC_USE_KEYFRAMES, fEnable? BST_CHECKED : BST_UNCHECKED);

	// Quality

	fSupports = (m_info.dwFlags & VIDCF_QUALITY) == VIDCF_QUALITY;

	::EnableWindow(GetDlgItem(IDC_EDIT_QUALITY), fSupports);
	::EnableWindow(GetDlgItem(IDC_QUALITY_SLIDER), fSupports);

	lValue = (fSupports && hic) ? (ICGetDefaultQuality(hic) + 50)/100 : 100;
	lValue = (fcc == 0 && m_pCodec) ? (m_pCodec->lQ + 50)/100 : lValue;

	SetDlgItemInt(IDC_EDIT_QUALITY, lValue, TRUE);
	SendDlgItemMessage(IDC_QUALITY_SLIDER, TBM_SETPOS, TRUE, lValue);
}

void CDlgCompressor::FreeCompressor()
{
	if(m_hic)
		ICClose(m_hic);

	m_hic = NULL;

	ZeroMemory(&m_info, sizeof m_info);
	m_info.dwFlags = sizeof(ICINFO);

	m_bSelected = false;
}

bool CDlgCompressor::IsUsingSelected()
{
	HWND hWndCtl = GetDlgItem(IDC_COMP_LIST);

	int ind = (int)SendMessage(hWndCtl, LB_GETCURSEL, 0, 0);
	if(ind == LB_ERR)
		return false;

	return (SendMessage(hWndCtl, LB_GETITEMDATA, ind, 0) == 0) ? true : false;
}

LRESULT CDlgCompressor::OnCloseCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(wID);
	return 0;
}

LRESULT CDlgCompressor::OnCloseOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	bool bUseSelected = IsUsingSelected();

	if(!bUseSelected && !m_bSelected)
	{
		MessageBox(TEXT("Select video compressor from the list"), TEXT("No codec selected"), MB_OK | MB_ICONSTOP);
		return 0;
	}

	if(m_pCodec)
	{
		if(m_pCodec->hic && !bUseSelected)
			ICClose(m_pCodec->hic);

		if(m_pCodec->lpState)
			free(m_pCodec->lpState);

		HIC hic = bUseSelected ? m_pCodec->hic : m_hic;

		ZeroMemory(m_pCodec, sizeof(COMPVARS));

		m_pCodec->cbSize  = sizeof(COMPVARS);
		m_pCodec->fccType = ICTYPE_VIDEO;
		m_pCodec->dwFlags = m_info.dwFlags | ICMF_COMPVARS_VALID;
		m_pCodec->fccHandler = m_info.fccHandler;
	
		m_pCodec->lKey = GetDlgItemInt(IDC_KEYRATE, NULL, FALSE);
		m_pCodec->lDataRate = GetDlgItemInt(IDC_DATARATE, NULL, FALSE);

		if(m_info.dwFlags & VIDCF_QUALITY)
			m_pCodec->lQ = (long)SendDlgItemMessage(IDC_QUALITY_SLIDER, TBM_GETPOS, 0, 0)*100;
		else
			m_pCodec->lQ = 10000;

		ICSendMessage(hic, ICM_SETQUALITY, (DWORD)(LONG_PTR)&m_pCodec->lQ, 0);

		// Save configuration state.
		//

		m_pCodec->cbState = ICGetStateSize(hic);

		if(m_pCodec->cbState > 0)
		{
			m_pCodec->lpState = malloc(m_pCodec->cbState);
			if(!m_pCodec->lpState)
			{
				m_pCodec->cbState = 0;
				Elvees::Output(Elvees::TError, TEXT("Memory operation failed"));
			}
			else
			{
				int cbState = (int)ICGetState(hic, m_pCodec->lpState, m_pCodec->cbState);

				if(cbState)
					ICSetState(hic, m_pCodec->lpState, cbState);
			}
		}

		m_pCodec->hic = hic;
		m_hic = NULL;
	}

	EndDialog(wID);
	return 0;
}
