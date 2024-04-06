/****************************************************************************
  camera_analyzer_interface.h
  ---------------------
  begin     : Aug 2004
  author(s) : A.Akhriev, E.Gorbachev, K.Lyadvinsky
  email     : aaah@mail.ru
****************************************************************************/

#pragma once

#include "../../CSInterfaces/data_transfer_types.h"

//=================================================================================================
/** \struct ICameraAnalyzer.
    \brief  Interface class of any camera analyzer. 

  Working scenario:
  <tt><pre>
  CreateCameraAnalyzer(...);
  SetData( ... any initialization data ... );
  SetData( ... any initialization data ... );
  Start(...);
  while (...);
  {
    ProcessImage(...);
    if (...) GetData( ... for hall analyzer ... );
  }
  Stop();
  Release();
  </pre></tt> */
//=================================================================================================
struct ICameraAnalyzer
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief ������ ������ ���������.

  \param  startTime  ����� ������� �������� �������� � �������������.
  \return            Ok = S_OK. */
///////////////////////////////////////////////////////////////////////////////////////////////////
virtual HRESULT Start( IN __int64 startTime ) = 0;


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief �������� ��������� ����������� ��� ��������� � ���������� ���.

  \param  pBI        ��������� �� ��������� �����������.
  \param  pBytes     ��������� �� ������ �����������.
  \param  timeStamp  ������� ����� � �������������.
  \return            Ok = S_OK. */
///////////////////////////////////////////////////////////////////////////////////////////////////
virtual HRESULT ProcessImage( IN const BITMAPINFO * pBI,
                              IN const void       * pBytes,
                              IN __int64            timeStamp ) = 0;


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief ��������� ������ ���������.

  \param  stopTime  the stop time of surveillance process in milliseconds.
  \return           Ok = S_OK. */
///////////////////////////////////////////////////////////////////////////////////////////////////
virtual HRESULT Stop( IN __int64 stopTime ) = 0;


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function releases interface instance.

  \return  Ok = S_OK. */
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

  \param  pData  pointer to the output (possibly resizable) data storage, say STL container.
  \return        Ok = S_OK. */
///////////////////////////////////////////////////////////////////////////////////////////////////
virtual HRESULT GetData( OUT csinterface::IDataType * pData ) const = 0;

};

