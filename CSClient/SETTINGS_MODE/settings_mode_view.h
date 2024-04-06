#pragma once

#include "../UT/OXLayoutManager.h"
#include "../UT/OXEdit.h"
#include "../RoomChooserTab.h"
#include "afxcmn.h"
#include "afxdtctl.h"

class CSettingsDoc;

// CSettingsView form view

class CSettingsView :	public CFormView, 
						public RoomChooserTab,
						public IRoomChangeObserver
{
	DECLARE_DYNCREATE(CSettingsView)

protected:
	CSettingsView();           // protected constructor used by dynamic creation
	virtual ~CSettingsView();

	bool							m_bInitialized;
	COXLayoutManager				m_LayoutManager;

	CSpinButtonCtrl					m_spinTimeAfter;
	CSpinButtonCtrl					m_spinTimeBefore;
	CSpinButtonCtrl					m_spinTimePeriod;

	CEdit							m_edTimeAfter;
	CEdit							m_edTimeBefore;
	CEdit							m_edTimePeriod;

	CSpinButtonCtrl					m_spinBackupEnd;
	CSpinButtonCtrl					m_spinBackupStart;
	CSpinButtonCtrl					m_spinBackupDate;

	COXNumericEdit					m_edBackupStart;
	COXNumericEdit					m_edBackupEnd;
	CEdit							m_edBackupDate;

	CEdit							m_edExcDelay;
	CEdit							m_edExcWaiting;
	CEdit							m_edExcConnectTime;
	CEdit							m_edExcConnectNumber;

	CSpinButtonCtrl					m_spinExcDelay;
	CSpinButtonCtrl					m_spinExcWaiting;
	CSpinButtonCtrl					m_spinExcConnectTime;
	CSpinButtonCtrl					m_spinExcConnectNumber;

	 CSettingsDoc * GetDocument();

	 void	OnRoomChange(int nRoomID);
	 void	ChangeRoomInControl();
public:
	enum { IDD = IDD_ALGO_SETTINGS };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	BOOL	Create(  CWnd* pParent, const CRect & rc, CCreateContext& cc, int i  );
protected:
	virtual void OnInitialUpdate(  );
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:


	afx_msg void OnBackupStartSpin( NMHDR* pHdr, LRESULT * pResult );
	afx_msg void OnBackupEndSpin( NMHDR* pHdr, LRESULT * pResult );
	afx_msg void OnBnClickedButtonApply();
};

#ifndef _DEBUG  
inline CSettingsDoc * CSettingsView::GetDocument() // non-debug version
{
	return reinterpret_cast<CSettingsDoc*>( m_pDocument );
}
#endif
