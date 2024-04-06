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

//! содержимое кресла на изображении
//! \sa Range::CONTENT
enum ChairContents
{
  CHAIR_UNDEFINED = 0,	//!<	не определено
  CHAIR_EMPTY,			//!<	не известно, но что-то есть
  CHAIR_HUMAN,			//!<	человек
  CHAIR_CLOTHES,		//!<	одежда/вещь
  CHAIR_UNKNOWN			//!<	не известно, но что-то есть
};

//=====================================================================================//
//                                 class IChairContent                                 //
//=====================================================================================//

//! интерфейс для работы с разметкой кресла на обучающем видео, обертка над IChairContentEx
struct IChairContent
{
	//! загрука списка разметки из шаблона кресел
	//! \param	szFileName			имя файла для загрузки
	//! \param	Chairs				массив размеченных кресел, для которых загружаем их разметку на видео
	//!	\return S_OK - успех, иначе - ошибка
	virtual HRESULT	Load( LPCWSTR szFileName, const std::vector<BaseChair>& Chairs )	= 0;

	//! получить содержимое всех кресел для определенного кадра
	//! \param	nFrame			номер кадра на видео
	//! \param	Contents		словарь(карта) индексов кресел (берутся из Load) и их содержимого
	//!	\return S_OK - успех, иначе - ошибка
	virtual HRESULT GetContent( int nFrame, std::map<int, ChairContents>& Contents)	= 0;

	//! освободить интерфейс
	virtual void	Release()															= 0;
};

#endif //__I_CHAIR_CONTENT_H_INCLUDED_8325085304053871__
