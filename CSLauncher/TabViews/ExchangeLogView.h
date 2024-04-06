#pragma once
#include "SimpleLog.h"

// CExchangeLogView view
class ExchangeTag : public SelectionTag
{
public:
	virtual bool		operator == ( const SelectionTag& rhs )
	{
		// assume it's single
		const ExchangeTag* pTag = dynamic_cast< const ExchangeTag* >( &rhs );
		return pTag != 0;
	}
	virtual CString		GetTabName() 	
	{
		return CString( _T("Exchange"));
	}
};

class CExchangeLogView : public CSimpleLog
{
	DECLARE_DYNCREATE(CExchangeLogView)

protected:
	CExchangeLogView();           // protected constructor used by dynamic creation
	virtual ~CExchangeLogView();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	bool	OnReceiveMessage( int nSeverity, LPCWSTR szMessage );
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnDestroy();
};


