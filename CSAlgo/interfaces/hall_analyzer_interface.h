/****************************************************************************
  hall_analyzer_interface.h
  ---------------------
  begin     : Aug 2004
  author(s) : A.Akhriev, E.Gorbachev, K.Lyadvinsky
  email     : aaah@mail.ru
****************************************************************************/

#pragma once

#include "../../CSInterfaces/data_transfer_interface.h"

//=================================================================================================
/** \class IHallAnalyzer.
    \brief Interface class of any hall analyzer.

  Working scenario:
  <tt><pre>
  CreateHallAnalyzer(...);
  SetData( ... any initialization data ... );
  SetData( ... any initialization data ... );
  ....
  for (all cameras) GetData( ... from camera analyzer ... );
  ProcessData(...);
  ....
  for (all cameras) GetData( ... from camera analyzer ... );
  ProcessData(...);
  ....
  for (all cameras) GetData( ... from camera analyzer ... );
  ProcessData(...);
  ....
  Release();
  </pre></tt> */
//=================================================================================================
struct IHallAnalyzer
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function analyzes the current situation throughout the hall.

  \param  spectatorNum  количество зрителей сидевших в зале в течении заданного периода.
  \param  timeStamp     текущее время в миллисекундах.
  \param  period        интервал анализа статистики в миллисекундах, [timeStamp-period,...,timeStamp],
                        period <= 0 means statistics counted off from the very beginning.
  \return               Ok = S_OK. */
///////////////////////////////////////////////////////////////////////////////////////////////////
virtual HRESULT ProcessData( OUT __int32 * pSpectatorNum,
                             IN __int64    timeStamp,
                             IN __int64    period ) = 0;


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function releases interface instance.

  \return Ok = S_OK. */
///////////////////////////////////////////////////////////////////////////////////////////////////
virtual HRESULT Release() = 0;


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function uploads specified data to this object.

  \param  pData  pointer to the input data.
  \return        Ok = S_OK. */
///////////////////////////////////////////////////////////////////////////////////////////////////
virtual HRESULT SetData( IN csinterface::IDataType * pData ) = 0;


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function unloads specified data from this object.

  \param  pData  pointer to the output (possibly resizable) data storage.
  \return        Ok = S_OK. */
///////////////////////////////////////////////////////////////////////////////////////////////////
virtual HRESULT GetData( OUT csinterface::IDataType * pData ) const = 0;

};

