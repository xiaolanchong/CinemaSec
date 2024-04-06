#pragma once

#include "../UT/OXLayoutManager.h"

#include "../GUI/Slider/NewSliderCtrl.h"
#include "../GUI/RangeCtrl/RangeSlider.h"
#include "../Gui/AlphaToolbar/AlphaToolBar.h"
#include "../CommonView/ControlHelper.h"

#include "../RangeSelect.h"

class CArchiveDoc;

// CVideoControlView form view

class CToolBarNoDisable : public CAlphaToolBar
{
public:
	void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
	{
		//UpdateDialogControls(pTarget, bDisableIfNoHndler);
		pTarget = (CFrameWnd*)GetParent();
		CAlphaToolBar::OnUpdateCmdUI( pTarget, bDisableIfNoHndler );
	}
};

class CVideoControlView : public CFormView
{
	DECLARE_DYNCREATE(CVideoControlView)

	CNewSliderCtrl	m_wndSlider;
	CToolBarNoDisable	m_wndVideoControlBar;
	CToolBarNoDisable	m_wndVideoRangeBar;

	COXLayoutManager	m_LayoutManager;
	ControlHelperWithFrame m_CtrlHelper;

	CRangeSelect			m_wndRangeSelect;

	bool m_bInitialized;
	bool m_bEnabled;

	__int64	m_nCurrentMS;
	DWORD	m_dwLastTick;
	CTime	m_timeBegin;

	enum	State
	{
		st_disable,
		st_stop,
		st_play,
		st_pause
	};

	enum	RangeState
	{
		rst_disable,
		rst_idle,
		rst_first,
		rst_selected
	};

	State		m_State;
	RangeState	m_RangeState;
	void	SeekVideoInternal( CTime time, bool bCallDoc );

	CTime	GetCurTime()
	{
		INT64 nCurMS = m_wndSlider.GetCurrent();
		CTimeSpan span( nCurMS / 1000 ) ;
		return m_timeBegin + span ;
	}
protected:
	CVideoControlView();           // protected constructor used by dynamic creation
	virtual ~CVideoControlView();

public:
	enum { IDD = IDD_VIDEO_CONTROL };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	BOOL	Create(  CWnd* pParent, const CRect & rc, UINT nID , CCreateContext& cc);

	void	EnableVideo( CTime timeBegin, CTime timeEnd );
	void	SeekVideo( CTime timeSeek );
	void	SeekForPhoto( CTime timeSeek );
	void	DisableVideo( );
	void	SetExternalTime( CTime timeFrame);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(  );

	CArchiveDoc * GetDocument();

	void	OnTimer( UINT_PTR nIDEvent );
	void	OnPlay();
	void	OnPlayUpdate(CCmdUI* pCmdUI);
	void	OnPause();
	void	OnPauseUpdate(CCmdUI* pCmdUI);
	void	OnStop();
	void	OnStopUpdate(CCmdUI* pCmdUI);

	void	OnSaveRange();
	void	OnSaveRangeUpdate(CCmdUI* pCmdUI);
	void	OnStartRange();
	void	OnStartRangeUpdate(CCmdUI* pCmdUI);
	void	OnEndRange();
	void	OnEndRangeUpdate(CCmdUI* pCmdUI);

	LRESULT	OnProcessSlider( WPARAM wParam, LPARAM lParam );
	DECLARE_MESSAGE_MAP()
};


#ifndef _DEBUG  
inline CArchiveDoc * CVideoControlView::GetDocument() // non-debug version
{
	return reinterpret_cast<CArchiveDoc*>( m_pDocument );
}
#endif
