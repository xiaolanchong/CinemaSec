/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: ChannelBar.hpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-05-07
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
//#pragma once
#ifndef __CHANNELBAR_HPP__
#define __CHANNELBAR_HPP__

using namespace std;

class CChannelBar : public CWnd
{
public:
	CChannelBar();
	virtual ~CChannelBar();

public:
	enum Position
	{
		LeftAligned,
		RightAligned
	};

//Methods
public:
	BOOL	Create( CWnd* pParent, CChannelBar::Position p = LeftAligned );
	CSize	GetDefSize();
	void	AddPane( CGenericPane* pPane );
	void	RemovePane( CGenericPane* pPane );
	
// Overrides
public:
	virtual BOOL PreCreateWindow( CREATESTRUCT& cs );
//	virtual void OnUpdateCmdUI(	CFrameWnd* pTarget,	BOOL bDisableIfNoHndler );
//	virtual BOOL OnCmdMsg( UINT nID, int nCode,	void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo );
protected:
	afx_msg void OnSize( UINT nType, int cx, int cy );
	afx_msg void OnLButtonDown(	UINT nFlags, CPoint point );
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP();

private:
	struct Channel
	{
		CRect paneRect;
		CGenericPane* pPane;
		CButton* pButton;
	};
private:
	CGenericPane* m_pActivePane;
	Position m_ePosition;

	CSize m_DefSize;
	CSize m_DefButtonSize;
	CRect m_DefIndent;
	list<Channel> m_Channels;

};


#endif //__CHANNELBAR_HPP__
