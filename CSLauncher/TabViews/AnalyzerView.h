#pragma once

#include "TabSelection.h"

class CLauncherDoc;

// CAnalyzerView view

class CameraTag : public SelectionTag
{
	int		m_nCameraID;
	int		m_nRoomID;
public:
	CameraTag( int c, int r ) : m_nCameraID(c) , m_nRoomID(r){}
	virtual bool		operator == ( const SelectionTag& rhs )
	{
		// assume it's single
		const CameraTag* pTag = dynamic_cast< const CameraTag* >( &rhs );
		return pTag != 0 && pTag->m_nCameraID == m_nCameraID ;
	}
	virtual CString		GetTabName() 	
	{
		CString s;
		s.Format( _T("camera %d[%d]"), m_nCameraID, m_nRoomID );
		return s;
	}
};

class CAnalyzerView :	public CScrollView,
						public TabSelection
{
	DECLARE_DYNCREATE(CAnalyzerView)

	//bool	bHall;
protected:
	CAnalyzerView();           // protected constructor used by dynamic creation
	virtual ~CAnalyzerView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	int			m_nCameraNo;

protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct

	DECLARE_MESSAGE_MAP()
};


