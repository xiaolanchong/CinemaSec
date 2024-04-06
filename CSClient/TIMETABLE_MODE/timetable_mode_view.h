#pragma once

//#include "../res/resource.h"
#include "../GUI/Grid/GridCtrl.h"
#include "../UT/OXLayoutManager.h"
#include "RoomChooserTab.h"
#include "../../CSEngine/DBFacet/TimeHelper.h"

class CTimeTableView;
class CTimeTableDoc;

class CGridCtrlEx : public CGridCtrl
{
	CTimeTableView* m_pView;

	virtual void  OnEndEditCell(int nRow, int nCol, CString str);
	virtual BOOL  ValidateEdit(int nRow, int nCol, LPCTSTR str);

	afx_msg	void OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags );

public:
	CGridCtrlEx( CTimeTableView* pView ) : m_pView(pView){}
protected:
	DECLARE_MESSAGE_MAP()
};

// CTimeTableView form view

class CTimeTableView :	public CFormView,  
						public RoomChooserTab,
						public IRoomChangeObserver
{
	DECLARE_DYNCREATE(CTimeTableView)

	enum
	{
		Col_Name	= 0,
		Col_Begin	= 1,
		Col_End		= 2
	};

	void	OnEndEditCell(int nRow, int nCol, CString str);
	BOOL	ValidateEdit(int nRow, int nCol, LPCTSTR str);
	void	OnDeleteKey( bool bShift );

	void	InsertRow();
	void	RefreshGrid();

	std::pair<CTime, CTime>	GetPeriodForSave();
	std::pair<CTime, CTime>	GetPeriodForLoad();
	CTime	GetTrueTimeForLoad( int nHours, int nMinutes);
	CTime	GetTrueTimeForSave( int nHours, int nMinutes);
	CTime	GetTrueTimeInternal( CTime timeStart, int nHours, int nMinutes);

	void	PropagateDay(	CTime timeBegin, CTime timeEnd,
							const std::vector< std::pair<CTime, CTime> >&	DayArr,
							const std::vector<std::wstring>&				DayName,
							TimeTable_t&									FullTimeTable, 
							std::vector<std::wstring>&						FullName,
							std::map<size_t, int>&							Index2RecordMap,
							bool											bIgnoreAfterMidnight);

	void	DumpTimetable(	const TimeTable_t&					FullTimetable, 
							const std::vector<std::wstring>&	FullNames ) const ;
	//CBoxOfficeView*	m_pParentView;

protected:
	CTimeTableView();           // protected constructor used by dynamic creation
	virtual ~CTimeTableView();

	bool							m_bInitialized;

	CDateTimeCtrl					m_wndDate;
	CDateTimeCtrl					m_wndStartTime;
	CGridCtrlEx						m_wndGrid;
	COXLayoutManager				m_LayoutManager;
	CSpinButtonCtrl					m_spinRowNumber;

	CDateTimeCtrl					m_wndDateSaveFrom;
	CDateTimeCtrl					m_wndDateSaveTo;
	CButton							m_btnIgnoreAfterMidnight;

	 CTimeTableDoc * GetDocument();

	 void	OnRoomChange(int nRoomID);
	 void	ChangeRoomInControl();

	 bool	CheckUserDate();

	 static int CALLBACK	CompareItem( LPARAM _1st, LPARAM _2nd, LPARAM _3rd );

	 void	SetRowCount( int nCount );
public:
	enum { IDD = IDD_ALGO_TIMETABLE };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

//	void	SetParent( CBoxOfficeView* pWnd ) { m_pParentView = pWnd ;}
	BOOL	Create(  CWnd* pParent, const CRect & rc, CCreateContext& cc, int i );
protected:
	virtual void OnInitialUpdate(  );
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	afx_msg void OnLoad();
	afx_msg void OnSave();
	afx_msg void OnTimer(UINT_PTR nID);
	afx_msg void OnNotifyDateChange( NMHDR * pNotifyStruct, LRESULT* result );
	afx_msg void OnNotifyRowNumberChange( NMHDR * pNotifyStruct, LRESULT* result );

	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  
inline CTimeTableDoc * CTimeTableView::GetDocument() // non-debug version
{
	return reinterpret_cast<CTimeTableDoc*>( m_pDocument );
}
#endif