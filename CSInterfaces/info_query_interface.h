/****************************************************************************
  info_query_interface.h
  ---------------------
  begin     : Aug 2004
  author(s) : A.Akhriev
  email     : aaah@mail.ru
****************************************************************************/

#pragma once

#include "common_interface_settings.h"
#include "data_transfer_interface.h"

namespace csinterface
{

//=================================================================================================
/** \brief Interface of any data storage that should be transferred between system's components. */
//=================================================================================================
struct IInformationQuery
{

//-------------------------------------------------------------------------------------------------
/** \brief Function uploads specified data to this object.

  \param  pData  pointer to the input data.
  \return        Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
virtual HRESULT SetData( IN const csinterface::IDataType * pData ) = 0;


//-------------------------------------------------------------------------------------------------
/** \brief Function unloads specified data from this object.

  \param  pData  pointer to the output (possibly resizable) data storage, say STL container.
  \return        Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
virtual HRESULT GetData( OUT csinterface::IDataType * pData ) const = 0;


//-------------------------------------------------------------------------------------------------
/** \brief Function sends request to the external application and receives filled up data storage.

  \param  pData  pointer to a data storage to be filled up.
  \return        Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
virtual HRESULT QueryData( IN OUT csinterface::IDataType * pData ) const = 0;

};

} // namespace csinterface

