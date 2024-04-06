#pragma once

//#define USE_NEW_SPLITTER
// CReportFrame frame

#include "ChildFrm.h"

#include "../gui/Splitter/FlatSplitterWnd.h"


class CReportFrame : public CChildFrame
{
	DECLARE_DYNCREATE(CReportFrame)
protected:
	CReportFrame();           // protected constructor used by dynamic creation
	virtual ~CReportFrame();

	CFlatSplitterWnd m_wndSplitter;
protected:
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
//	virtual void OnSize(UINT nType, int cx, int cy);
};

