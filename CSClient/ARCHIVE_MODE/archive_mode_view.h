#pragma once

#include "../CommonView/ArrangeView.h"
#include "../Log_MODE/ClientLogManager.h"
#include "FileTransferDialog.h"

//#define DEBUG_TRANSFER

class CArchiveView : public CArrangeView
{
protected:
  DECLARE_DYNCREATE(CArchiveView)
  CArchiveView();
  virtual ~CArchiveView();

  bool							m_bInitialized;
 
  std::auto_ptr<CFileTransferDialog>	m_pdlgTransfer;

  int									m_nCachedRoomID;

public:
  CArchiveDoc * GetDocument();
  virtual void OnInitialUpdate(  );

  virtual void	PostLoadRoom();
  virtual void	LoadRoom( int nRoomID, const WindowArray_t& wa );

  virtual void	AddContextMenuItem( CMenu& menu );
  virtual void	ProcessContextMenuItem( UINT_PTR nID );
public:
  //{{AFX_VIRTUAL(CArchiveView)
  //}}AFX_VIRTUAL

public:
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump( CDumpContext & dc ) const;
#endif

public:
  //{{AFX_MSG(CArchiveView)
	afx_msg void OnDestroy();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  
inline CArchiveDoc * CArchiveView::GetDocument() // non-debug version
{
  return reinterpret_cast<CArchiveDoc*>( m_pDocument );
}
#endif