// ChairSerializer.h: interface for the ChairSerializer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHAIRSERIALIZER_H__12EE411E_DB5B_42C8_8154_79A4D905F6CA__INCLUDED_)
#define AFX_CHAIRSERIALIZER_H__12EE411E_DB5B_42C8_8154_79A4D905F6CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <mmsystem.h>

#include "../xml/xmllite.h"
#include "ChairLink.h"
#include "../../CSUtility/csutility.h"
#include "ChairDisk.h"
#include "../chair_int.h"

MACRO_EXCEPTION( ChairSerializerException,		CommonException )
MACRO_EXCEPTION( ChairSerializerInvalidVersion, ChairSerializerException )
MACRO_EXCEPTION( ChairSerializerInvalidFile,	ChairSerializerException )

typedef std::vector<BaseChair> BaseChairArr_t;

//! ������������ �������� ������
class ChairSerializer  
{
protected:	

	//! �������������� ��������� ������ ����� �������� � �����
	//! \param	ChairOnDisk		�������� ������
	//! \param	ChairList		���������������
	static void ConvertToBaseChair	( const std::vector<ChairDisk_t>& ChairOnDisk,	BaseChairArr_t& ChairList) ;

	//! �������������� ��������� ������ ��� ���������� �� ����, ������ ���������� �� �������
	//! \param	ChairList		�������� ������
	//! \param	ChairOnDisk		���������������
	static void ConvertFromBaseChair( const BaseChairArr_t& ChairList,				std::vector<ChairDisk_t>& ChairOnDisk) ;

	//! ��������������� � ������� ����� ������ ������, �� ������������
	static void AddLinkListToBaseChair	( const ChairLinkDiskArray_t& LinkOnDisk, BaseChairArr_t& ChairList) ;

	//! ��������������� � ������� ����� ������ ������, �� ������������
	static void CreateLinkListFromBaseChair		( const BaseChairArr_t& ChairList,		 ChairLinkDiskArray_t& LinkOnDisk) ;

	ChairSerializer();
	virtual ~ChairSerializer();

	//! ���������� ��������� ������ � �������
	//! \param	elChair		������� ���������
	//! \param	ch			��������� ������
	static void	SaveChair(XmlLite::XMLElement& elChair, const ChairDisk_t& ch);

	//! �������� ��������� ������ �� ��������
	//! \param	elChair		������� ���������
	//! \param	ch			��������� ������
	static void	LoadChair(XmlLite::XMLElement& elChair, ChairDisk_t& ch);

	//! ���������� ����� ������ � �������
	//! \param	elLink		������� ���������
	//! \param	cld			��������� �����
	static void SaveLink(XmlLite::XMLElement& elLink, const ChairLinkDisk& cld  );

	//! �������� ����� ������ � �������
	//! \param	elLink		������� ���������
	//! \param	cld			��������� �����
	static void LoadLink(XmlLite::XMLElement& elLink, ChairLinkDisk& cld  );

	//! ���������� ������ ������� ������ � �������
	//! \param	elLink		������� ���������
	//! \param	ch			������ 2������ �����
	static void	SaveArray(XmlLite::XMLElement& elArray, const Vec2fArr& ch);

	//! �������� ������ ������� ������ �� ��������
	//! \param	elLink		������� ���������
	//! \param	ch			������ 2������ �����
	static void	LoadArray(XmlLite::XMLElement& elArray, Vec2fArr& ch);

	//! ���������� ���� ������ ��� ����� ������
	//! \param	elLink		������� ���������
	//! \param	ch			��� ������ ��� ����� ������
	static void	SaveChairForCamera(XmlLite::XMLElement& elChair, const ChairDisk_t& ch);

	//! �������� ���� ������ ��� ����� ������
	//! \param	elLink		������� ���������
	//! \param	ch			��� ������ ��� ����� ������
	static void	LoadChairForCamera(XmlLite::XMLElement& elChair, ChairDisk_t& ch);

	//! ���������� ������ ������� ������ �� �������� � ������ �������
	//! \param	elLink		������� ���������
	//! \param	ch			������ 2������ �����
	static void	SaveArrayWithCompression(XmlLite::XMLElement& elArray, const Vec2fArr& ch);

	//! �������� ������ ������� ������ �� �������� � ������ �������
	//! \param	elLink		������� ���������
	//! \param	ch			������ 2������ �����
	static void	LoadArrayWithCompression(XmlLite::XMLElement& elArray, Vec2fArr& ch);
};

//////////////////

//! ��������� ���������� ������
struct ChairLoaderVersionAbs : ChairSerializer
{
	//! �������� �� xml ���������
	//! \param	xmlDoc	��������
	//! \param	Chairs	������
	//! \param PosMap	����� ID ������-������� �� ���������	
	virtual void	LoadFromDocument( XmlLite::XMLDocument& xmlDoc, BaseChairArr_t& Chairs, IChairSerializer::PositionMap_t& PosMap ) = 0;

	//! �������� ��������� � ����������� �� ������ �������
	//! \param	xmlDoc		xml-��������
	//! \return ���������	�� ���������
	static std::auto_ptr<ChairLoaderVersionAbs>	GetLoader( XmlLite::XMLDocument& xmlDoc );
};

//! ����� ���������� ������ � ������� 1 (��� �������������)
class ChairSaverVersion1 : public ChairSerializer
{
public:
	//! ���������� � xml ��������
	//! \param	xmlDoc	��������
	//! \param	Chairs	������ ������
	void	SaveToDocument( XmlLite::XMLDocument& xmlDoc,
							const BaseChairArr_t& Chairs) ;
};

//! ����� ���������� ������ � ������� 2 (�������)
class ChairSaverVersion2 : public ChairSerializer
{
public:
	//! ���������� � xml ��������
	//! \param	xmlDoc	��������
	//! \param	Chairs	������ ������
	//! \param	PosMap	����� ID ������-������� �� ���������
	void	SaveToDocument( XmlLite::XMLDocument& xmlDoc,
							const BaseChairArr_t& Chairs, 
							const IChairSerializer::PositionMap_t& PosMap ) ;
};

//! ����� �������� ������ � ������� 1 (��� �������������)
struct ChairLoaderVersion1 : public ChairLoaderVersionAbs
{
	//! \param	xmlDoc	��������
	//! \param	Chairs	������ ������
	//! \param	PosMap	����� ID ������-������� �� ��������� (�� ������������ � ������ �������)
	virtual void	LoadFromDocument( XmlLite::XMLDocument& xmlDoc, BaseChairArr_t& Chairs, IChairSerializer::PositionMap_t& PosMap ) ;
};

//! ����� �������� ������ � ������� 2 (�������� � �������)
struct ChairLoaderVersion2 : public ChairLoaderVersionAbs
{
	//! \param	xmlDoc	��������
	//! \param	Chairs	������ ������
	//! \param	PosMap	����� ID ������-������� �� ���������
	virtual void	LoadFromDocument( XmlLite::XMLDocument& xmlDoc, BaseChairArr_t& Chairs, IChairSerializer::PositionMap_t& PosMap );
};

/////////////

#endif // !defined(AFX_CHAIRSERIALIZER_H__12EE411E_DB5B_42C8_8154_79A4D905F6CA__INCLUDED_)
