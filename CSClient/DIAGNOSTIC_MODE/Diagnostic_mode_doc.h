#pragma once

#if _MSC_VER <= 1200
#pragma warning( disable : 4097 )
#endif 

#include "../../CSEngine/DBFacet/TimeHelper.h"
#include "../../CSEngine/diagnostic/ISystemDiagnostic.h"
#include "../../CSEngine/CSEngine.h"
#include <boost/shared_ptr.hpp>

class COXTreeCtrl;

class CDiagnosticView;


class CDiagnosticDoc : public CDocument
{

	boost::shared_ptr<ISystemDiagnostic>	m_pDiagnostic;
protected:
  DECLARE_DYNCREATE(CDiagnosticDoc)
  CDiagnosticDoc();
  virtual ~CDiagnosticDoc();

  bool		RefreshChildren( COXTreeCtrl* pTree, CString sServerName, HTREEITEM htParent);
  bool		RefreshExchangeItem ( COXTreeCtrl* pTree, CString sName, HTREEITEM hti );
  void		InsertCameraItem(  COXTreeCtrl* pTree, HTREEITEM htiParent, int nID );
  HTREEITEM	InsertComputerItem(  COXTreeCtrl* pTree, CString sName, int nRoomID);
  void		InsertExchangeItem ( COXTreeCtrl* pTree, CString sName, HTREEITEM htParent );
public:

  CDiagnosticView*		GetDiagnosticView();

	void				CreateDiagnostic();
	void				DestroyDiagnostic();
	void				RefreshDiagnostic(int nRoomID, COXTreeCtrl* pTree);
	void				CreateRoomDiagnostic(int nRoomID, COXTreeCtrl* pTree);
	void				OnRClickItem( COXTreeCtrl* pTree, HTREEITEM hti, CWnd* pThis  );

public:
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump( CDumpContext & dc ) const;
#endif

public:
  //{{AFX_MSG(CDiagnosticDoc)

  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};


