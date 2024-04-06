//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                               Copyright� ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	������ ��� �������� ������� ��� ����������� ���������
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 15.07.2005
//                                                                                      //
//======================================================================================//
#ifndef _CHAIR_COMMON_CLASS_5932257169132003_
#define _CHAIR_COMMON_CLASS_5932257169132003_

#include "../common/Exception.h"
#include "../common/tstring.h"
#include "ChairSerializerImpl.h"
#include "../chair_int.h"

//! ����� �������� ����� ������
class ChairSaver 
{
	//! ��������, � ������� ������������ ����������
	XmlLite::XMLDocument	m_xmlDocument;

	//! ���������� � ��������
	//! \param	Chairs	����������� ������
	//! \param	PosMap	������� ����� �� ��������
	void	SaveToDocument(const BaseChairArr_t& Chairs, const IChairSerializer::PositionMap_t& PosMap);
public:
	//! ���������� � ����
	//! \param	szFileName	������ ��� �����
	//! \param	Chairs		����������� ������
	//! \param	PosMap		������� ����� �� ��������
	void	Save( LPCTSTR szFileName,					const BaseChairArr_t& Chairs, const IChairSerializer::PositionMap_t& PosMap   ) ;

	//! ���������� � ������ (AFAIK �� ������������)
	//! \param	strDocument	��������� ������������� �����
	//! \param	Chairs		����������� ������
	//! \param	PosMap		������� ����� �� ��������
	void	SaveToString( std::tstring& strDocument,	const BaseChairArr_t& Chairs, const IChairSerializer::PositionMap_t& PosMap ) ;

	//! ���������� � �������� ������
	//! \param	Data		���������� ����� ������
	//! \param	Chairs		����������� ������
	//! \param	PosMap		������� ����� �� ��������
	void	SaveToBinArray( std::vector<BYTE>& Data,	const BaseChairArr_t& Chairs, const IChairSerializer::PositionMap_t& PosMap );
};

class ChairLoader 
{
	//! ��������, �� �������� ������������ ��������
	XmlLite::XMLDocument	m_xmlDocument;

	//! �������� �� ���������
	//! \param	Chairs	����������� ������
	//! \param	PosMap	������� ����� �� ��������
	void	LoadFromDocument(BaseChairArr_t& Chairs, IChairSerializer::PositionMap_t& PosMap);

	//! �������� �� ������������������ ������
	//! \param	Chairs	����������� ������
	void	Validate( BaseChairArr_t& Chairs ) const;
public:
	//! �������� �� ����� \sa  ChairSaver::Save
	//! \param	szFileName	������ ��� �����
	//! \param	Chairs		������
	//! \param	PosMap		������� ����� �� ��������
	void	Load( LPCTSTR szFileName,							BaseChairArr_t& Chairs, IChairSerializer::PositionMap_t& PosMap  ) ;

	//! �������� �� ������ (AFAIK �� ������������)
	//! \param	szDocument	��������� ������������� �����
	//! \param	Chairs		������
	//! \param	PosMap		������� ����� �� ��������
	void	LoadFromString( LPCTSTR szDocument,					BaseChairArr_t& Chairs, IChairSerializer::PositionMap_t& PosMap   ) ;

	//! �������� �� ��������� �������
	//! \param	Data		���������� ����� ������
	//! \param	nSize		������ �������� ������ � ������
	//! \param	Chairs		������
	//! \param	PosMap		������� ����� �� ��������
	void	LoadFromBinArray( const void* pData, size_t nSize,	BaseChairArr_t& Chairs, IChairSerializer::PositionMap_t& PosMap );
};

#endif // _CHAIR_COMMON_CLASS_5932257169132003_