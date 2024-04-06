#pragma once

//#define USE_NEW_SPLITTER
// CVideoFrame frame

#include "ChildFrm.h"

#include "../gui/Splitter/FlatSplitterWnd.h"


class CVideoFrame : public CChildFrame
{
	DECLARE_DYNCREATE(CVideoFrame)
protected:
	CVideoFrame();           // protected constructor used by dynamic creation
	virtual ~CVideoFrame();

	CFlatSplitterWnd m_wndSplitter;
protected:
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual void OnSize(UINT nType, int cx, int cy);
};

