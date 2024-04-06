#if !defined(AFX_REPORTDOCUMENT_H__37CC9FC9_892D_471F_B3E4_BB25F45FD6EE__INCLUDED_)
#define AFX_REPORTDOCUMENT_H__37CC9FC9_892D_471F_B3E4_BB25F45FD6EE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

class CReportView;
class CReportParamView;

#if 0
struct  FirstReportRow
{
	CTime	timeRecord;
	int		nRoomID;
	int		nTotal;
	int		nSpectators;

	FirstReportRow( CTime t, int id, int tot, int spec ):
		timeRecord(t), nRoomID(id), nTotal(tot), nSpectators(spec)
		{}
};
#endif

namespace XmlLite
{
	class XMLDocument;
}

class CReportDocument : public CDocument
{
protected:
	CReportDocument();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CReportDocument)
	
	template <class T>	T*	GetView()
	{
		POSITION pos = GetFirstViewPosition();
		while( pos )
		{
			CView* pView = GetNextView( pos );
			if( pView->IsKindOf( RUNTIME_CLASS( T ) ) ) return (T*)pView;
		}
		return 0;
	}
// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReportDocument)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CReportDocument();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	void	FillRoomTree( CTreeCtrl* pTree, int nCinemaID   );
	bool	CreateFirstReport( CTime timeBegin, CTime timeEnd, const std::vector<int>& SelectedRooms);

	void	PreviewReport();
	void	PrintReport();
	void	SaveReport();

	// Generated message map functions
protected:
	//{{AFX_MSG(CReportDocument)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_REPORTDOCUMENT_H__37CC9FC9_892D_471F_B3E4_BB25F45FD6EE__INCLUDED_)
