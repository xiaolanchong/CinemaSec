//=====================================================================================//
//                                                                                     //
//                                      CinemaSec                                      //
//                                                                                     //
//                              Copyright by ElVEES 2005                               //
//                                                                                     //
//   -------------------------------------------------------------------------------   //
//   Description:                                                                      //
/*
		Chair content loading interface for DataPrepare
                                                                                       */
//                                                                                     //
//   -------------------------------------------------------------------------------   //
//!		\author Eugene Gorbachev                                                       //
//!		\date   15.02.2005                                                             //
//!		modified   2005-10-07                                                          //
//                                                                                     //
//=====================================================================================//
#ifndef __I_CHAIR_CONTENT_H_INCLUDED_8325085304053871__
#define __I_CHAIR_CONTENT_H_INCLUDED_8325085304053871__

#include "../CSAlgo/ALGORITHM/base_chair.h"

//! IChairContent interface GUID 
#define CHAIR_CONTENT_INTERFACE	L"{D7BFD7C7-327D-463C-BFB3-A2DF403D6B50}"

//! ���������� ������ �� �����������
//! \sa Range::CONTENT
enum ChairContents
{
  CHAIR_UNDEFINED = 0,	//!<	�� ����������
  CHAIR_EMPTY,			//!<	�� ��������, �� ���-�� ����
  CHAIR_HUMAN,			//!<	�������
  CHAIR_CLOTHES,		//!<	������/����
  CHAIR_UNKNOWN			//!<	�� ��������, �� ���-�� ����
};

//=====================================================================================//
//                                 class IChairContent                                 //
//=====================================================================================//

//! ��������� ��� ������ � ��������� ������ �� ��������� �����, ������� ��� IChairContentEx
struct IChairContent
{
	//! ������� ������ �������� �� ������� ������
	//! \param	szFileName			��� ����� ��� ��������
	//! \param	Chairs				������ ����������� ������, ��� ������� ��������� �� �������� �� �����
	//!	\return S_OK - �����, ����� - ������
	virtual HRESULT	Load( LPCWSTR szFileName, const std::vector<BaseChair>& Chairs )	= 0;

	//! �������� ���������� ���� ������ ��� ������������� �����
	//! \param	nFrame			����� ����� �� �����
	//! \param	Contents		�������(�����) �������� ������ (������� �� Load) � �� �����������
	//!	\return S_OK - �����, ����� - ������
	virtual HRESULT GetContent( int nFrame, std::map<int, ChairContents>& Contents)	= 0;

	//! ���������� ���������
	virtual void	Release()															= 0;
};

#endif //__I_CHAIR_CONTENT_H_INCLUDED_8325085304053871__
