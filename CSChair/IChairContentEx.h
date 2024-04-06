//=====================================================================================//
//                                                                                     //
//                                      CinemaSec                                      //
//                                                                                     //
//                              Copyright by ElVEES 2005                               //
//                                                                                     //
//   -------------------------------------------------------------------------------   //
//   Description: ��������� ��� �������� ����� 
//					������� ������, ��� �������� ���� �������, �� �����-���� ����� ����� 
//					����������� ��������� ���������, � ������������ � ��� ����������(����������)
//					������������ ���� ����� ���������� ��� ���� ������ - �������� �����
//                                                                                     //
//   -------------------------------------------------------------------------------   //
//   Author: Eugene Gorbachev                                                          //
//   Date:   15.02.2005                                                                //
//                                                                                     //
//=====================================================================================//
#ifndef __I_CHAIR_CONTENT_EX_H_INCLUDED_3421736227037312__
#define __I_CHAIR_CONTENT_EX_H_INCLUDED_3421736227037312__

#include <list>
#include "../CSAlgo/ALGORITHM/base_chair.h"
#include "interface.h"

//! IChairContentEx interface GUID 
#define CHAIR_CONTENT_INTERFACE_EX	L"{823436DB-6F53-44F9-A45C-5D032E7FCD4C}"

//! ������� �������� �����
struct Range
{
	//! ���������� ������ �� �����������
	enum CONTENT
	{
		undefined,		//!<	�� ����������
		unknown,		//!<	�� ��������, �� ���-�� ����
		human,			//!<	�������
		clothes,		//!<	������/����
		empty			//!<	�����
	};
		
	//! ������ �������, �����
	int		m_nStart,
	//! ����� �������, �����
			m_nEnd;
	//! ���������� ������ � ���� ������
	CONTENT m_nContent;
	
	//! \param	s	��������� ����
	//! \param	e	�������� ����
	//! \param	c	����������
	Range( int s, int e, CONTENT c ) : m_nStart(s),m_nEnd(e),m_nContent(c){}
	Range( const Range& c ) : m_nStart(c.m_nStart),m_nEnd(c.m_nEnd),m_nContent(c.m_nContent){}
};

//! ������ �������� ��������
typedef std::list< Range >	RangeList_t;

//=====================================================================================//
//                                class IChairContentEx                                //
//=====================================================================================//
//! ��������� ��� ������ � ��������� ������ �� ��������� �����
//! ��������� �������� CreateInteface
struct IChairContentEx
{
	//! ������� � ������ �������� �������� ������ ������������ �������
	//! \param	RangeList	������ ��������
	//! \param	nStart		������ ������� 
	//! \param	nEnd		����� �������
	//! \param	cont		���������� �������
	//!	\return S_OK - success, all other - error
	virtual HRESULT			AddToRangeList		( RangeList_t& RangeList, int nStart, int nEnd, Range::CONTENT cont ) = 0;

	//! ������� �� ������ �������� ��������
	//! \param	RangeList	������ ��������
	//! \param	nMin		������ ������� 
	//! \param	nMax		����� �������
	//!	\return S_OK - success, all other - error
	virtual HRESULT			RemoveFromRangeList	( RangeList_t& RangeList, int nMin, int nMax) = 0;

	//! ������� ���������� ������ � Output
	//! \param	RangeList	������ ��������
	virtual void			DumpRangeList(const RangeList_t& RangeList) = 0 ;
	
	//! ���������� ������� ��������� ��� ����� ��������
	//! \param	RangeList	������ ��������
	//! \param	nTime		����� ����� 
	//! \return				���������� �����
	virtual Range::CONTENT	GetState( const RangeList_t& RangeList, int nTime ) = 0;
	
	//! �������� ������ �������� �� ������� ������ \sa IChairContentEx::Save
	//! \param	szFileName			��� ����� ��� ��������
	//! \param	Chairs				������ ����������� ������, ��� ������� ��������� 
	//! \param	RangeArr			�������� ������ �������� 
	//! \param	sVideoFile			�������� �������� ����������, � �������� ������������� ��������
	//! \param	sChairFile			�������� �������� ����� ��������, � �������� ������������� �������� 
	//! \param	sBackgroundFile		�������� �������� ����� ����, � �������� ������������� �������� ������
	//!	\return S_OK - success, all other - error
	virtual HRESULT			Load(	LPCWSTR szFileName, 
									const std::vector<BaseChair>&Chairs, 
									std::vector<RangeList_t>& RangeArr,
									std::wstring& sVideoFile,
									std::wstring& sChairFile, 
									std::wstring& sBackgroundFile ) = 0;

	//! ���������� ������ �������� �� ������� ������ \sa IChairContentEx::Load
	//! \param	sFileName			��� ����� ��� ����������
	//! \param	Chairs				������ ����������� ������, ��� ������� ��������� 
	//! \param	RangeArr			������ �������� 
	//! \param	sVideoFile			�������� ����������, � �������� ������������� ��������
	//! \param	sChairFile			�������� ����� ��������, � �������� ������������� �������� 
	//! \param	sBackgroundFile		�������� ����� ����, � �������� ������������� �������� ������
	//!	\return S_OK - success, all other - error
	virtual HRESULT			Save(	LPCWSTR sFileName, 
									const std::vector<BaseChair>&Chairs, 
									const std::vector<RangeList_t>& RangeArr,
									LPCWSTR sVideoFile,
									LPCWSTR sChairFile, 
									LPCWSTR sBackgroundFile
									) = 0;

	//! ���������� ���������
	virtual void 			Release() = 0;
};

#endif //__I_CHAIR_CONTENT_EX_H_INCLUDED_3421736227037312__
