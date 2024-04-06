#pragma once
#include "SimpleLog.h"

// CVideoLogView view
class VideoTag : public SelectionTag
{
public:
	virtual bool		operator == ( const SelectionTag& rhs )
	{
		// assume it's single
		const VideoTag* pTag = dynamic_cast< const VideoTag* >( &rhs );
		return pTag != 0;
	}
	virtual CString		GetTabName() 	
	{
		return CString( _T("Video"));
	}
};

class CVideoLogView : public CSimpleLog
{
	DECLARE_DYNCREATE(CVideoLogView)

protected:
	CVideoLogView();           // protected constructor used by dynamic creation
	virtual ~CVideoLogView();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	bool	OnReceiveMessage( int nSeverity, LPCWSTR szMessage );
protected:
	DECLARE_MESSAGE_MAP()
};


