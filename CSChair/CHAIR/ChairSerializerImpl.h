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

//! сериализация разметки кресел
class ChairSerializer  
{
protected:	

	//! преобразование структуры кресла после загрузки с диска
	//! \param	ChairOnDisk		исходный массив
	//! \param	ChairList		преобразованный
	static void ConvertToBaseChair	( const std::vector<ChairDisk_t>& ChairOnDisk,	BaseChairArr_t& ChairList) ;

	//! преобразование структуры кресла для сохранения на диск, замена указателей на индексы
	//! \param	ChairList		исходный массив
	//! \param	ChairOnDisk		преобразованный
	static void ConvertFromBaseChair( const BaseChairArr_t& ChairList,				std::vector<ChairDisk_t>& ChairOnDisk) ;

	//! проектировалась в расчета учета связей кресел, не используется
	static void AddLinkListToBaseChair	( const ChairLinkDiskArray_t& LinkOnDisk, BaseChairArr_t& ChairList) ;

	//! проектировалась в расчета учета связей кресел, не используется
	static void CreateLinkListFromBaseChair		( const BaseChairArr_t& ChairList,		 ChairLinkDiskArray_t& LinkOnDisk) ;

	ChairSerializer();
	virtual ~ChairSerializer();

	//! сохранение структуры кресла в элемент
	//! \param	elChair		элемент документа
	//! \param	ch			структура кресла
	static void	SaveChair(XmlLite::XMLElement& elChair, const ChairDisk_t& ch);

	//! загрузка структуры кресла из элемента
	//! \param	elChair		элемент документа
	//! \param	ch			структура кресла
	static void	LoadChair(XmlLite::XMLElement& elChair, ChairDisk_t& ch);

	//! сохранение связи кресла в элемент
	//! \param	elLink		элемент документа
	//! \param	cld			структура связи
	static void SaveLink(XmlLite::XMLElement& elLink, const ChairLinkDisk& cld  );

	//! загрузка связи кресла в элемент
	//! \param	elLink		элемент документа
	//! \param	cld			структура связи
	static void LoadLink(XmlLite::XMLElement& elLink, ChairLinkDisk& cld  );

	//! сохранение кривой обводки кресла в элемент
	//! \param	elLink		элемент документа
	//! \param	ch			массив 2мерных точек
	static void	SaveArray(XmlLite::XMLElement& elArray, const Vec2fArr& ch);

	//! загрузка кривой обводки кресла из элемента
	//! \param	elLink		элемент документа
	//! \param	ch			массив 2мерных точек
	static void	LoadArray(XmlLite::XMLElement& elArray, Vec2fArr& ch);

	//! сохранение всех кресел для одной камеры
	//! \param	elLink		элемент документа
	//! \param	ch			все кресла для одной камеры
	static void	SaveChairForCamera(XmlLite::XMLElement& elChair, const ChairDisk_t& ch);

	//! загрузка всех кресел для одной камеры
	//! \param	elLink		элемент документа
	//! \param	ch			все кресла для одной камеры
	static void	LoadChairForCamera(XmlLite::XMLElement& elChair, ChairDisk_t& ch);

	//! сохранение кривой обводки кресла из элемента в сжатом формате
	//! \param	elLink		элемент документа
	//! \param	ch			массив 2мерных точек
	static void	SaveArrayWithCompression(XmlLite::XMLElement& elArray, const Vec2fArr& ch);

	//! загрузка кривой обводки кресла из элемента в сжатом формате
	//! \param	elLink		элемент документа
	//! \param	ch			массив 2мерных точек
	static void	LoadArrayWithCompression(XmlLite::XMLElement& elArray, Vec2fArr& ch);
};

//////////////////

//! интерфейс загрузчика кресел
struct ChairLoaderVersionAbs : ChairSerializer
{
	//! загрузка из xml документа
	//! \param	xmlDoc	документ
	//! \param	Chairs	кресла
	//! \param PosMap	карта ID камеры-позиция на плоскости	
	virtual void	LoadFromDocument( XmlLite::XMLDocument& xmlDoc, BaseChairArr_t& Chairs, IChairSerializer::PositionMap_t& PosMap ) = 0;

	//! получить загрузчик в зависимости от версии формата
	//! \param	xmlDoc		xml-документ
	//! \return указатель	на интерфейс
	static std::auto_ptr<ChairLoaderVersionAbs>	GetLoader( XmlLite::XMLDocument& xmlDoc );
};

//! класс сохранения кресел в формате 1 (для совместимости)
class ChairSaverVersion1 : public ChairSerializer
{
public:
	//! сохранение в xml документ
	//! \param	xmlDoc	документ
	//! \param	Chairs	массив кресел
	void	SaveToDocument( XmlLite::XMLDocument& xmlDoc,
							const BaseChairArr_t& Chairs) ;
};

//! класс сохранения кресел в формате 2 (текущий)
class ChairSaverVersion2 : public ChairSerializer
{
public:
	//! сохранение в xml документ
	//! \param	xmlDoc	документ
	//! \param	Chairs	массив кресел
	//! \param	PosMap	карта ID камеры-позиция на плоскости
	void	SaveToDocument( XmlLite::XMLDocument& xmlDoc,
							const BaseChairArr_t& Chairs, 
							const IChairSerializer::PositionMap_t& PosMap ) ;
};

//! класс загрузки кресел в формате 1 (для совместимости)
struct ChairLoaderVersion1 : public ChairLoaderVersionAbs
{
	//! \param	xmlDoc	документ
	//! \param	Chairs	массив кресел
	//! \param	PosMap	карта ID камеры-позиция на плоскости (не используется в данном формате)
	virtual void	LoadFromDocument( XmlLite::XMLDocument& xmlDoc, BaseChairArr_t& Chairs, IChairSerializer::PositionMap_t& PosMap ) ;
};

//! класс загрузки кресел в формате 2 (основной и текущий)
struct ChairLoaderVersion2 : public ChairLoaderVersionAbs
{
	//! \param	xmlDoc	документ
	//! \param	Chairs	массив кресел
	//! \param	PosMap	карта ID камеры-позиция на плоскости
	virtual void	LoadFromDocument( XmlLite::XMLDocument& xmlDoc, BaseChairArr_t& Chairs, IChairSerializer::PositionMap_t& PosMap );
};

/////////////

#endif // !defined(AFX_CHAIRSERIALIZER_H__12EE411E_DB5B_42C8_8154_79A4D905F6CA__INCLUDED_)
