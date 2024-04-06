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

//! общий класс исключения при загрузки разметки видео
MACRO_EXCEPTION( ContentSerializerException,		CommonException )
//! неизвестная версия
MACRO_EXCEPTION( ContentSerializerInvalidVersion, ContentSerializerException )
//! неверный файл
MACRO_EXCEPTION( ContentSerializerInvalidFile,	ContentSerializerException )
//! неправильное значение
MACRO_EXCEPTION( ContentInvalidValue,			ContentSerializerException )

//! массив списков разметки
typedef std::vector< RangeList_t >		ChairTrainList_t;
//! массив разметки кресел
typedef std::vector<BaseChair>			BaseChairArr_t;

//! класс сериализации списков разметки видео
class ContentSerializer  
{
protected:
	//! xml-документ открытия/сохранения
	XmlLite::XMLDocument	m_xmlDocument;

	ContentSerializer() {};
	virtual ~ContentSerializer() {};

	//! геометричексий центр кресла+разметка для него
	typedef std::pair<  Vec2f , RangeList_t  >		TrainListPairV2_t;
	//! для всех кресел
	typedef std::vector< TrainListPairV2_t >		TrainListV2_t;
	//! номер камеры + вся разметка для нее, формат версии 2
	typedef std::map< int, TrainListV2_t>			ZoneTrainListV2_t;
	//! номер камеры + вся разметка для нее, формат версии 1
	typedef std::map< int, RangeList_t >			TrainListV1_t;

	//! сопоставить креслам разметку, привязывает по центрам разметку к креслам для версии формата 1
	//! позволяет редактировать кресла, не теряя по идентификаторам разметку для них
	//! \param	chSet		массив кресел, для которых передана разметка
	//! \param	chTL		массив разметки с центрами кресел
	//! \param	TrainList	преобразованный массив разметок
	static void ConvertV1( const BaseChairArr_t& chSet, const ChairTrainList_t& chTL,	TrainListV1_t& TrainList );

	//! сопоставить креслам разметку, привязывает по центрам разметку к креслам, см. выше
	//! позволяет редактировать кресла, не теряя по идентификаторам разметку для них
	//! \param	chSet		массив кресел, для которых передана разметка
	//! \param	TrainList	преобразованный массив разметок
	//! \param	chTL		массив разметки с центрами кресел
	static void ConvertV1( const BaseChairArr_t& chSet, const TrainListV1_t& TrainList, ChairTrainList_t& chTL );

	//! сопоставить креслам разметку, привязывает по центрам разметку к креслам
	//! позволяет редактировать кресла, не теряя по идентификаторам разметку для них
	//! \param	chSet		массив кресел, для которых передана разметка
	//! \param	chTL		массив разметки
	//! \param	chIL		преобразованный массив разметок, разнесенный по камерам и креслам
	static void Convert( const BaseChairArr_t& chSet, const ChairTrainList_t& chTL,	ZoneTrainListV2_t& chIL );

	//! сопоставить креслам разметку, привязывает по центрам разметку к креслам, см. выше
	//! позволяет редактировать кресла, не теряя по идентификаторам разметку для них
	//! \param	chSet		массив кресел, для которых передана разметка
	//! \param	chTL		массив разметки
	//! \param	chIL		преобразованный массив разметок, разнесенный по камерам и креслам
	static void Convert( const BaseChairArr_t& chSet, const ZoneTrainListV2_t& chIL,	ChairTrainList_t& chTL);

	//! сохранить массив отрезков
	//! \param	el		xml-элемент, в который происходит сохранение
	//! \param	rl		сохраняемый массив отрезков
	static void	SaveRangeList(	XmlLite::XMLElement& el,	const RangeList_t& rl );

	//! сохранить отрезок разметки, вызывается из SaveRangeList
	//! \param	el		xml-элемент, в который происходит сохранение
	//! \param	rl		сохраняемый отрезок
	static void SaveRange(		XmlLite::XMLElement& el,	const Range& r );

	//! загрузка массива отрезков
	//! \param	el		xml-элемент, из которого происходит загрузка
	//! \param	rl		получает массив отрезков
	static void	LoadRangeList(	WORD wVersion, XmlLite::XMLElement& el,	RangeList_t& rl );

	//! загрузка отрезка
	//! \param	el		xml-элемент, из которого происходит загрузка
	//! \param	rl		получает отрезок
	static void LoadRange(		WORD wVersion, XmlLite::XMLElement& el,	Range& r );

	//! функция для отладки, получить имя содержимого кресла
	//! \param	cnt		содержимое кресла
	//! \return			имя содержимого
	//! \exception		ContentInvalidValue
	static	LPCTSTR			ContentToName( Range::CONTENT cnt )THROW_HEADER (ContentInvalidValue);

	//! функция для отладки, получить содержимое кресла по имени, созданное ContentToName
	//! \param	cnt		имя содержимое кресла
	//! \return			содержимое
	//! \exception		ContentInvalidValue
	static	Range::CONTENT	NameToContent( CString cnt )THROW_HEADER (ContentInvalidValue);

};


//! класс загрузки разметки версии 1
class ContentLoaderV1 : protected ContentSerializer
{
protected:
	//! загрузить содержимое
	//! \param	wVersion		версия формата
	//! \param	el				элемент, из которого загружаем
	//!	\param	ctl				список разметки
	static void	LoadContent(	WORD wVersion, XmlLite::XMLElement& el,	TrainListV1_t& ctl );
};

//! класс загрузки разметки версии 2
class ContentLoaderV2 : protected ContentLoaderV1
{
protected:
	//! загрузить содержимое
	//! \param	wVersion		версия формата
	//! \param	el				элемент, из которого загружаем
	//!	\param	ctl				список разметки
	static void	LoadContent(	WORD wVersion, XmlLite::XMLElement& el,	ZoneTrainListV2_t& ctl );

	//! загрузить содержимое по камерам
	//! \param	wVersion		версия формата
	//! \param	el				элемент, из которого загружаем
	//!	\param	ctl				список разметки
	static void	LoadZone(		WORD wVersion, XmlLite::XMLElement& el,	TrainListV2_t& ctl );
};

class ContentLoader : protected ContentLoaderV2
{
public:
	//! загрузка списка разметки из шаблона кресел 
	//! \param	szFileName			имя файла для загрузки
	//! \param	chset				массив размеченных кресел, для которых загружаем 
	//! \param	ctl					получает список разметки 
	//! \param	VideoFile			получает название видеофайла, с которого производилась разметка
	//! \param	ChairFile			получает название файла разметки, с которого производилась разметка 
	//! \param	BgFile				получает название файла фона, с которого производилась разметка кресел

	void Load(	LPCWSTR szFileName,
				const BaseChairArr_t& chset,
				ChairTrainList_t& ctl,
				std::wstring& VideoFile,
				std::wstring& ChairFile,
				std::wstring& BgFile
				) 
				const THROW_HEADER (ContentSerializerException);
};

//! текущий формат (версия 2) сохранения разметки
class ContentSaver : protected ContentSerializer
{
public:
	//! сохранение списка разметки из шаблона кресел 
	//! \param	sFileName			имя файла для сохранения
	//! \param	chset				массив размеченных кресел, для которых загружаем 
	//! \param	ctl					список разметки 
	//! \param	VideoFile			название видеофайла, с которого производилась разметка
	//! \param	ChairFile			название файла разметки, с которого производилась разметка 
	//! \param	BgFile				название файла фона, с которого производилась разметка кресел
	void Save(	LPCWSTR szFileName, 
				const BaseChairArr_t& chset,
				const ChairTrainList_t& ctl,
				const std::wstring& VideoFile,
				const std::wstring& ChairFile,
				const std::wstring& BgFile
				) THROW_HEADER (ContentSerializerException);
};

#endif // !defined(AFX_CONTENTSERIALIZER_H__FF6DBC54_4DAF_435C_BEA9_735EB6F5962E__INCLUDED_)
