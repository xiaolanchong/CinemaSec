/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: CamWizardDialog.cpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-06-10
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
#include "Common.hpp"
#include "Resource.hpp"
#include "CamWizardDialog.hpp"


CCamWizardDialog::CCamWizardDialog():
	CDialog( IDD_CAMWIZARD )
{
	m_nCamX = 2;
	m_nCamY = 2;
	m_nCamImgWidth = 500;
	m_nCamImgHeight = 300;
}

CCamWizardDialog::~CCamWizardDialog()
{

}

BOOL CCamWizardDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	return TRUE;
}

void CCamWizardDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	
	DDX_Text(pDX, IDC_CAMX_NUM, m_nCamX );
	DDX_Text(pDX, IDC_CAMY_NUM, m_nCamY );
	DDX_Text(pDX, IDC_CAM_IMG_WIDTH, m_nCamImgWidth );
	DDX_Text(pDX, IDC_CAM_IMG_HEIGHT, m_nCamImgHeight );

	//Validate controls

	DDV_MinMaxInt( pDX, m_nCamX, 1, 6 );
	DDV_MinMaxInt( pDX, m_nCamY, 1, 6 );
 
	DDV_MinMaxInt( pDX, m_nCamImgWidth, 100/*300*/, 1000 );
	DDV_MinMaxInt( pDX, m_nCamImgHeight,100 /*300*/, 1000 );

}


BEGIN_MESSAGE_MAP(CCamWizardDialog, CDialog)
//	ON_BN_CLICKED( IDOK, OnCmdOkClickedButton )
END_MESSAGE_MAP()

bool CCamWizardDialog::CheckDialog()
{
	return false;
}

void CCamWizardDialog::OnCmdOkClickedButton()
{


}
