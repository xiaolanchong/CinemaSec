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
//! интерфейс сериализации кресел
struct IChairSerializer
{
	//! карта ID камеры(зоны) - позиция камеры в 2х мерном массиве, где (0, 0) - левый верхний
	typedef std::map<int, std::pair<int,int> >	PositionMap_t;

	//! загрузить файл кресел
	//! \param	szFileName	полное имя файла кресел
	//! \param	Chairs		массив кресел
	//! \param	PosMap		карта расположения камер
	//!	\return S_OK - успех, иначе - ошибка
	virtual HRESULT LoadGaugeHallFromFile		(LPCWSTR szFileName,				std::vector<BaseChair>&	Chairs, PositionMap_t& PosMap  ) = 0;

	//! загрузить  кресла из строкового представления файла, используется при работе с БД
	//! \param	szSource	содержимое файла кресел
	//! \param	Chairs		массив кресел
	//! \param	PosMap		карта расположения камер
	//!	\return S_OK - успех, иначе - ошибка
	virtual HRESULT LoadGaugeHallFromString		(LPCWSTR szSource,					std::vector<BaseChair>&	Chairs, PositionMap_t& PosMap  ) = 0;

	//! загрузить  кресла из двоичного представления файла, используется при работе с БД
	//! \param	pData		содержимое файла кресел
	//! \param	nSize		размер pData в байтах
	//! \param	Chairs		массив кресел
	//! \param	PosMap		карта расположения камер
	//!	\return S_OK - успех, иначе - ошибка
	virtual HRESULT LoadGaugeHallFromBinaryData	(const void* pData, size_t nSize,	std::vector<BaseChair>&	Chairs,	PositionMap_t& PosMap  ) = 0;

	//! сохранить файл кресел \sa IChairSerializer::LoadGaugeHallFromFile
	//! \param	szFileName	полное имя файла кресел
	//! \param	Chairs		массив кресел
	//! \param	PosMap		карта расположения камер
	//!	\return S_OK - успех, иначе - ошибка
	virtual HRESULT SaveGaugeHallToFile			(LPCWSTR szFileName,		const std::vector<BaseChair>&	Chairs, const PositionMap_t& PosMap  ) = 0;

	//! сохранить  кресла в строкового представления файла \sa IChairSerializer::LoadGaugeHallFromString
	//! \param	sSource		содержимое файла кресел
	//! \param	Chairs		массив кресел
	//! \param	PosMap		карта расположения камер
	//!	\return S_OK - успех, иначе - ошибка
	//!	\return S_OK - успех, иначе - ошибка
	virtual HRESULT SaveGaugeHallToString		(std::wstring& sSource,		const std::vector<BaseChair>&	Chairs, const PositionMap_t& PosMap  ) = 0;

	//! сохранить  кресла в двоичное представление файла \sa IChairSerializer::LoadGaugeHallFromBinaryData
	//! \param	Data		содержимое файла кресел
	//! \param	Chairs		массив кресел
	//! \param	PosMap		карта расположения камер
	//!	\return S_OK - успех, иначе - ошибка
	virtual HRESULT SaveGaugeHallToBinaryData	(std::vector<BYTE>& Data,	const std::vector<BaseChair>&	Chairs, const PositionMap_t& PosMap  ) = 0;

	//! освободить интерфейс
	virtual void	Release() = 0;
};

#endif //__I_CHAIR_SERIALIZER_H_INCLUDED_5275078104573630__