//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Классы для загрузки обводки без конкретного алгоритма
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

//! класс загрузки файла кресел
class ChairSaver 
{
	//! документ, в который производится сохранение
	XmlLite::XMLDocument	m_xmlDocument;

	//! сохранение в документ
	//! \param	Chairs	сохраняемые кресла
	//! \param	PosMap	позиции камер по индексам
	void	SaveToDocument(const BaseChairArr_t& Chairs, const IChairSerializer::PositionMap_t& PosMap);
public:
	//! сохранение в файл
	//! \param	szFileName	полное имя файла
	//! \param	Chairs		сохраняемые кресла
	//! \param	PosMap		позиции камер по индексам
	void	Save( LPCTSTR szFileName,					const BaseChairArr_t& Chairs, const IChairSerializer::PositionMap_t& PosMap   ) ;

	//! сохранение в строки (AFAIK не используется)
	//! \param	strDocument	строковое представление файла
	//! \param	Chairs		сохраняемые кресла
	//! \param	PosMap		позиции камер по индексам
	void	SaveToString( std::tstring& strDocument,	const BaseChairArr_t& Chairs, const IChairSerializer::PositionMap_t& PosMap ) ;

	//! сохранение в двоичный массив
	//! \param	Data		содержимое файла кресел
	//! \param	Chairs		сохраняемые кресла
	//! \param	PosMap		позиции камер по индексам
	void	SaveToBinArray( std::vector<BYTE>& Data,	const BaseChairArr_t& Chairs, const IChairSerializer::PositionMap_t& PosMap );
};

class ChairLoader 
{
	//! документ, из которого производится загрузка
	XmlLite::XMLDocument	m_xmlDocument;

	//! загрузка из документа
	//! \param	Chairs	загружаемые кресла
	//! \param	PosMap	позиции камер по индексам
	void	LoadFromDocument(BaseChairArr_t& Chairs, IChairSerializer::PositionMap_t& PosMap);

	//! проверка на непротиворечивость данных
	//! \param	Chairs	проверяемый массив
	void	Validate( BaseChairArr_t& Chairs ) const;
public:
	//! загрузка из файла \sa  ChairSaver::Save
	//! \param	szFileName	полное имя файла
	//! \param	Chairs		кресла
	//! \param	PosMap		позиции камер по индексам
	void	Load( LPCTSTR szFileName,							BaseChairArr_t& Chairs, IChairSerializer::PositionMap_t& PosMap  ) ;

	//! загрузка из строки (AFAIK не используется)
	//! \param	szDocument	строковое представление файла
	//! \param	Chairs		кресла
	//! \param	PosMap		позиции камер по индексам
	void	LoadFromString( LPCTSTR szDocument,					BaseChairArr_t& Chairs, IChairSerializer::PositionMap_t& PosMap   ) ;

	//! загрузка из двоичного массива
	//! \param	Data		содержимое файла кресел
	//! \param	nSize		размер двоичных данных в байтах
	//! \param	Chairs		кресла
	//! \param	PosMap		позиции камер по индексам
	void	LoadFromBinArray( const void* pData, size_t nSize,	BaseChairArr_t& Chairs, IChairSerializer::PositionMap_t& PosMap );
};

#endif // _CHAIR_COMMON_CLASS_5932257169132003_