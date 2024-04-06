//////////////////////////////////////////////////////////////////////////
// SeriesAlgorithm.cpp
// ELVEES
// author: Alexander Boltnev
// e-mail: Alexander.Boltnev@biones.com
// date  : May 2005
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "SeriesAlgorithm.h"
#include "../../CSPlayList/CSDataPrepareGlobals.h"

HRESULT CreateLearningAlgorithmInstance( IN  csinterface::IInformationQuery   * pInfo,
									     IN  IDebugOutput        * pDebug,
										 OUT csinterface::ILearningAlgorithm ** ppAlgo )
{
	HRESULT retValue = E_FAIL;

	try
	{
		if (ppAlgo == 0)
			return E_FAIL;
		(*ppAlgo) = 0;

		SeriesAlgorithm * pAlgo = new SeriesAlgorithm();
		if (pAlgo == 0)
			return E_FAIL;

		if (pInfo == 0)
			return E_FAIL;

		HRESULT res = pAlgo->Initialize( pInfo, pDebug ); 

		if (FAILED( res ))
			delete pAlgo;
		else
			(*ppAlgo) = pAlgo;
		return res;
	}
	catch (std::runtime_error & e)
	{
		e;
	}

	return retValue;
}

LPCWSTR GetLearningAlgorithmName()
{
	return L"Series learning";
}


SeriesAlgorithm::SeriesAlgorithm(void):
	bReady(false)
{

}

SeriesAlgorithm::~SeriesAlgorithm(void)
{

}
//-------------------------------------------------------------------------------------------------
/** \brief Function launches computational process.

\param  startTime  the start time of surveillance process in seconds.
\return            Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
HRESULT SeriesAlgorithm::Start( IN float startTime )
{
	HRESULT retValue = E_FAIL;
	try
	{
		bReady = true;
		HRESULT startResult = S_OK;
		if (startResult == S_OK)
		{
			csinterface::TMsgIAmReady msg;
			pInfo->SetData(&msg);
		}
		retValue = startResult;
	}
	catch (std::runtime_error & e)
	{
		e;
	}
	return retValue;
}

//-------------------------------------------------------------------------------------------------
/** \brief Function unloads specified data from this object.

\param  pData  pointer to the output (possibly resizable) data storage, say STL container.
\return        Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
HRESULT SeriesAlgorithm::GetData( OUT csinterface::IDataType * pData ) const
{
	switch( pData->type() )
	{
	case csinterface::DATAID_QIMAGE:
		if (bReady == true)
		{
			csinterface::TQImage * pImage = (csinterface::TQImage *) pData;
			pImage->data = testImage;
			return S_OK;
		}
		else
		{
			return E_FAIL;
		}
	}
	return E_FAIL;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function uploads specified data to this object.

\param  pData  pointer to the input data.
\return        Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
HRESULT SeriesAlgorithm::SetData( IN const csinterface::IDataType * pData )
{
	switch( pData->type() )
	{
	case csinterface::DATAID_MSG_SAVE_TEMP_DATA:
		{
			return S_OK;
		}
		break;
	case csinterface::DATAID_MSG_LOAD_TEMP_DATA:
		{
			return S_OK;
		}
		break;
	case csinterface::DATAID_MSG_CLEAN_TEMP_DATA:
		{
			return S_OK;
		}
		break;
	}
	return E_FAIL;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function releases the implementation of this interface.

\return  Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
HRESULT SeriesAlgorithm::Release()
{	
	delete this;
	return S_OK;
}

HRESULT SeriesAlgorithm::Initialize( csinterface::IInformationQuery * pInfo, 
												 IDebugOutput                   * pDebugOutput) 
{
	this->pInfo = pInfo;
	this->pDebugOutput = pDebugOutput;
	return S_OK;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function receives a successive image and processes it.

\param  pBI        pointer to the image's header.
\param  pImage     pointer to the image's beginning.
\param  timeStamp  the current time in seconds.
\return            Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
HRESULT SeriesAlgorithm::ProcessImage( IN const BITMAPINFO * pBI,
									   IN const __int8     * pImage,
									   IN float              timeStamp )
{
	HRESULT retValue = E_FAIL;
	try
	{
		if (csutility::CopyDIBToColorImage(pBI, (LPCUBYTE)pImage, &testImage) == true) retValue = S_OK;
	}
	catch (std::runtime_error & e)
	{
		e;
	}
	return retValue;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function finalizes computational process.

\param  stopTime  the stop time of surveillance process in seconds.
\return           Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
HRESULT SeriesAlgorithm::Stop( IN float stopTime, bool saveResult )
{
	saveResult;
	csinterface::TMsgSaveTempData msg;
	SetData(&msg);
	HRESULT retValue = E_FAIL;
	try
	{
		bReady = false;
		retValue = S_OK;
	}
	catch (std::runtime_error & e)
	{
		e;
	}
	return retValue;
}




//-------------------------------------------------------------------------------------------------
/** \brief Function sends request to the external application and receives filled up data storage.

\param  pData  pointer to a data storage to be filled up.
\return        Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
HRESULT SeriesAlgorithm::QueryData( IN OUT csinterface::IDataType * pData ) const
{
	pData;
	return S_OK;
}