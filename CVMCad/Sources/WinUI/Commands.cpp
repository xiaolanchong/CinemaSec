/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: Commands.cpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-05-15
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: Command's hive
*
*
*/
#include "Common.hpp"
#include "Exceptions.hpp"
#include "Configuration.hpp"
#include "Resource.hpp"

#include "Model.hpp"
#include "Graphics.hpp"
#include "Model.hpp"
#include "WinUI.hpp"

#include "Commands.hpp"


//////////////////////////////////////////////////////////////////////////
//CCommand
//////////////////////////////////////////////////////////////////////////
uint CCommand::GetId()
{
	return m_nId;
}
void CCommand::AddExecutor( ICommandExecutor* pExecutor )
{
	m_CmdExecsList.push_back( pExecutor );
}

void CCommand::RemoveExecutor( ICommandExecutor* pExecutor )
{
	m_CmdExecsList.remove( pExecutor );
}

//////////////////////////////////////////////////////////////////////////
//CFileNewCommand
//////////////////////////////////////////////////////////////////////////
CFileNewCommand::CFileNewCommand()
{
	m_nId = ID_MENU_FILE_NEW;
}
void CFileNewCommand::Execute()
{
	CApplicationManager* pAM = CApplicationManager::GetInstance();
	CApplicationConfig* pAC = CApplicationConfig::GetInstance();

	CCamWizardDialog dlg;

	if( dlg.DoModal() == IDOK )
	{
		wstring sTitle;
		pAC->GetDefFileName( sTitle );
		CMainWindow* pMainWindow = pAM->GetMainWindow();
		pMainWindow->SetClientTitle( sTitle );

		CScene* pSceneComponent = new CScene();
		ASSERT( pSceneComponent != NULL );

		pAM->SetActiveScene( pSceneComponent );

		float fX = 0.0f;
		float fY = 0.0f;
		float fElementWidth = (float)dlg.m_nCamImgWidth;
		float fElementHeight = (float)dlg.m_nCamImgHeight;

		for( int i = 0; i < dlg.m_nCamX; ++i )
		{
			for( int j = 0; j < dlg.m_nCamY; ++j )
			{
				CCamera* pCamera = pSceneComponent->AddCamera();
				ASSERT( pCamera != NULL );

				pCamera->SetId( j,i );

				CElement* pElement = pCamera->AddElement();
				ASSERT( pElement != NULL );

				Vertex topLeft, topRight, bottomLeft, bottomRight;

				//Top left vertex
				topLeft.x = fX;
				topLeft.y = fY;
				topLeft.tu = 0.0f;
				topLeft.tv = 0.0f;

				//Bottom left vertex
				bottomLeft.x = fX;
				bottomLeft.y = fY + fElementHeight; 
				bottomLeft.tu = 0.0f; 
				bottomLeft.tv = 1.0f; 

				//Top right vertex
				topRight.x = fX + fElementWidth;
				topRight.y = fY;
				topRight.tu = 1.0f;
				topRight.tv = 0.0f;

				//Bottom right vertex
				bottomRight.x = fX + fElementWidth;
				bottomRight.y = fY + fElementHeight;
				bottomRight.tu = 1.0f;
				bottomRight.tv = 1.0f;

				pElement->AddPoint( topLeft );
				pElement->AddPoint( topRight );
				pElement->AddPoint( bottomLeft );
				pElement->AddPoint( bottomRight );

				CElementToolSet::Triangulate( pElement );
                
	            fX += fElementWidth;
			}

			fX = 0.0f;
			fY += fElementHeight;
		}
	}

	pAM->NotifyViewers();
}
void CFileNewCommand::Unexecute()
{

}

void CFileNewCommand::ProcessUpdate()
{   
	m_bEnabled = ( CApplicationManager::GetInstance()->GetActiveScene() != NULL ) ? false : true;

	CCmdExecsPtrListIt it;
	CCmdExecsPtrListIt begin_it = m_CmdExecsList.begin();
	CCmdExecsPtrListIt end_it = m_CmdExecsList.end();

	for( it = begin_it; it != end_it; ++it )
		(*it)->Enable( m_nId, m_bEnabled );
}

//////////////////////////////////////////////////////////////////////////
//CFileCloseCommand
//////////////////////////////////////////////////////////////////////////
CFileCloseCommand::CFileCloseCommand()
{
	m_nId = ID_MENU_FILE_CLOSE;
}
void CFileCloseCommand::Execute()
{
	CApplicationManager* pAM = CApplicationManager::GetInstance();
	CApplicationConfig* pAC = CApplicationConfig::GetInstance();

	CScene* pScene = pAM->GetActiveScene();

	CMainWindow* pMainWindow = pAM->GetMainWindow();
	pMainWindow->SetClientTitle( L"" );

	ASSERT( pScene != NULL );
	delete pScene;

	pAM->SetActiveScene( NULL );
	pAM->NotifyViewers();
}
void CFileCloseCommand::Unexecute()
{

}
void CFileCloseCommand::ProcessUpdate()
{   
	m_bEnabled = ( CApplicationManager::GetInstance()->GetActiveScene() == NULL ) ? false : true;

	CCmdExecsPtrListIt it;
	CCmdExecsPtrListIt begin_it = m_CmdExecsList.begin();
	CCmdExecsPtrListIt end_it = m_CmdExecsList.end();

	for( it = begin_it; it != end_it; ++it )
		(*it)->Enable( m_nId, m_bEnabled );
}

//////////////////////////////////////////////////////////////////////////
//CFileOpenCommand
//////////////////////////////////////////////////////////////////////////
CFileOpenCommand::CFileOpenCommand()
{
	m_nId = ID_MENU_FILE_OPEN;
	m_bEnabled = true;
}
void CFileOpenCommand::Execute()
{
	CApplicationManager* pAM = CApplicationManager::GetInstance();
	CApplicationConfig* pAC = CApplicationConfig::GetInstance();
	CScene* pScene = pAM->GetActiveScene();

	wchar_t szFilters[]=
		L"CVMCad Files (*.cvm)|*.cvm|All Files (*.*)|*.*||";

	// Create an Open dialog; the default file name extension is ".cvm".
	CFileDialog fileDlg ( TRUE, L"cvm", L"*.cvm",
		OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, szFilters, pAM->GetMainWindow() );

	// Display the file dialog. When user clicks OK, fileDlg.DoModal() 
	// returns IDOK.
	if( fileDlg.DoModal ()==IDOK )
	{
		CString pathName = fileDlg.GetPathName();

		try
		{
			if( pScene != NULL )
			{
				pScene = NULL;
				delete pScene;
			}
			
			CSceneToolSet::LoadScene( pathName.GetBuffer(), &pScene );
			::PathStripPath( pathName.GetBuffer() );
			pAM->GetMainWindow()->SetClientTitle( pathName.GetBuffer() );

			CApplicationManager::GetInstance()->SetActiveScene( pScene );
			CApplicationManager::GetInstance()->NotifyViewers();
		}
		catch(...)
		{

		}
	}
}
void CFileOpenCommand::Unexecute()
{

}
void CFileOpenCommand::ProcessUpdate()
{   
	CCmdExecsPtrListIt it;
	CCmdExecsPtrListIt begin_it = m_CmdExecsList.begin();
	CCmdExecsPtrListIt end_it = m_CmdExecsList.end();

	for( it = begin_it; it != end_it; ++it )
		(*it)->Enable( m_nId, m_bEnabled );
}

//////////////////////////////////////////////////////////////////////////
//CFileSaveCommand
//////////////////////////////////////////////////////////////////////////
CFileSaveCommand::CFileSaveCommand()
{
	m_nId = ID_MENU_FILE_SAVE;
}
void CFileSaveCommand::Execute()
{
	CApplicationManager* pAM = CApplicationManager::GetInstance();
	CApplicationConfig* pAC = CApplicationConfig::GetInstance();

	wchar_t szFilters[]=
		L"CVMCad Files (*.cvm)|*.cvm||";

	// Create an Open dialog; the default file name extension is ".cvm".
	CFileDialog fileDlg ( FALSE, L"cvm", L"*.cvm",
		OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilters,  pAM->GetMainWindow() );

	// Display the file dialog. When user clicks OK, fileDlg.DoModal() 
	// returns IDOK.
	if( fileDlg.DoModal ()==IDOK )
	{
		CString pathName = fileDlg.GetPathName();
		try
		{
			CScene* pScene = pAM->GetActiveScene();
			ASSERT(pScene != NULL );

			CSceneToolSet::SaveScene( pathName.GetBuffer(), pScene );
			::PathStripPath( pathName.GetBuffer() );
			pAM->GetMainWindow()->SetClientTitle( pathName.GetBuffer() );
		}
		catch(...)
		{

		}
	}
	pAM->NotifyViewers();
}
void CFileSaveCommand::Unexecute()
{

}
void CFileSaveCommand::ProcessUpdate()
{   
	m_bEnabled = ( CApplicationManager::GetInstance()->GetActiveScene() == NULL ) ? false : true;

	CCmdExecsPtrListIt it;
	CCmdExecsPtrListIt begin_it = m_CmdExecsList.begin();
	CCmdExecsPtrListIt end_it = m_CmdExecsList.end();

	for( it = begin_it; it != end_it; ++it )
		(*it)->Enable( m_nId, m_bEnabled );
}
//////////////////////////////////////////////////////////////////////////
//CFileExitCommand
//////////////////////////////////////////////////////////////////////////
CFileExitCommand::CFileExitCommand()
{
	m_nId = ID_MENU_FILE_EXIT;
	m_bEnabled = true;
}
void CFileExitCommand::Execute()
{
	CApplicationManager* pAM = CApplicationManager::GetInstance();
	pAM->GetMainWindow()->DestroyWindow();
}
void CFileExitCommand::Unexecute()
{

}
void CFileExitCommand::ProcessUpdate()
{   
	CCmdExecsPtrListIt it;
	CCmdExecsPtrListIt begin_it = m_CmdExecsList.begin();
	CCmdExecsPtrListIt end_it = m_CmdExecsList.end();

	for( it = begin_it; it != end_it; ++it )
		(*it)->Enable( m_nId, m_bEnabled );
}

//////////////////////////////////////////////////////////////////////////
//CAddCameraCommand
//////////////////////////////////////////////////////////////////////////
CAddCameraCommand::CAddCameraCommand()
{
	m_nId = ID_CMENU_ADD_CAMERA;
}
void CAddCameraCommand::Execute()
{
	CApplicationManager* pAM = CApplicationManager::GetInstance();
	CScene* pScene = pAM->GetActiveScene();
	ASSERT(pScene != NULL );

	CCamera* pCamera = pScene->AddCamera();
	pCamera->AddElement();
	pAM->NotifyViewers();
}

void CAddCameraCommand::Unexecute()
{

}
void CAddCameraCommand::ProcessUpdate()
{   
	CApplicationManager* pAM = CApplicationManager::GetInstance();
	CPropertiesView* pPV = pAM->GetPropertiesView();

	CScene* pScene; 
	CNode* pNode = pPV->GetHighlitedNode();
	if( pScene = dynamic_cast<CScene*>(pNode)  )
		m_bEnabled = true;
	else
		m_bEnabled = false;


	CCmdExecsPtrListIt it;
	CCmdExecsPtrListIt begin_it = m_CmdExecsList.begin();
	CCmdExecsPtrListIt end_it = m_CmdExecsList.end();

	for( it = begin_it; it != end_it; ++it )
		(*it)->Enable( m_nId, m_bEnabled );
}


//////////////////////////////////////////////////////////////////////////
//CRemoveCameraCommand
//////////////////////////////////////////////////////////////////////////
CRemoveCameraCommand::CRemoveCameraCommand()
{
	m_nId = ID_CMENU_REMOVE_CAMERA;
}
void CRemoveCameraCommand::Execute()
{
	CApplicationManager* pAM = CApplicationManager::GetInstance();
	CPropertiesView* pPV = pAM->GetPropertiesView();

	CScene* pScene = pAM->GetActiveScene();
	ASSERT( pScene != NULL );

	CCamera* pCamera; 
	CNode* pNode = pPV->GetHighlitedNode();
	if( pCamera = dynamic_cast<CCamera*>(pNode)  )
        pScene->RemoveCamera( pCamera );

	pAM->NotifyViewers();
}

void CRemoveCameraCommand::Unexecute()
{

}
void CRemoveCameraCommand::ProcessUpdate()
{   
	CApplicationManager* pAM = CApplicationManager::GetInstance();
	CPropertiesView* pPV = pAM->GetPropertiesView();

	CCamera* pCamera; 
	CNode* pNode = pPV->GetHighlitedNode();
	if( pCamera = dynamic_cast<CCamera*>(pNode)  )
		m_bEnabled = true;
	else
		m_bEnabled = false;


	CCmdExecsPtrListIt it;
	CCmdExecsPtrListIt begin_it = m_CmdExecsList.begin();
	CCmdExecsPtrListIt end_it = m_CmdExecsList.end();

	for( it = begin_it; it != end_it; ++it )
		(*it)->Enable( m_nId, m_bEnabled );
}

//////////////////////////////////////////////////////////////////////////
//CAddElementCommand
//////////////////////////////////////////////////////////////////////////
CAddElementCommand::CAddElementCommand()
{
	m_nId = ID_CMENU_ADD_ELEMENT;
}
void CAddElementCommand::Execute()
{
	CApplicationManager* pAM = CApplicationManager::GetInstance();
	CPropertiesView* pPV = pAM->GetPropertiesView();

	CCamera* pCamera; 
	CNode* pNode = pPV->GetHighlitedNode();
	if( pCamera = dynamic_cast<CCamera*>(pNode)  )
		pCamera->AddElement();

	pAM->NotifyViewers();
}

void CAddElementCommand::Unexecute()
{

}
void CAddElementCommand::ProcessUpdate()
{   
	CApplicationManager* pAM = CApplicationManager::GetInstance();
	CPropertiesView* pPV = pAM->GetPropertiesView();

	CCamera* pCamera; 
	CNode* pNode = pPV->GetHighlitedNode();
	if( pCamera = dynamic_cast<CCamera*>(pNode)  )
		m_bEnabled = true;
	else
		m_bEnabled = false;


	CCmdExecsPtrListIt it;
	CCmdExecsPtrListIt begin_it = m_CmdExecsList.begin();
	CCmdExecsPtrListIt end_it = m_CmdExecsList.end();

	for( it = begin_it; it != end_it; ++it )
		(*it)->Enable( m_nId, m_bEnabled );
}


//////////////////////////////////////////////////////////////////////////
//CRemoveElementCommand
//////////////////////////////////////////////////////////////////////////
CRemoveElementCommand::CRemoveElementCommand()
{
	m_nId = ID_CMENU_REMOVE_ELEMENT;
}
void CRemoveElementCommand::Execute()
{
	CApplicationManager* pAM = CApplicationManager::GetInstance();
	CPropertiesView* pPV = pAM->GetPropertiesView();
	CScene* pScene = pAM->GetActiveScene();
	ASSERT( pScene != NULL );

	CCamera* pCamera;
	CElement* pElement; 
	CNode* pNode = pPV->GetHighlitedNode();
	if( pElement = dynamic_cast<CElement*>(pNode)  )
	{
		CNode* pParentNode = CNodeToolSet::FindParent( pScene, pNode );
		if( ( pParentNode != NULL ) && ( pCamera = dynamic_cast<CCamera*>(pParentNode) ) )
			pCamera->RemoveElement( pElement );
	}
	pAM->NotifyViewers();
}

void CRemoveElementCommand::Unexecute()
{

}
void CRemoveElementCommand::ProcessUpdate()
{   
	CApplicationManager* pAM = CApplicationManager::GetInstance();
	CPropertiesView* pPV = pAM->GetPropertiesView();

	CElement* pElement; 
	CNode* pNode = pPV->GetHighlitedNode();
	if( pElement = dynamic_cast<CElement*>(pNode)  )
		m_bEnabled = true;
	else
		m_bEnabled = false;


	CCmdExecsPtrListIt it;
	CCmdExecsPtrListIt begin_it = m_CmdExecsList.begin();
	CCmdExecsPtrListIt end_it = m_CmdExecsList.end();

	for( it = begin_it; it != end_it; ++it )
		(*it)->Enable( m_nId, m_bEnabled );
}
//////////////////////////////////////////////////////////////////////////
CSelectCameraSourceCommand::CSelectCameraSourceCommand()
{
	m_nId = ID_CMENU_SELECT_CAMERA_SOURCE;
	m_bChecked = false;
}

void CSelectCameraSourceCommand::Execute()
{
	CApplicationManager* pAM = CApplicationManager::GetInstance();
	CPropertiesView* pPV = pAM->GetPropertiesView();

	wchar_t szFilters[]=
		L"Bitmap Files (*.bmp)|*.bmp|All Files (*.*)|*.*||";

	// Create an Open dialog; the default file name extension is ".cvm".
	CFileDialog fileDlg ( TRUE, L"bmp", L"*.bmp",
		OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, szFilters, pAM->GetMainWindow() );

	// Display the file dialog. When user clicks OK, fileDlg.DoModal() 
	// returns IDOK.
	if( fileDlg.DoModal() ==IDOK )
	{
		CString pathName = fileDlg.GetPathName();

		CScene* pScene = pAM->GetActiveScene();
		ASSERT( pScene != NULL );

		CCamera* pCamera; 
		CNode* pNode = pPV->GetHighlitedNode();
		VERIFY( pCamera = dynamic_cast<CCamera*>(pNode) );

		try
		{

			BITMAPINFO* pBMInfo;

			Gdiplus::Color c;
			HBITMAP hBitmap;
			Gdiplus::Bitmap bmp( pathName.GetBuffer() );
			bmp.GetHBITMAP( c, &hBitmap );
			

			pBMInfo = CTexture::CreateBitmapInfoStruct( hBitmap );

			LONG width = pBMInfo->bmiHeader.biWidth;
			LONG height = pBMInfo->bmiHeader.biHeight;

			int nBytesPerPixel;

			if( pBMInfo->bmiHeader.biBitCount == 32 )
				nBytesPerPixel = 4;
			else if( pBMInfo->bmiHeader.biBitCount == 24 )
				nBytesPerPixel = 3;
			else if( pBMInfo->bmiHeader.biBitCount == 16 )
				nBytesPerPixel = 2;
			else
				return;	//FIXME throw here



			BYTE* pBuffer = new BYTE[  width * height * nBytesPerPixel ];
			::GetBitmapBits( hBitmap, width * height * nBytesPerPixel, (void*)pBuffer );            

			pCamera->SetSource( pBMInfo, pBuffer );

			delete pBuffer;
		}
		catch( CBaseException<CGraphicsDevice> e )
		{
			e.DisplayErrorMessage();
		}
		catch( ... )
		{

		}
	}

	pAM->NotifyViewers();
}

void CSelectCameraSourceCommand::Unexecute()
{


}

void CSelectCameraSourceCommand::ProcessUpdate()
{   
	CApplicationManager* pAM = CApplicationManager::GetInstance();
	CPropertiesView* pPV = pAM->GetPropertiesView();
	CNode* pNode = pPV->GetHighlitedNode();

	if( dynamic_cast<CCamera*>(pNode) )
		m_bEnabled = true;
	else
		m_bEnabled = false;


	CCmdExecsPtrListIt it;
	CCmdExecsPtrListIt begin_it = m_CmdExecsList.begin();
	CCmdExecsPtrListIt end_it = m_CmdExecsList.end();

	for( it = begin_it; it != end_it; ++it )
	{
		(*it)->Enable( m_nId, m_bEnabled  );
	}
}

//////////////////////////////////////////////////////////////////////////
//CMakeActiveElementCommand
//////////////////////////////////////////////////////////////////////////
CMakeActiveElementCommand::CMakeActiveElementCommand()
{
	m_nId = ID_CMENU_MAKE_ACTIVE_ELEMENT;
	m_bChecked = false;
}
void CMakeActiveElementCommand::Execute()
{
	CApplicationManager* pAM = CApplicationManager::GetInstance();
	CPropertiesView* pPV = pAM->GetPropertiesView();
	CScene* pScene = pAM->GetActiveScene();
	ASSERT( pScene != NULL );

	CElement* pElement; 
	CNode* pNode = pPV->GetHighlitedNode();
	if( pElement = dynamic_cast<CElement*>(pNode)  )
	{
		CElementToolSet::DeactivateAll();
		pElement->SetStatus( true );
	}
	pAM->NotifyViewers();
}

void CMakeActiveElementCommand::Unexecute()
{

}
void CMakeActiveElementCommand::ProcessUpdate()
{   
	CApplicationManager* pAM = CApplicationManager::GetInstance();
	CPropertiesView* pPV = pAM->GetPropertiesView();
	CNode* pNode = pPV->GetHighlitedNode();

	if( dynamic_cast<CElement*>(pNode) )
	{
		m_bEnabled = true;

		CElement* pElement;

		if( CElementToolSet::FindActive( &pElement ) && pElement == pNode )
			m_bChecked = true;
		else
			m_bChecked = false;
	}
	else
	{
		m_bChecked = false;
		m_bEnabled = false;
	}


	CCmdExecsPtrListIt it;
	CCmdExecsPtrListIt begin_it = m_CmdExecsList.begin();
	CCmdExecsPtrListIt end_it = m_CmdExecsList.end();

	for( it = begin_it; it != end_it; ++it )
	{
		(*it)->Check( m_nId, m_bChecked );
		(*it)->Enable( m_nId, m_bEnabled  );
	}
}

//////////////////////////////////////////////////////////////////////////
//CMakeActiveCameraCommand
//////////////////////////////////////////////////////////////////////////
CMakeActiveCameraCommand::CMakeActiveCameraCommand()
{
	m_nId = ID_CMENU_MAKE_ACTIVE_CAMERA;
	m_bChecked = false;
	m_bEnabled = false;
}
void CMakeActiveCameraCommand::Execute()
{
	CApplicationManager* pAM = CApplicationManager::GetInstance();
	CPropertiesView* pPV = pAM->GetPropertiesView();
	CScene* pScene = pAM->GetActiveScene();
	ASSERT( pScene != NULL );

	CCamera* pCamera; 
	CNode* pNode = pPV->GetHighlitedNode();
	if( pCamera = dynamic_cast<CCamera*>(pNode)  )
	{
		CCameraToolSet::DeactivateAll();
		pCamera->SetStatus( true );

		CNodePtr pNode = pCamera->GetChild(0);
		if( CElement* pElement = dynamic_cast<CElement*>(pNode) )
		{
			CElementToolSet::DeactivateAll();
			pElement->SetStatus( true );
		}
	}
	pAM->NotifyViewers();
}

void CMakeActiveCameraCommand::Unexecute()
{

}
void CMakeActiveCameraCommand::ProcessUpdate()
{   
	CApplicationManager* pAM = CApplicationManager::GetInstance();
	CPropertiesView* pPV = pAM->GetPropertiesView();
	CNode* pNode = pPV->GetHighlitedNode();

	if( dynamic_cast<CCamera*>(pNode) )
	{
		m_bEnabled = true;

		CCamera* pElement;
		if( CCameraToolSet::FindActive( &pElement ) && pElement == pNode )
			m_bChecked = true;
		else
			m_bChecked = false;
	}
	else
	{
		m_bEnabled = false;
		m_bChecked = false;
	}

	CCmdExecsPtrListIt it;
	CCmdExecsPtrListIt begin_it = m_CmdExecsList.begin();
	CCmdExecsPtrListIt end_it = m_CmdExecsList.end();

	for( it = begin_it; it != end_it; ++it )
	{
		(*it)->Check( m_nId, m_bChecked );
		(*it)->Enable( m_nId, m_bEnabled  );
	}
}

//////////////////////////////////////////////////////////////////////////
//CAddPointCommand
//////////////////////////////////////////////////////////////////////////
CAddPointCommand::CAddPointCommand()
{
	m_nId = ID_CMENU_ADD_POINT;
}
void CAddPointCommand::Execute()
{
	CApplicationManager* pAM = CApplicationManager::GetInstance();
	CPropertiesView* pPV = pAM->GetPropertiesView();
	CGeometryView* pGV = pAM->GetGeometryView();

	CScene* pScene = pAM->GetActiveScene();
	ASSERT( pScene != NULL );

	CElement* pElement;
	if( CElementToolSet::FindActive( &pElement ) )
	{
		Vertex v;
		CPoint pt;
		float panX;
		float panY;
		pGV->GetPan( panX, panY );
		pGV->GetCMenuPoint( pt );

		v.x = (float)(pt.x) - panX;
		v.y = (float)(pt.y) - panY;

		//TexCoords...

		//Find parent camera
		CNode* pNode = CNodeToolSet::FindParent( pScene, pElement );
		if( CCamera* pCamera = dynamic_cast<CCamera*>(pNode) )
		{
			Vertex topLeft, bottomRight;
			VertexPtrArray va;

			//Collect camera's vertices
			CNodeToolSet::CollectNodes<Vertex>( pCamera, va );

			//Get bound rect
			CVertexToolSet::GetBounds( &va, topLeft, bottomRight );

			float fBoundWidth = fabs( bottomRight.x - topLeft.x );
			float fBoundHeight = fabs( bottomRight.y - topLeft.y );

			//Apply mapping
			v.tu = ( v.x - topLeft.x ) / fBoundWidth;
			v.tv = ( v.y - topLeft.y ) / fBoundHeight;

			//If vertex is out of bound, correct it
			if( v.tu < 0.0f )
				v.tu = 0.0f;
			else if( v.tu > 1.0f )
                v.tu = 1.0f;
			
			if( v.tv < 0.0f )
				v.tv = 0.0f;
			else if( v.tv > 1.0f )
				v.tv = 1.0f;
		}

		pElement->AddPoint(v);
		CElementToolSet::Triangulate( pElement );
	}

	pAM->NotifyViewers();

}

void CAddPointCommand::Unexecute()
{


}

void CAddPointCommand::ProcessUpdate()
{   
	CApplicationManager* pAM = CApplicationManager::GetInstance();
	CPropertiesView* pPV = pAM->GetPropertiesView();

	CElement* pElement;
	if( CElementToolSet::FindActive( &pElement ) )
		m_bEnabled = true;
	else
		m_bEnabled = false;

	CCmdExecsPtrListIt it;
	CCmdExecsPtrListIt begin_it = m_CmdExecsList.begin();
	CCmdExecsPtrListIt end_it = m_CmdExecsList.end();

	for( it = begin_it; it != end_it; ++it )
		(*it)->Enable( m_nId, m_bEnabled );
}

//////////////////////////////////////////////////////////////////////////
//CRemovePointsCommand
//////////////////////////////////////////////////////////////////////////
CRemovePointsCommand::CRemovePointsCommand()
{
	m_nId = ID_CMENU_REMOVE_POINTS;
}
void CRemovePointsCommand::Execute()
{
	CApplicationManager* pAM = CApplicationManager::GetInstance();
	CScene* pScene = pAM->GetActiveScene();

	VertexArray* pVertexArray;
	IndexArray* pIndexArray;


	CElementPtrArray elementArray;
	CNodeToolSet::CollectNodes<CElement>( pScene, elementArray );

	for( CElementPtrArrayIt it = elementArray.begin(); it != elementArray.end(); ++it)
	{
		(*it)->GetVertices( &pVertexArray );
		(*it)->GetIndices( &pIndexArray );

		VertexArrayIt pos = pVertexArray->begin();

		while ( pos != pVertexArray->end() )
		{
			if( (*pos).selected == true )
			{
				if ( !pIndexArray->empty() )
					pIndexArray->clear();

				if( pVertexArray->empty() )
					break;

				pVertexArray->erase( pos );
				pos = pVertexArray->begin();
				continue;
			}

			++pos;
		}


		CElementToolSet::Triangulate( *it );
	}

	pAM->NotifyViewers();
}

void CRemovePointsCommand::Unexecute()
{


}

void CRemovePointsCommand::ProcessUpdate()
{   
	CApplicationManager* pAM = CApplicationManager::GetInstance();
	CScene* pScene = pAM->GetActiveScene();

	VertexPtrArray vertices;
	CNodeToolSet::CollectNodes<Vertex>( pScene, vertices );

	int nSelectedNum = 0;

	for( VertexPtrArrayIt pos = vertices.begin(); pos != vertices.end(); ++pos )
	{
		if( (*pos)->selected == true )
		{
			nSelectedNum++;
			break;
		}
	}

	if( nSelectedNum == 0 )
		m_bEnabled = false;
	else
		m_bEnabled = true;

	CCmdExecsPtrListIt it;
	CCmdExecsPtrListIt begin_it = m_CmdExecsList.begin();
	CCmdExecsPtrListIt end_it = m_CmdExecsList.end();

	for( it = begin_it; it != end_it; ++it )
		(*it)->Enable( m_nId, m_bEnabled );
}

//////////////////////////////////////////////////////////////////////////
//CTriangulateCommand
//////////////////////////////////////////////////////////////////////////
CTriangulateCommand::CTriangulateCommand()
{
	m_nId = ID_CMENU_TRIANGULATE_ELEMENTS_POINTS;
	m_bEnabled = false;
}
void CTriangulateCommand::Execute()
{
	CApplicationManager* pAM = CApplicationManager::GetInstance();
	CPropertiesView* pPV = pAM->GetPropertiesView();
	CGeometryView* pGV = pAM->GetGeometryView();
	CNode* pNode = pPV->GetHighlitedNode();

	CElement* pElement;
	if( pElement = dynamic_cast<CElement*>(pNode) )
		CElementToolSet::Triangulate( pElement );

	pAM->NotifyViewers();
}

void CTriangulateCommand::Unexecute()
{

}

void CTriangulateCommand::ProcessUpdate()
{   
	CApplicationManager* pAM = CApplicationManager::GetInstance();
	CPropertiesView* pPV = pAM->GetPropertiesView();
	CNode* pNode = pPV->GetHighlitedNode();

	if( dynamic_cast<CElement*>(pNode) )
		m_bEnabled = true;
	else
		m_bEnabled = false;

	CCmdExecsPtrListIt it;
	CCmdExecsPtrListIt begin_it = m_CmdExecsList.begin();
	CCmdExecsPtrListIt end_it = m_CmdExecsList.end();

	for( it = begin_it; it != end_it; ++it )
		(*it)->Enable( m_nId, m_bEnabled );
}
//////////////////////////////////////////////////////////////////////////
//CApplyNormalMappingCommand
//////////////////////////////////////////////////////////////////////////
CApplyNormalMappingCommand::CApplyNormalMappingCommand()
{
	m_nId = ID_CMENU_APPLY_NORMAL_MAPPING;
	m_bEnabled = false;
}
void CApplyNormalMappingCommand::Execute()
{
	CApplicationManager* pAM = CApplicationManager::GetInstance();
	CPropertiesView* pPV = pAM->GetPropertiesView();
	CNode* pNode = pPV->GetHighlitedNode();

	CCamera* pCamera;
	if( pCamera = dynamic_cast<CCamera*>(pNode) )
		CCameraToolSet::ApplyNormalMapping( pCamera );

	pAM->NotifyViewers();
}

void CApplyNormalMappingCommand::Unexecute()
{

}

void CApplyNormalMappingCommand::ProcessUpdate()
{   
	CApplicationManager* pAM = CApplicationManager::GetInstance();
	CPropertiesView* pPV = pAM->GetPropertiesView();
	CNode* pNode = pPV->GetHighlitedNode();

	if( dynamic_cast<CCamera*>(pNode) )
		m_bEnabled = true;
	else
		m_bEnabled = false;

	CCmdExecsPtrListIt it;
	CCmdExecsPtrListIt begin_it = m_CmdExecsList.begin();
	CCmdExecsPtrListIt end_it = m_CmdExecsList.end();

	for( it = begin_it; it != end_it; ++it )
		(*it)->Enable( m_nId, m_bEnabled );
}

//////////////////////////////////////////////////////////////////////////
//CGVSelectTool
//////////////////////////////////////////////////////////////////////////
CGVSelectTool::CGVSelectTool()
{
	m_nId = ID_TOOL_SELECT;
	m_bEnabled = true;
	m_bChecked = true;
}
void CGVSelectTool::Execute()
{
	CApplicationManager* pAM = CApplicationManager::GetInstance();
	CPropertiesView* pPV = pAM->GetPropertiesView();
	CGeometryView* pGV = pAM->GetGeometryView();

	m_bChecked = m_bChecked?false:true;

	pGV->ChangeTool( ID_TOOL_SELECT );
	pAM->NotifyViewers();
}

void CGVSelectTool::Unexecute()
{


}

void CGVSelectTool::ProcessUpdate()
{   
	CApplicationManager* pAM = CApplicationManager::GetInstance();
	CPropertiesView* pPV = pAM->GetPropertiesView();
	CGeometryView* pGV = pAM->GetGeometryView();

    
	if( ID_TOOL_SELECT == pGV->GetActiveTool()  )
		m_bChecked = true;
	else
		m_bChecked = false;

	CCmdExecsPtrListIt it;
	CCmdExecsPtrListIt begin_it = m_CmdExecsList.begin();
	CCmdExecsPtrListIt end_it = m_CmdExecsList.end();

	for( it = begin_it; it != end_it; ++it )
	{
		(*it)->Enable( m_nId, m_bEnabled );
		(*it)->Check( m_nId, m_bChecked );
	}
}

//////////////////////////////////////////////////////////////////////////
//CGVPanTool
//////////////////////////////////////////////////////////////////////////
CGVPanTool::CGVPanTool()
{
	m_nId = ID_TOOL_PAN;
	m_bEnabled = true;
	m_bChecked = false;
}
void CGVPanTool::Execute()
{
	CApplicationManager* pAM = CApplicationManager::GetInstance();
	CPropertiesView* pPV = pAM->GetPropertiesView();
	CGeometryView* pGV = pAM->GetGeometryView();

	m_bChecked = m_bChecked?false:true;

	pGV->ChangeTool( ID_TOOL_PAN );
	pAM->NotifyViewers();
}

void CGVPanTool::Unexecute()
{

}

void CGVPanTool::ProcessUpdate()
{   
	CApplicationManager* pAM = CApplicationManager::GetInstance();
	CPropertiesView* pPV = pAM->GetPropertiesView();
	CGeometryView* pGV = pAM->GetGeometryView();


	if( ID_TOOL_PAN == pGV->GetActiveTool()  )
		m_bChecked = true;
	else
		m_bChecked = false;

	CCmdExecsPtrListIt it;
	CCmdExecsPtrListIt begin_it = m_CmdExecsList.begin();
	CCmdExecsPtrListIt end_it = m_CmdExecsList.end();

	for( it = begin_it; it != end_it; ++it )
	{
		(*it)->Enable( m_nId, m_bEnabled );
		(*it)->Check( m_nId, m_bChecked );
	}
}

//////////////////////////////////////////////////////////////////////////
//CGVZoomTool
//////////////////////////////////////////////////////////////////////////
CGVZoomTool::CGVZoomTool()
{
	m_nId = ID_TOOL_ZOOM;
	m_bEnabled = true;
	m_bChecked = false;
}
void CGVZoomTool::Execute()
{
	CApplicationManager* pAM = CApplicationManager::GetInstance();
	CPropertiesView* pPV = pAM->GetPropertiesView();
	CGeometryView* pGV = pAM->GetGeometryView();

	m_bChecked = m_bChecked?false:true;

	pGV->ChangeTool( ID_TOOL_ZOOM );
	pAM->NotifyViewers();
}

void CGVZoomTool::Unexecute()
{


}

void CGVZoomTool::ProcessUpdate()
{   
	CApplicationManager* pAM = CApplicationManager::GetInstance();
	CPropertiesView* pPV = pAM->GetPropertiesView();
	CGeometryView* pGV = pAM->GetGeometryView();

	if( ID_TOOL_ZOOM == pGV->GetActiveTool()  )
		m_bChecked = true;
	else
		m_bChecked = false;

	CCmdExecsPtrListIt it;
	CCmdExecsPtrListIt begin_it = m_CmdExecsList.begin();
	CCmdExecsPtrListIt end_it = m_CmdExecsList.end();

	for( it = begin_it; it != end_it; ++it )
	{
		(*it)->Enable( m_nId, m_bEnabled );
		(*it)->Check( m_nId, m_bChecked );
	}
}

//////////////////////////////////////////////////////////////////////////
//CGVHorizontalAlignCommand
//////////////////////////////////////////////////////////////////////////
CGVHorizontalAlignCommand::CGVHorizontalAlignCommand()
{
	m_nId = ID_TOOL_HORIZONTAL_ALIGN;
}
void CGVHorizontalAlignCommand::Execute()
{
	CApplicationManager* pAM = CApplicationManager::GetInstance();
	CPropertiesView* pPV = pAM->GetPropertiesView();
	CScene* pScene = pAM->GetActiveScene();

	if( pScene != NULL )
	{
		VertexPtrArray vertices;
		CNodeToolSet::CollectNodes<Vertex>( pScene, vertices );

	}


	VertexPtrArray va;
	VertexPtrArray vaSelected;

	CNodeToolSet::CollectNodes<Vertex>( pScene, va );

	for( size_t i = 0; i < va.size(); ++i )
	{
		if( va[i]->selected )
		{
			vaSelected.push_back( va[i] );
		}
	}

	CVertexToolSet::Align( &vaSelected, CVertexToolSet::ToolOptions::HorizontalAlign );
	pAM->NotifyViewers();
}

void CGVHorizontalAlignCommand::Unexecute()
{


}

void CGVHorizontalAlignCommand::ProcessUpdate()
{   
	CApplicationManager* pAM = CApplicationManager::GetInstance();
	CScene* pScene = pAM->GetActiveScene();



	VertexPtrArray vertices;
	if( pScene != NULL )
	{
		CNodeToolSet::CollectNodes<Vertex>( pScene, vertices );
	}


	int nSelectedNum = 0;

	for( VertexPtrArrayIt pos = vertices.begin(); pos != vertices.end(); ++pos )
	{
		if( (*pos)->selected == true )
		{
			nSelectedNum++;
			break;
		}
	}

	if( nSelectedNum == 0 )
		m_bEnabled = false;
	else
		m_bEnabled = true;

	CCmdExecsPtrListIt it;
	CCmdExecsPtrListIt begin_it = m_CmdExecsList.begin();
	CCmdExecsPtrListIt end_it = m_CmdExecsList.end();

	for( it = begin_it; it != end_it; ++it )
		(*it)->Enable( m_nId, m_bEnabled );
}
//////////////////////////////////////////////////////////////////////////
//CGVVerticalAlignCommand
//////////////////////////////////////////////////////////////////////////
CGVVerticalAlignCommand::CGVVerticalAlignCommand()
{
	m_nId = ID_TOOL_VERTICAL_ALIGN;
}
void CGVVerticalAlignCommand::Execute()
{
	CApplicationManager* pAM = CApplicationManager::GetInstance();
	CPropertiesView* pPV = pAM->GetPropertiesView();
	CScene* pScene = pAM->GetActiveScene();


	VertexPtrArray va;
	VertexPtrArray vaSelected;

	CNodeToolSet::CollectNodes<Vertex>( pScene, va );

	for( size_t i = 0; i < va.size(); ++i )
	{
		if( va[i]->selected )
		{
			vaSelected.push_back( va[i] );
		}
	}

	CVertexToolSet::Align( &vaSelected, CVertexToolSet::ToolOptions::VerticalAlign );
   	pAM->NotifyViewers();
}

void CGVVerticalAlignCommand::Unexecute()
{


}

void CGVVerticalAlignCommand::ProcessUpdate()
{   
	CApplicationManager* pAM = CApplicationManager::GetInstance();
	CScene* pScene = pAM->GetActiveScene();


	VertexPtrArray vertices;

	if( pScene != NULL )
		CNodeToolSet::CollectNodes<Vertex>( pScene, vertices );

	int nSelectedNum = 0;

	for( VertexPtrArrayIt pos = vertices.begin(); pos != vertices.end(); ++pos )
	{
		if( (*pos)->selected == true )
		{
			nSelectedNum++;
			break;
		}
	}

	if( nSelectedNum == 0 )
		m_bEnabled = false;
	else
		m_bEnabled = true;

	CCmdExecsPtrListIt it;
	CCmdExecsPtrListIt begin_it = m_CmdExecsList.begin();
	CCmdExecsPtrListIt end_it = m_CmdExecsList.end();

	for( it = begin_it; it != end_it; ++it )
		(*it)->Enable( m_nId, m_bEnabled );
}