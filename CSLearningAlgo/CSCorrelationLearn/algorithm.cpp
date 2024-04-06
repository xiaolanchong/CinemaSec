/****************************************************************************
algorithm.cpp
---------------------
begin     : Aug 2004
author(s) : A.Akhriev, A. Boltnev
email     : aaah@mail.ru, Alexander.Boltnev@biones.com
****************************************************************************/

#include "stdafx.h"
#include "algorithm.h"
#include "fstream"


//-------------------------------------------------------------------------------------------------
/** \brief Function creates the instance of any learning algorithm.

  \param  pInfo   pointer to the external object that could supply additional information.
  \param  pDebug  pointer to the object that prints debug information.
  \param  ppAlgo  address of pointer that will point to the instance of created object.
  \return         Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
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

		CrossCorrelationLearningAlgo * pAlgo = new CrossCorrelationLearningAlgo();
		if (pAlgo == 0)
			return E_FAIL;

		if (pInfo == 0)
			return E_FAIL;

		HRESULT res = pAlgo->Initialize( pInfo, pDebug); 

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


//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
CrossCorrelationLearningAlgo::CrossCorrelationLearningAlgo()
: bReady(false)
, totalIntervals(256)
, min_v(0.0f)
, max_v(2.0f)
{
}


//-------------------------------------------------------------------------------------------------
/** \brief Destructor. */
//-------------------------------------------------------------------------------------------------
CrossCorrelationLearningAlgo::~CrossCorrelationLearningAlgo()
{
}


//-------------------------------------------------------------------------------------------------
/** \brief Function launches computational process.

  \param  startTime  the start time of surveillance process in seconds.
  \return            Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
HRESULT CrossCorrelationLearningAlgo::Start( IN float startTime )
{
	HRESULT retValue = E_FAIL;

	try
	{
		InitCamAnalyzer(); // initializes camera analyzer, background picture, etc
		HRESULT startResult = pCameraAnalyzer->Start(__int64 (startTime * 1000.0));

		histoArr.resize(emptyHallChairArr.size());
		for (size_t i = 0; i < histoArr.size(); i++) 
		{
			histoArr[i].content = EMPTY;
			histoArr[i].histo.Initialize(min_v, max_v, totalIntervals);			
		}
		totalHisto.Initialize(min_v, max_v, totalIntervals);
		std::fstream file;
		file.open( "histogram_TMP.dat", std::ios_base::in);
		totalHisto.Load(file);
		file.close();

		if (startResult == S_OK)
			pInfo->SetData(&csinterface::TMsgIAmReady());
		frame = 0;
		retValue = startResult;
	}
	catch (std::runtime_error & e)
	{
		e;
	}
	return retValue;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function receives a successive image and processes it.

  \param  pBI        pointer to the image's header.
  \param  pImage     pointer to the image's beginning.
  \param  timeStamp  the current time in seconds.
  \return            Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
HRESULT CrossCorrelationLearningAlgo::ProcessImage( IN const BITMAPINFO * pBI,
                                                     IN const __int8     * pImage,
                                                    IN float              timeStamp )
{
	HRESULT retValue = E_FAIL;
	try
	{
		HRESULT res = pCameraAnalyzer->ProcessImage(pBI, pImage, __int64(timeStamp * 1000.0f));
		
		csutility::CopyDIBToColorImage(pBI, (LPCUBYTE)pImage, &testImage);
		
		char str[25];
		ltoa(frame, str, 10);
		csutility::DrawDigits5x10(str, 50, 25, true, &testImage, RGB(255,255,255), RGB(0,0,0));

		
		if( res == S_OK )
		{	
			// get information about chair states
			csinterface::TChairContentsMap chairContentsMap;
			pInfo->GetData(&chairContentsMap);

			csalgo::TChairLearnArr chairLearnArr;
			if (FAILED(pCameraAnalyzer->GetData( &chairLearnArr ))) AfxMessageBox(_T("error in pCameraAnalyzer->GetData()"));

			DrawContentAndCurves(chairContentsMap, chairLearnArr, testImage);
			
			// save to histogram
			for (size_t i = 0; i < chairLearnArr.data.size(); i++) // go through chairs and save data to the total histogram
			{
				long index = chairLearnArr.data[i].index;
				ChairContents answerCC = chairContentsMap.data.find(index)->second; 
				VERIFY(chairContentsMap.data.find(index) != chairContentsMap.data.end());
	
				LayerName answer;
				switch(answerCC) {
				case CHAIR_EMPTY:
					answer = LayerName::EMPTY;
					break;
				case CHAIR_HUMAN:
					answer = LayerName::HUMAN;
					break;
				case CHAIR_CLOTHES:
					answer = LayerName::CLOTH;
					break;
				default:
					continue;
				}
			
				 
				
				if (histoArr[i].content != answer)
				{
					histoArr[i].histo.Normilize();
					totalHisto += histoArr[i].histo;
					histoArr[i].histo.FillZero();
					histoArr[i].content = answer;
				}
				histoArr[i].histo.AddValue(chairLearnArr.data[i].statNCC, answer);
			}
		}
		retValue = S_OK;
		frame++;
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
HRESULT CrossCorrelationLearningAlgo::Stop( IN float stopTime, bool saveResult )
{
	if (saveResult)
		SaveFinalData();
	SetData(&csinterface::TMsgSaveTempData());
	HRESULT retValue = E_FAIL;

	size_t size = histoArr.size();
	try
	{
		retValue = pCameraAnalyzer->Stop(  __int64(stopTime * 1000.0f));
		pCameraAnalyzer->Release();
	}
	catch (std::runtime_error & e)
	{
		e;
	}
	return retValue;
}

HRESULT CrossCorrelationLearningAlgo::SaveFinalData()
{
	// summ normalizes histograms
	for (size_t i = 0; i < histoArr.size(); i++) // go through chairs and save data to the total histogram
	{
		csutility::MultiHistogram < float , 3 > tmpHist;
		tmpHist.Initialize(min_v, max_v, totalIntervals);
		tmpHist = histoArr[i].histo;
		tmpHist.Normilize();
		totalHisto += tmpHist;
	}
	csutility::MultiHistogram < float , 3 > allTotalHist;
	allTotalHist.Initialize(min_v, max_v, totalIntervals);
	allTotalHist = totalHisto;
	allTotalHist.Normilize();

	std::fstream file;
	file.open( "histogram.dat", std::ios_base::out | std::ios_base::trunc );
	allTotalHist.Save(file);
	file.close();
	return S_OK;
}

//-------------------------------------------------------------------------------------------------
/** \brief Function releases the implementation of this interface.

  \return  Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
HRESULT CrossCorrelationLearningAlgo::Release()
{	
	delete this;
	return S_OK;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function uploads specified data to this object.

  \param  pData  pointer to the input data.
  \return        Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
HRESULT CrossCorrelationLearningAlgo::SetData( IN const csinterface::IDataType * pData )
{
	switch( pData->type() )
	{
	case csinterface::DATAID_MSG_SAVE_TEMP_DATA:
		{
			csutility::MultiHistogram < float , 3 > allTotalHist;
			allTotalHist.Initialize(min_v, max_v, totalIntervals);
			allTotalHist = totalHisto;

			for (size_t i = 0; i < histoArr.size(); i++) // go through chairs and save data to the total histogram
			{
				csutility::MultiHistogram < float , 3 > tmpHist;
				tmpHist.Initialize(min_v, max_v, totalIntervals);
				tmpHist = histoArr[i].histo;
				tmpHist.Normilize();
				allTotalHist += tmpHist;
			}

			std::fstream file;
			file.open( "histogram_TMP.dat", std::ios_base::out | std::ios_base::trunc );
			allTotalHist.Save(file);
			file.close();
			return S_OK;
		}
		break;
	case csinterface::DATAID_MSG_LOAD_TEMP_DATA:
		{
			std::fstream file;
			file.open( "histogram_TMP.dat", std::ios_base::in);
			totalHisto.Load(file);
			file.close();
			return S_OK;
		}
		break;
	case csinterface::DATAID_MSG_CLEAN_TEMP_DATA:
		{
			std::fstream file;
			file.open( "histogram_TMP.dat", std::ios_base::out | std::ios_base::trunc );
			file.close();
			return S_OK;
		}
		break;
	}
	return E_FAIL;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function unloads specified data from this object.

  \param  pData  pointer to the output (possibly resizable) data storage, say STL container.
  \return        Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
HRESULT CrossCorrelationLearningAlgo::GetData( OUT csinterface::IDataType * pData ) const
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
/** \brief Function sends request to the external application and receives filled up data storage.

  \param  pData  pointer to a data storage to be filled up.
  \return        Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
HRESULT CrossCorrelationLearningAlgo::QueryData( IN OUT csinterface::IDataType * pData ) const
{
	pData;
	return S_OK;
}

// initialize camera analyzer
HRESULT CrossCorrelationLearningAlgo::InitCamAnalyzer()
{
	csinterface::TBackground   bkgrnd;
	csinterface::TEmptyHallArr emptyHall;

	pInfo->GetData(&bkgrnd);
	pInfo->GetData(&emptyHall);

	emptyHallChairArr = emptyHall.data;
	fBackground       = bkgrnd.data;


	Int8Arr binParams;
	VERIFY(!FAILED(SetDefaultParameters(&binParams, NULL)));
	//TO DO: // show parameters dialog!!!
	HRESULT res = CreateCameraAnalyzer(&pCameraAnalyzer, &binParams, &emptyHallChairArr, &fBackground, emptyHallChairArr[0].cameraNo, pDebugOutput);
	if (res == S_OK) bReady = true;
	if (res != S_OK ) OutputDebugString(_T("Camera analyzer cannot be initialized!"));
	return res;
}

HRESULT CrossCorrelationLearningAlgo::Initialize( csinterface::IInformationQuery * pInfo, 
												  IDebugOutput                   * pDebugOutput) 
{
	this->pInfo = pInfo;
	this->pDebugOutput = pDebugOutput;
	return S_OK;
}

HRESULT CrossCorrelationLearningAlgo::DrawContentAndCurves(csinterface::TChairContentsMap& chairContentsMap, csalgo::TChairLearnArr& chairLearnArr, QImage& img)
{
	Vec2i   pt;
	RGBQUAD color;

	for (size_t i = 0; i < emptyHallChairArr.size(); i++)
	{
		Vec2fArr curve = emptyHallChairArr[i].curve;
		for (int j = 0; j < (int)curve.size(); j++)
		{

			//long index = emptyHallChairArr[i].index;
			long index = chairLearnArr.data[i].index;
			ChairContents answer = chairContentsMap.data.find(index)->second; 

			switch(answer) 
			{
			case CHAIR_EMPTY:
				{
					color.rgbRed = 0;
					color.rgbGreen = 255;
					color.rgbBlue = 255;
					color.rgbReserved = 0;
				}
				break;
			case CHAIR_HUMAN:
				{
					color.rgbRed = 0;
					color.rgbGreen = 0;
					color.rgbBlue = 255;
					color.rgbReserved = 0;
				}
				break;
			case CHAIR_CLOTHES:
				{
					color.rgbRed = 255;
					color.rgbGreen = 0;
					color.rgbBlue = 0;
					color.rgbReserved = 0;
				}
				break;
			case CHAIR_UNDEFINED:
				{
					color.rgbRed = 0;
					color.rgbGreen = 255;
					color.rgbBlue = 0;
					color.rgbReserved = 0;
				}
				break;
			default:
				{
					color.rgbRed = 255;
					color.rgbGreen = 255;
					color.rgbBlue = 255;
					color.rgbReserved = 0;
				}
			}

			pt = curve[j];
			img[pt] = color;
		}
	}
	return S_OK;
}