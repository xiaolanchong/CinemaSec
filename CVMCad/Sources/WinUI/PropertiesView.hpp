/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: PropertiesView.hpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-05-19
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
//#pragma once
#ifndef __PROPERTIESVIEW_HPP__
#define __PROPERTIESVIEW_HPP__


class CPropertiesView : public CGenericPane, public IBaseView
{
public:
	CPropertiesView();
	virtual ~CPropertiesView();

public:
	BOOL Create( CWnd* pParent );


public:
	void OnUpdate();
	CNode* GetSelectedNode();
	CNode* GetHighlitedNode();


protected:
	void UpdatePropertiesList( CNodePtr pNode );

protected:
	afx_msg INT	 OnCreate( LPCREATESTRUCT lpCreateStruct );
	afx_msg void OnSize( UINT nType, int cx, int cy );
	afx_msg void OnDestroy();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg BOOL OnCmdMsg( UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo );


	afx_msg void OnCMenuSceneItem( const CPoint& pt );
	afx_msg void OnCMenuCameraItem( const CPoint& pt );
	afx_msg void OnCMenuElementItem( const CPoint& pt );
	afx_msg void OnTreeViewItemSelChanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult);

	DECLARE_MESSAGE_MAP();



private:
	CImageList	m_TVImageList;
	HTREEITEM	m_hHighlightedItem;
	CTreeCtrl	m_wndTreeView;
	CPropTree	m_wndListView;
	vector< pair< CNode*, bool > > m_TVNodesVisibilityArray;
	


	CPropertiesViewCMenu m_wndCMenu;



};

#endif //__PROPERTIESVIEW_HPP__
