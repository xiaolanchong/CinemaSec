/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: CamWizardDialog.hpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-06-10
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
#pragma once
#ifndef __CAMWIZARDDIALOG_HPP__
#define __CAMWIZARDDIALOG_HPP__

class CCamWizardDialog : public CDialog
{
public:
	CCamWizardDialog();
	virtual ~CCamWizardDialog();
	bool CheckDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    
	virtual BOOL OnInitDialog();

	afx_msg void OnCmdOkClickedButton();

protected:
	DECLARE_MESSAGE_MAP()

protected:
	CEdit m_wndCamX;
	CEdit m_wndCamY;
	CEdit m_wndCamImgWidth;
	CEdit m_wndCamImgHeight;

public:
	int	m_nCamX;
	int m_nCamY;
	int m_nCamImgWidth;
	int m_nCamImgHeight;
};




#endif //__CAMWIZARDDIALOG_HPP__
