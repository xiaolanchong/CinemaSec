//=====================================================================================//
//                                                                                     //
//                                      CinemaSec                                      //
//                                                                                     //
//                                 Copyright by ElVEES 2005                            //
//                                                                                     //
//   -------------------------------------------------------------------------------   //
//   Description:                                                                      //
//                                                                                     //
//   -------------------------------------------------------------------------------   //
//   Author:  Eugene Gorbachev                                                         //
//   Date:   31.01.2005                                                                //
//                                                                                     //
//=====================================================================================//
#ifndef __I_CHAIR_SERIALIZER_H_INCLUDED_5275078104573630__
#define __I_CHAIR_SERIALIZER_H_INCLUDED_5275078104573630__

//! IChairSerializer GUID, use it in CreateInterface
#define CHAIR_SERIALIZER_INTERFACE_0 L"{9B8D4F76-8CB9-4B5B-B730-034115092881}"

#include <vector>
#include "../csalgo/algorithm/base_chair.h"
//=====================================================================================//
//                               struct IChairSerializer                                //
//=====================================================================================//
//! ��������� ������������ ������
struct IChairSerializer
{
	//! ����� ID ������(����) - ������� ������ � 2� ������ �������, ��� (0, 0) - ����� �������
	typedef std::map<int, std::pair<int,int> >	PositionMap_t;

	//! ��������� ���� ������
	//! \param	szFileName	������ ��� ����� ������
	//! \param	Chairs		������ ������
	//! \param	PosMap		����� ������������ �����
	//!	\return S_OK - �����, ����� - ������
	virtual HRESULT LoadGaugeHallFromFile		(LPCWSTR szFileName,				std::vector<BaseChair>&	Chairs, PositionMap_t& PosMap  ) = 0;

	//! ���������  ������ �� ���������� ������������� �����, ������������ ��� ������ � ��
	//! \param	szSource	���������� ����� ������
	//! \param	Chairs		������ ������
	//! \param	PosMap		����� ������������ �����
	//!	\return S_OK - �����, ����� - ������
	virtual HRESULT LoadGaugeHallFromString		(LPCWSTR szSource,					std::vector<BaseChair>&	Chairs, PositionMap_t& PosMap  ) = 0;

	//! ���������  ������ �� ��������� ������������� �����, ������������ ��� ������ � ��
	//! \param	pData		���������� ����� ������
	//! \param	nSize		������ pData � ������
	//! \param	Chairs		������ ������
	//! \param	PosMap		����� ������������ �����
	//!	\return S_OK - �����, ����� - ������
	virtual HRESULT LoadGaugeHallFromBinaryData	(const void* pData, size_t nSize,	std::vector<BaseChair>&	Chairs,	PositionMap_t& PosMap  ) = 0;

	//! ��������� ���� ������ \sa IChairSerializer::LoadGaugeHallFromFile
	//! \param	szFileName	������ ��� ����� ������
	//! \param	Chairs		������ ������
	//! \param	PosMap		����� ������������ �����
	//!	\return S_OK - �����, ����� - ������
	virtual HRESULT SaveGaugeHallToFile			(LPCWSTR szFileName,		const std::vector<BaseChair>&	Chairs, const PositionMap_t& PosMap  ) = 0;

	//! ���������  ������ � ���������� ������������� ����� \sa IChairSerializer::LoadGaugeHallFromString
	//! \param	sSource		���������� ����� ������
	//! \param	Chairs		������ ������
	//! \param	PosMap		����� ������������ �����
	//!	\return S_OK - �����, ����� - ������
	//!	\return S_OK - �����, ����� - ������
	virtual HRESULT SaveGaugeHallToString		(std::wstring& sSource,		const std::vector<BaseChair>&	Chairs, const PositionMap_t& PosMap  ) = 0;

	//! ���������  ������ � �������� ������������� ����� \sa IChairSerializer::LoadGaugeHallFromBinaryData
	//! \param	Data		���������� ����� ������
	//! \param	Chairs		������ ������
	//! \param	PosMap		����� ������������ �����
	//!	\return S_OK - �����, ����� - ������
	virtual HRESULT SaveGaugeHallToBinaryData	(std::vector<BYTE>& Data,	const std::vector<BaseChair>&	Chairs, const PositionMap_t& PosMap  ) = 0;

	//! ���������� ���������
	virtual void	Release() = 0;
};

#endif //__I_CHAIR_SERIALIZER_H_INCLUDED_5275078104573630__