#pragma once

#include "SimpleLog.h"
// CRoomView view

class RoomTag : public SelectionTag
{
	int	m_nRoomID;
public:
	RoomTag(int nRoomID) : m_nRoomID( nRoomID ){}
	int					GetRoomID() const { return m_nRoomID; }
	virtual bool		operator == ( const SelectionTag& rhs )
	{
		const RoomTag* pRoomTag = dynamic_cast< const RoomTag* >( &rhs );
		if( !pRoomTag ) return false;
		else return GetRoomID() == pRoomTag->GetRoomID();
	}
	virtual CString		GetTabName() 	
	{
		CString s;
		s.Format( _T("Room %d"), m_nRoomID);
		return s;
	}
};

class CRoomView : public CSimpleLog
{
	DECLARE_DYNCREATE(CRoomView)
	
	int		m_nRoomID;
	bool	m_bInit;
protected:
	CRoomView();           // protected constructor used by dynamic creation
	virtual ~CRoomView();

	bool	OnReceiveMessage( int nSeverity, LPCWSTR szMessage );
public:
	void	SetRoomID( int nRoomID )	{ m_nRoomID = nRoomID;	}
	int		GetRoomID() const			{ return m_nRoomID;		}

	virtual void OnInitialUpdate();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnDestroy();
	afx_msg void OnRoomRestart();
};


