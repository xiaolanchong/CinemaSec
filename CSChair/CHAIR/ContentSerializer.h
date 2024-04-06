// ContentSerializer.h: interface for the ContentSerializer class.
//
//////////////////////////////////////////////////////////////////////

//!	\author	Eugene Gorbachev
//! \date	15.02.2005
//	modified 2005-10-07

#if !defined(AFX_CONTENTSERIALIZER_H__FF6DBC54_4DAF_435C_BEA9_735EB6F5962E__INCLUDED_)
#define AFX_CONTENTSERIALIZER_H__FF6DBC54_4DAF_435C_BEA9_735EB6F5962E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "range.h"
#include "../xml/xmllite.h"

//! ����� ����� ���������� ��� �������� �������� �����
MACRO_EXCEPTION( ContentSerializerException,		CommonException )
//! ����������� ������
MACRO_EXCEPTION( ContentSerializerInvalidVersion, ContentSerializerException )
//! �������� ����
MACRO_EXCEPTION( ContentSerializerInvalidFile,	ContentSerializerException )
//! ������������ ��������
MACRO_EXCEPTION( ContentInvalidValue,			ContentSerializerException )

//! ������ ������� ��������
typedef std::vector< RangeList_t >		ChairTrainList_t;
//! ������ �������� ������
typedef std::vector<BaseChair>			BaseChairArr_t;

//! ����� ������������ ������� �������� �����
class ContentSerializer  
{
protected:
	//! xml-�������� ��������/����������
	XmlLite::XMLDocument	m_xmlDocument;

	ContentSerializer() {};
	virtual ~ContentSerializer() {};

	//! �������������� ����� ������+�������� ��� ����
	typedef std::pair<  Vec2f , RangeList_t  >		TrainListPairV2_t;
	//! ��� ���� ������
	typedef std::vector< TrainListPairV2_t >		TrainListV2_t;
	//! ����� ������ + ��� �������� ��� ���, ������ ������ 2
	typedef std::map< int, TrainListV2_t>			ZoneTrainListV2_t;
	//! ����� ������ + ��� �������� ��� ���, ������ ������ 1
	typedef std::map< int, RangeList_t >			TrainListV1_t;

	//! ����������� ������� ��������, ����������� �� ������� �������� � ������� ��� ������ ������� 1
	//! ��������� ������������� ������, �� ����� �� ��������������� �������� ��� ���
	//! \param	chSet		������ ������, ��� ������� �������� ��������
	//! \param	chTL		������ �������� � �������� ������
	//! \param	TrainList	��������������� ������ ��������
	static void ConvertV1( const BaseChairArr_t& chSet, const ChairTrainList_t& chTL,	TrainListV1_t& TrainList );

	//! ����������� ������� ��������, ����������� �� ������� �������� � �������, ��. ����
	//! ��������� ������������� ������, �� ����� �� ��������������� �������� ��� ���
	//! \param	chSet		������ ������, ��� ������� �������� ��������
	//! \param	TrainList	��������������� ������ ��������
	//! \param	chTL		������ �������� � �������� ������
	static void ConvertV1( const BaseChairArr_t& chSet, const TrainListV1_t& TrainList, ChairTrainList_t& chTL );

	//! ����������� ������� ��������, ����������� �� ������� �������� � �������
	//! ��������� ������������� ������, �� ����� �� ��������������� �������� ��� ���
	//! \param	chSet		������ ������, ��� ������� �������� ��������
	//! \param	chTL		������ ��������
	//! \param	chIL		��������������� ������ ��������, ����������� �� ������� � �������
	static void Convert( const BaseChairArr_t& chSet, const ChairTrainList_t& chTL,	ZoneTrainListV2_t& chIL );

	//! ����������� ������� ��������, ����������� �� ������� �������� � �������, ��. ����
	//! ��������� ������������� ������, �� ����� �� ��������������� �������� ��� ���
	//! \param	chSet		������ ������, ��� ������� �������� ��������
	//! \param	chTL		������ ��������
	//! \param	chIL		��������������� ������ ��������, ����������� �� ������� � �������
	static void Convert( const BaseChairArr_t& chSet, const ZoneTrainListV2_t& chIL,	ChairTrainList_t& chTL);

	//! ��������� ������ ��������
	//! \param	el		xml-�������, � ������� ���������� ����������
	//! \param	rl		����������� ������ ��������
	static void	SaveRangeList(	XmlLite::XMLElement& el,	const RangeList_t& rl );

	//! ��������� ������� ��������, ���������� �� SaveRangeList
	//! \param	el		xml-�������, � ������� ���������� ����������
	//! \param	rl		����������� �������
	static void SaveRange(		XmlLite::XMLElement& el,	const Range& r );

	//! �������� ������� ��������
	//! \param	el		xml-�������, �� �������� ���������� ��������
	//! \param	rl		�������� ������ ��������
	static void	LoadRangeList(	WORD wVersion, XmlLite::XMLElement& el,	RangeList_t& rl );

	//! �������� �������
	//! \param	el		xml-�������, �� �������� ���������� ��������
	//! \param	rl		�������� �������
	static void LoadRange(		WORD wVersion, XmlLite::XMLElement& el,	Range& r );

	//! ������� ��� �������, �������� ��� ����������� ������
	//! \param	cnt		���������� ������
	//! \return			��� �����������
	//! \exception		ContentInvalidValue
	static	LPCTSTR			ContentToName( Range::CONTENT cnt )THROW_HEADER (ContentInvalidValue);

	//! ������� ��� �������, �������� ���������� ������ �� �����, ��������� ContentToName
	//! \param	cnt		��� ���������� ������
	//! \return			����������
	//! \exception		ContentInvalidValue
	static	Range::CONTENT	NameToContent( CString cnt )THROW_HEADER (ContentInvalidValue);

};


//! ����� �������� �������� ������ 1
class ContentLoaderV1 : protected ContentSerializer
{
protected:
	//! ��������� ����������
	//! \param	wVersion		������ �������
	//! \param	el				�������, �� �������� ���������
	//!	\param	ctl				������ ��������
	static void	LoadContent(	WORD wVersion, XmlLite::XMLElement& el,	TrainListV1_t& ctl );
};

//! ����� �������� �������� ������ 2
class ContentLoaderV2 : protected ContentLoaderV1
{
protected:
	//! ��������� ����������
	//! \param	wVersion		������ �������
	//! \param	el				�������, �� �������� ���������
	//!	\param	ctl				������ ��������
	static void	LoadContent(	WORD wVersion, XmlLite::XMLElement& el,	ZoneTrainListV2_t& ctl );

	//! ��������� ���������� �� �������
	//! \param	wVersion		������ �������
	//! \param	el				�������, �� �������� ���������
	//!	\param	ctl				������ ��������
	static void	LoadZone(		WORD wVersion, XmlLite::XMLElement& el,	TrainListV2_t& ctl );
};

class ContentLoader : protected ContentLoaderV2
{
public:
	//! �������� ������ �������� �� ������� ������ 
	//! \param	szFileName			��� ����� ��� ��������
	//! \param	chset				������ ����������� ������, ��� ������� ��������� 
	//! \param	ctl					�������� ������ �������� 
	//! \param	VideoFile			�������� �������� ����������, � �������� ������������� ��������
	//! \param	ChairFile			�������� �������� ����� ��������, � �������� ������������� �������� 
	//! \param	BgFile				�������� �������� ����� ����, � �������� ������������� �������� ������

	void Load(	LPCWSTR szFileName,
				const BaseChairArr_t& chset,
				ChairTrainList_t& ctl,
				std::wstring& VideoFile,
				std::wstring& ChairFile,
				std::wstring& BgFile
				) 
				const THROW_HEADER (ContentSerializerException);
};

//! ������� ������ (������ 2) ���������� ��������
class ContentSaver : protected ContentSerializer
{
public:
	//! ���������� ������ �������� �� ������� ������ 
	//! \param	sFileName			��� ����� ��� ����������
	//! \param	chset				������ ����������� ������, ��� ������� ��������� 
	//! \param	ctl					������ �������� 
	//! \param	VideoFile			�������� ����������, � �������� ������������� ��������
	//! \param	ChairFile			�������� ����� ��������, � �������� ������������� �������� 
	//! \param	BgFile				�������� ����� ����, � �������� ������������� �������� ������
	void Save(	LPCWSTR szFileName, 
				const BaseChairArr_t& chset,
				const ChairTrainList_t& ctl,
				const std::wstring& VideoFile,
				const std::wstring& ChairFile,
				const std::wstring& BgFile
				) THROW_HEADER (ContentSerializerException);
};

#endif // !defined(AFX_CONTENTSERIALIZER_H__FF6DBC54_4DAF_435C_BEA9_735EB6F5962E__INCLUDED_)
