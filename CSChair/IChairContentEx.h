//=====================================================================================//
//                                                                                     //
//                                      CinemaSec                                      //
//                                                                                     //
//                              Copyright by ElVEES 2005                               //
//                                                                                     //
//   -------------------------------------------------------------------------------   //
//   Description: интерфейс для разметки видео 
//					Каждому креслу, для которого есть обводка, на каком-либо видео можно 
//					сопоставить временные интервалы, в соответствии с его состоянием(содержимым)
//					совокупность всех таких интервалов для всех кресел - разметка видео
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

//! отрезок разметки видео
struct Range
{
	//! содержимое кресла на изображении
	enum CONTENT
	{
		undefined,		//!<	не определено
		unknown,		//!<	не известно, но что-то есть
		human,			//!<	человек
		clothes,		//!<	одежда/вещь
		empty			//!<	пусто
	};
		
	//! начало отрезка, кадры
	int		m_nStart,
	//! конец отрезка, кадры
			m_nEnd;
	//! содержимое кресла в этот период
	CONTENT m_nContent;
	
	//! \param	s	начальный кадр
	//! \param	e	конечный кадр
	//! \param	c	содержимое
	Range( int s, int e, CONTENT c ) : m_nStart(s),m_nEnd(e),m_nContent(c){}
	Range( const Range& c ) : m_nStart(c.m_nStart),m_nEnd(c.m_nEnd),m_nContent(c.m_nContent){}
};

//! список отрезков разметки
typedef std::list< Range >	RangeList_t;

//=====================================================================================//
//                                class IChairContentEx                                //
//=====================================================================================//
//! интерфейс для работы с разметкой кресла на обучающем видео
//! создается функцией CreateInteface
struct IChairContentEx
{
	//! вставка в список отрезков разметки нового размеченного отрезка
	//! \param	RangeList	список отрезков
	//! \param	nStart		начало отрезка 
	//! \param	nEnd		конец отрезка
	//! \param	cont		содержимое отрезка
	//!	\return S_OK - success, all other - error
	virtual HRESULT			AddToRangeList		( RangeList_t& RangeList, int nStart, int nEnd, Range::CONTENT cont ) = 0;

	//! удалить из список отрезков разметки
	//! \param	RangeList	список отрезков
	//! \param	nMin		начало отрезка 
	//! \param	nMax		конец отрезка
	//!	\return S_OK - success, all other - error
	virtual HRESULT			RemoveFromRangeList	( RangeList_t& RangeList, int nMin, int nMax) = 0;

	//! выыести содержимое списка в Output
	//! \param	RangeList	список отрезков
	virtual void			DumpRangeList(const RangeList_t& RangeList) = 0 ;
	
	//! определить текущее состояние для кадра разметки
	//! \param	RangeList	список отрезков
	//! \param	nTime		номер кадра 
	//! \return				содержимое кадра
	virtual Range::CONTENT	GetState( const RangeList_t& RangeList, int nTime ) = 0;
	
	//! загрузка списка разметки из шаблона кресел \sa IChairContentEx::Save
	//! \param	szFileName			имя файла для загрузки
	//! \param	Chairs				массив размеченных кресел, для которых загружаем 
	//! \param	RangeArr			получает список разметки 
	//! \param	sVideoFile			получает название видеофайла, с которого производилась разметка
	//! \param	sChairFile			получает название файла разметки, с которого производилась разметка 
	//! \param	sBackgroundFile		получает название файла фона, с которого производилась разметка кресел
	//!	\return S_OK - success, all other - error
	virtual HRESULT			Load(	LPCWSTR szFileName, 
									const std::vector<BaseChair>&Chairs, 
									std::vector<RangeList_t>& RangeArr,
									std::wstring& sVideoFile,
									std::wstring& sChairFile, 
									std::wstring& sBackgroundFile ) = 0;

	//! сохранение списка разметки из шаблона кресел \sa IChairContentEx::Load
	//! \param	sFileName			имя файла для сохранения
	//! \param	Chairs				массив размеченных кресел, для которых загружаем 
	//! \param	RangeArr			список разметки 
	//! \param	sVideoFile			название видеофайла, с которого производилась разметка
	//! \param	sChairFile			название файла разметки, с которого производилась разметка 
	//! \param	sBackgroundFile		название файла фона, с которого производилась разметка кресел
	//!	\return S_OK - success, all other - error
	virtual HRESULT			Save(	LPCWSTR sFileName, 
									const std::vector<BaseChair>&Chairs, 
									const std::vector<RangeList_t>& RangeArr,
									LPCWSTR sVideoFile,
									LPCWSTR sChairFile, 
									LPCWSTR sBackgroundFile
									) = 0;

	//! освободить интерфейс
	virtual void 			Release() = 0;
};

#endif //__I_CHAIR_CONTENT_EX_H_INCLUDED_3421736227037312__
