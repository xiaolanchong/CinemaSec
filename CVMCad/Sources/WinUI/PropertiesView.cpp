/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: PropertiesView.cpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-05-19
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
#include "Common.hpp"
#include "Resource.hpp"
#include "Configuration.hpp"
#include "Model.hpp"
#include "Graphics.hpp"
#include "WinUI.hpp"
#include "BaseView.hpp"
#include "Pane.hpp"
#include "PropertiesView.hpp"

CPropertiesView::CPropertiesView()
{
	m_hHighlightedItem = NULL;    
}

CPropertiesView::~CPropertiesView()
{

}

BOOL CPropertiesView::Create( CWnd* pParent )
{
	if( !CGenericPane::Create( pParent, L"Scene Properties" ) )
		return FALSE;
	return TRUE;
}


void CPropertiesView::OnUpdate()
{
	m_hHighlightedItem = NULL;

	CScene* pActiveScene = CApplicationManager::GetInstance()->GetActiveScene();
	if( pActiveScene == NULL )
	{
		m_wndTreeView.SetRedraw(FALSE);
		m_wndTreeView.DeleteAllItems();
		m_wndTreeView.SetRedraw(TRUE);
		return;
	}


	//Tree view...
	m_wndTreeView.SetRedraw(FALSE);
	m_wndTreeView.DeleteAllItems();

	CNodePtr pChildNode;
	CNodePtr pParentNode;
	typedef pair<CNodePtr, HTREEITEM> NodePair;
	list<NodePair> nodes;

	wstring sNodeType;
	int nImage = 0;

	pActiveScene->GetType( sNodeType );
	HTREEITEM hRootNode = m_wndTreeView.InsertItem( sNodeType.c_str(), 0,0, TVI_ROOT );
	m_wndTreeView.SetItemData( hRootNode, (DWORD_PTR)pActiveScene );
	nodes.push_back( make_pair( pActiveScene, hRootNode ) ); 

	wstring sNodeTitle;

	for( list<NodePair>::iterator it = nodes.begin() ; it != nodes.end(); ++it )
	{
		pParentNode = it -> first;
		m_wndTreeView.SetItemData( it->second, (DWORD_PTR)(it->first) );

		for( int i = 0 ; pChildNode = pParentNode->GetChild(i); ++i )
		{
			bool bNodeVisible = true;
			//Determine node type 
			if( CCamera* pCamera = dynamic_cast<CCamera*>(pChildNode) )
			{
				//If camera isn't active, image num = 1(NonActive) otherwise =2 
				nImage = pCamera->GetStatus()?2:1;
				bNodeVisible = true;
				pChildNode->GetType( sNodeType );

				std::wstring sCamX, sCamY;
				std::pair<int,int> CamId = pCamera->GetId();
				
				Convert::FromType( CamId.first, sCamX );
				Convert::FromType( CamId.second, sCamY );

				sNodeTitle = sNodeType;
				sNodeTitle += L"(";
				sNodeTitle += sCamX;
				sNodeTitle += L",";
				sNodeTitle += sCamY;
				sNodeTitle += L")";
			}

			else if( CElement* pElement = dynamic_cast<CElement*>(pChildNode) )
			{
				//If element isn't active, image num = 3(NonActive) otherwise =4 
				nImage = pElement->GetStatus()?4:3;
				bNodeVisible = true;
				pChildNode->GetType( sNodeType );

				sNodeTitle = sNodeType;

			}
			else if( Vertex* pVertex = dynamic_cast<Vertex*>(pChildNode) )
			{
				//If Vertex isn't selected, image num = 7(NonActive) otherwise =8 
				nImage = pVertex->selected?8:7;
				bNodeVisible = false;
				pChildNode->GetType( sNodeType );

				sNodeTitle = sNodeType;
			}

			//Insert node
			nodes.push_back( make_pair( pChildNode, m_wndTreeView.InsertItem( sNodeTitle.c_str(), nImage, nImage, it->second ) ) );


			m_wndTreeView.Expand( (it -> second), bNodeVisible?TVE_EXPAND:TVE_COLLAPSE );
		}
	}

	m_wndTreeView.SetRedraw(TRUE);
	m_wndTreeView.UpdateWindow();

}

CNode* CPropertiesView::GetSelectedNode()
{
	CNode* pNode = NULL;
	HTREEITEM hTreeItem = m_wndTreeView.GetSelectedItem();
	if( hTreeItem )
		pNode = (CNode*)m_wndTreeView.GetItemData( hTreeItem );
	return pNode;
}

CNode* CPropertiesView::GetHighlitedNode()
{
	CNode* pNode;
	if( m_hHighlightedItem == NULL )
		pNode = NULL;
	else
		pNode = (CNode*)m_wndTreeView.GetItemData( m_hHighlightedItem );

	return pNode;
}

void CPropertiesView::UpdatePropertiesList( CNodePtr pNode )
{
	wstring sNodeName;
	wstring sNodeType;

	pNode->GetName( sNodeName );
	pNode->GetType( sNodeType );

	m_wndListView.SetRedraw( FALSE );
	m_wndListView.DeleteAllItems();
	m_wndListView.SetRedraw( TRUE );
}

//////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP( CPropertiesView, CGenericPane )
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_CONTEXTMENU()
	ON_NOTIFY( TVN_SELCHANGED, IDR_SCENEPROP_TREE, OnTreeViewItemSelChanged )
	ON_NOTIFY( PTN_ITEMCHANGED, IDR_SCENEPROP_LIST, OnItemChanged)
END_MESSAGE_MAP()


INT CPropertiesView::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	m_wndTreeView.Create( 
		WS_CHILD
		|WS_VISIBLE
		|TVS_HASBUTTONS
		|TVS_HASLINES
		|TVS_LINESATROOT
		|TVS_SHOWSELALWAYS,
		CRect(0,0,0,0), this, IDR_SCENEPROP_TREE );

	m_wndListView.Create( 
		WS_CHILD
		|WS_VISIBLE
		|LVS_SINGLESEL
		|LVS_REPORT,
		CRect(0,0,0,0), this, IDR_SCENEPROP_LIST );


	CBitmap bmp;

	bmp.LoadBitmap( IDB_PROPERTIES_IMAGELIST );

	//Create ImageList with mask
	m_TVImageList.Create( 16, 16, ILC_COLOR24|ILC_MASK, 32,32 );
	m_TVImageList.Add( &bmp, RGB(180,30,150) );
	m_TVImageList.SetBkColor(CLR_NONE);

	m_wndTreeView.SetImageList( &m_TVImageList, TVSIL_NORMAL );

	m_wndCMenu.Create( this );

	CApplicationManager::GetInstance()->AddViewer( this );
	return TRUE;
}

void CPropertiesView::OnDestroy()
{
	CApplicationManager::GetInstance()->RemoveViewer( this );
}

void CPropertiesView::OnSize( UINT nType, int cx, int cy ) 
{
	CGenericPane::OnSize( nType, cx, cy );

	CRect clientRect;
	CSize clientSize;
	CalcClientRect( &clientRect );

	clientSize.cx = clientRect.right - clientRect.left;
	clientSize.cy = clientRect.bottom - clientRect.top;

	int nTreeHeight = (int)floor(clientSize.cy * 0.5);
	int nListHeight = clientSize.cy - nTreeHeight; 

	//Resize list & tree controls
	CRect treeRect( 
		clientRect.left,
		clientRect.top,
		clientSize.cx, 
		nTreeHeight );

	m_wndTreeView.SetWindowPos( NULL, 
		treeRect.left,
		treeRect.top,
		treeRect.right, 
		treeRect.bottom, SWP_NOZORDER );

	CRect listRect(
		clientRect.left,
		treeRect.bottom + treeRect.top + 5,
		clientSize.cx, 
		nListHeight);

	m_wndListView.SetWindowPos( NULL, 
		listRect.left,
		listRect.top,
		listRect.right, 
		listRect.bottom, SWP_NOZORDER );
}


void CPropertiesView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	if( pWnd->m_hWnd = m_wndTreeView.m_hWnd )
	{
		UINT uFlags;
		CPoint ptTree = point;
		m_wndTreeView.ScreenToClient(&ptTree);

		HTREEITEM htItem = m_wndTreeView.HitTest(ptTree, &uFlags);
		if ((htItem != NULL) && (uFlags & TVHT_ONITEM)) {
			if (point.x == -1 && point.y == -1)
			{
				//Keystroke invocation
				CRect rect;
				GetClientRect(&rect);
				ClientToScreen(&rect);
				point = rect.TopLeft();
				point.Offset(5, 5);
			}
			m_wndTreeView.SelectItem( htItem );
			m_hHighlightedItem = htItem;
			m_wndCMenu.Show( this, point );
		}
		else
			CWnd::OnContextMenu(pWnd, point);
	}
}

BOOL CPropertiesView::OnCmdMsg( UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo )
{
	//Redirect all command messages to CommandManager
	if( nCode == CN_COMMAND && CCommandManager::GetInstance()->ProcessCommand( nID ) )
		return true;

	return CWnd::OnCmdMsg( nID, nCode, pExtra, pHandlerInfo );
}

void CPropertiesView::OnTreeViewItemSelChanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	CNode* pNode = (CNode*)m_wndTreeView.GetItemData( pNMTreeView->itemNew.hItem );
	ASSERT( pNode != NULL );

	//m_wndListView.SetRedraw(FALSE);

	m_wndListView.DeleteAllItems();
	CPropTreeItem* pLVModelRootItem;
	CPropTreeItem* pLVNodeTypeItem;
	CPropTreeItem* pLVNodeNameItem;

	pLVModelRootItem = m_wndListView.InsertItem( new CPropTreeItem() );
	pLVModelRootItem->SetLabelText( _T("Node's Properties") );
	pLVModelRootItem->SetInfoText(_T(""));
	
	//Node type
	pLVNodeTypeItem = m_wndListView.InsertItem( new CPropTreeItemStatic(), pLVModelRootItem );
	pLVNodeTypeItem->SetLabelText( _T( "(Type)") );

	//Node name
	pLVNodeNameItem = m_wndListView.InsertItem( new CPropTreeItemStatic(), pLVModelRootItem );
	pLVNodeNameItem->SetLabelText( _T( "(Name)") );

	if( CCamera* pCamera = dynamic_cast<CCamera*>(pNode) )
	{
		wstring tmpString;

		CPropTreeItemEdit* pLVCameraIDX;
		CPropTreeItemEdit* pLVCameraIDY;

		//Camera ID
		pLVCameraIDX = (CPropTreeItemEdit*)m_wndListView.InsertItem( new CPropTreeItemEdit(), pLVModelRootItem );
		pLVCameraIDY = (CPropTreeItemEdit*)m_wndListView.InsertItem( new CPropTreeItemEdit(), pLVModelRootItem );

		pLVCameraIDX->SetLabelText( _T("idx") );
		pLVCameraIDY->SetLabelText( _T("idy") );

		std::pair<int,int> val = pCamera->GetId();

		//idx
		Convert::FromType( val.first, tmpString );
		pLVCameraIDX->SetItemValue( (LPARAM)tmpString.c_str() );

		//idy
		Convert::FromType( val.second, tmpString );
		pLVCameraIDY->SetItemValue( (LPARAM)tmpString.c_str() );


	}
	else if( CElement* pElement = dynamic_cast<CElement*>(pNode) )
	{
		CPropTreeItemCombo* pLVNodeWired;

		//Wired mode
		pLVNodeWired = (CPropTreeItemCombo*)m_wndListView.InsertItem( new CPropTreeItemCombo(), pLVModelRootItem );
		pLVNodeWired->CreateComboBoxBool();
		pLVNodeWired->SetLabelText( _T("Wired") );
		pLVNodeWired->SetItemValue( (LPARAM)pElement->GetWired() );
	}
	else if( Vertex* pVertex = dynamic_cast<Vertex*>( pNode ) )
	{
		wstring tmpString;

		CPropTreeItemStatic* pLVNodeX = 
			(CPropTreeItemStatic*)m_wndListView.InsertItem( new CPropTreeItemStatic(), pLVModelRootItem );
		Convert::FromType( pVertex->x, tmpString );

		pLVNodeX->SetLabelText( L"x" );
		pLVNodeX->SetItemValue( (LPARAM)tmpString.c_str() );

		CPropTreeItemStatic* pLVNodeY = 
			(CPropTreeItemStatic*)m_wndListView.InsertItem( new CPropTreeItemStatic(), pLVModelRootItem );
		Convert::FromType( pVertex->y, tmpString );
		pLVNodeY->SetLabelText( L"y" );
		pLVNodeY->SetItemValue( (LPARAM)tmpString.c_str() );


		CPropTreeItemStatic* pLVNodeTu = 
			(CPropTreeItemStatic*)m_wndListView.InsertItem( new CPropTreeItemStatic(), pLVModelRootItem );
		Convert::FromType( pVertex->tu, tmpString );
		pLVNodeTu->SetLabelText( L"tu" );
		pLVNodeTu->SetItemValue( (LPARAM)tmpString.c_str() );

		CPropTreeItemStatic* pLVNodeTv = 
			(CPropTreeItemStatic*)m_wndListView.InsertItem( new CPropTreeItemStatic(), pLVModelRootItem );
		Convert::FromType( pVertex->tv, tmpString );
		pLVNodeTv->SetLabelText( L"tv" );
		pLVNodeTv->SetItemValue( (LPARAM)tmpString.c_str() );

		//CPropTreeItemColor* pLVNodeTreeItemColor = 
		//	(CPropTreeItemColor*)m_wndListView.InsertItem( new CPropTreeItemColor(), pLVModelRootItem );
		//pLVNodeTreeItemColor->SetLabelText( L"color" );
		//pLVNodeTreeItemColor->SetItemValue( (LPARAM)pVertex->color );
	}

	wstring type;
	pNode->GetType( type );
	pLVNodeTypeItem->SetItemValue( (LPARAM)type.c_str() );
		
	pLVModelRootItem->Expand();

	*pResult = 0;
}

void CPropertiesView::OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult)
{
	LPNMPROPTREE pNMPropTree = (LPNMPROPTREE)pNotifyStruct;

	if(CPropTreeItem* pItem = pNMPropTree->pItem)
	{
		//Determine which node was selected in the TreeItem;
		CNode* pNode = GetSelectedNode();


		if( CCamera* pCamera = dynamic_cast<CCamera*>(pNode) )
		{
			if( wcscmp(pItem->GetLabelText(), L"idx") == 0 )
			{
				wstring w( (wchar_t*)pItem->GetItemValue() );
				pCamera->SetIdX( Convert::ToType<int>(w) );
			}

			if( wcscmp(pItem->GetLabelText(), L"idy") == 0 )
			{
				wstring w( (wchar_t*)pItem->GetItemValue() );
				pCamera->SetIdY( Convert::ToType<int>(w) );
			}
		}

		if( CElement* pElement = dynamic_cast<CElement*>(pNode) )
		{
			if( wcscmp(pItem->GetLabelText(), L"Wired") == 0 )
			{
				pElement->SetWired( pItem->GetItemValue() == 0?false:true );
			}
		}

	}

	CApplicationManager::GetInstance()->NotifyViewers();
	*plResult = 0;
}

