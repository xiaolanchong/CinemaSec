#include "stdafx.h"
#include "algorithm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


AlgorithmVersionTwo::AlgorithmVersionTwo() : params(FIRST_CONTROL_IDENTIFIER)
{
	// set parameters to default values
	csutility::SetDefaultParameters < Parameters > ( params, 0, 0 );
}


AlgorithmVersionTwo::~AlgorithmVersionTwo()
{
}

HRESULT AlgorithmVersionTwo::Initialize( csinterface::IInformationQuery * pInfo, 
										csinterface::IDebugOutput * pDebugOutput) 
{
	this->pInfo = pInfo;
	this->pDebugOutput = pDebugOutput;
	return S_OK;
}


HRESULT AlgorithmVersionTwo::Start( IN float startTime )
{

	HRESULT retValue = E_FAIL;
	try
	{
		// here we obtain image of the background 
		{
			csinterface::TBackground bkgrnd;
			pInfo->GetData(&bkgrnd);
			fBackground = bkgrnd.data;
		}
		// here we obtain the set of chairs in empty hall
		{
			csinterface::TEmptyHallArr emptyHall;
			pInfo->GetData(&emptyHall);
			emptyHallChairArr = emptyHall.data;
		}

		// initialize "extended chairs"
		{
			emptyHallChairArrEx.clear();
			emptyHallChairArrEx.resize(emptyHallChairArr.size());

			for (int i = 0; i < (int)emptyHallChairArr.size(); i++ )
			{
				emptyHallChairArrEx[i] = emptyHallChairArr[i];
			}

			csalgocommon::InitializeChairs < BaseChairExArr > ( emptyHallChairArrEx, 0, -1, true);
		}

		// initialize arrays with behaviour objects for each chair
		{
			dynamicArr.clear();
			staticArr.clear();
			for (int i = 0; i < (int)emptyHallChairArr.size(); i++)
			{
				std::pair < ChairContents, DynamicChairBehaviour > dynamicElem;
				dynamicElem.second.Reset( true ); // use "mean" normalization
				dynamicElem.first = CHAIR_UNDEFINED;
				dynamicArr.push_back(dynamicElem);			

				std::pair < ChairContents, StaticChairBehaviour > staticElem;
				staticElem.first = CHAIR_UNDEFINED;
				staticArr.push_back(staticElem);			
			}
		}

		// open output streams
		OpenOutputStreams();

		// initialize video writers
		{
			videoWritersArr.clear();
			for (int i = 0; i < (int)emptyHallChairArr.size(); i++)
			{
                VideoWriter writer;
				writer.Reset( i, resultPath + _T("\\"), CHAIR_UNDEFINED, _T("nope") );
				videoWritersArr.push_back( writer );
			}
		}
		standardizer.Initialize( true, 64, 64, 2 );

		m_acquirer.Initialize( 0, 0, params.s_acquirer );

		//we start from the first frame
		m_skip = params.s_sequence.p_framesToSkip();

		frame = 0;

		retValue = S_OK;
	}
	catch (std::runtime_error & e)
	{
		e;
	}
	return retValue;
}

HRESULT AlgorithmVersionTwo::ProcessImage( IN const BITMAPINFO * pBI,
										  IN const __int8     * pImage,
										  IN float              timeStamp )
{
	CSingleLock lock( &m_critSection, TRUE );

	HRESULT retValue = E_FAIL;

	try
	{

		m_acquirer.Acquire( pBI, (const ubyte *)pImage, 0 );

		csinterface::TPlaylistParameters params;
		pInfo->GetData(&params);

		// build up the test image and let it go
		{		
		//	csutility::CopyDIBToColorImage(pBI, (LPCUBYTE)pImage, &testImage);
			floatImage = m_acquirer.GetSmoothImage();
			csutility::CopyFloatImageToColorImage( &floatImage, &testImage );
			// draw frame number
			char str[25];
			ltoa(frame, str, 10);
			csutility::DrawDigits5x10(str, 70, 25, true, &testImage, RGB(255,10,20), RGB(200,200,200));
			std::wstring strVideoID = params.data.item.video;
		
			//USES_CONVERSION;
			//csutility::DrawDigits5x10("123", 170, 55, true, &testImage, RGB(255,10,20), RGB(200,200,200));
		}

		{	// daynight test
			DayNight daynight( 115, 68 );

			Arr2f daynightImg;
			csutility::CopyDIBToFloatImage( pBI, (LPCUBYTE)pImage, &daynightImg );
			if ( daynight.Determine( daynightImg ) )
			{
				csutility::DrawDigits5x10("1", 170, 55, true, &testImage, RGB(255,10,20), RGB(200,200,200));
			}
			else
			{
				csutility::DrawDigits5x10("0", 170, 55, true, &testImage, RGB(255,10,20), RGB(200,200,200));
			}
		}
	
		// get information about chair states
		csinterface::TChairContentsMap chairContentsMap;
		pInfo->GetData(&chairContentsMap);


		// go through the chairs and update behaviour objects
		for (int i = 0; i < (int)emptyHallChairArr.size(); i++)
		{
			long index = emptyHallChairArr[i].index;
			ChairContents answerCC = chairContentsMap.data.find(index)->second; 
			// skip	bad chairs (some errors in marking)
			if (chairContentsMap.data.find(index) == chairContentsMap.data.end()) continue;

			// we look only for chairs with right contents
			switch(answerCC) 
			{
			case CHAIR_EMPTY:
				break;
			case CHAIR_HUMAN:
				break;
			case CHAIR_CLOTHES:
				break;
			default:
				continue;
			}

			// draw curve with contents
			{
				DrawCurveWithContents(emptyHallChairArr[i].curve, answerCC, testImage);
			}

			/* macaroni code
			//clear chair if interval ends
			if ( dynamicArr[i].first != answerCC )
			{
			PutChairToStream( i );
			dynamicArr[i].second.Reset( true );
			staticArr[i].second.Initialize();
			dynamicArr[i].first = answerCC;
			staticArr[i].first = answerCC;
			}



			dynamicArr[i].second.Update(floatImage, emptyHallChairArrEx[i]);
			staticArr[i].second.Compute(floatImage, fBackground, emptyHallChairArrEx[i]);


			if ( frame % 300 == 0 )
			{
			PutChairToStream( i );

			}
			*/

			if ( videoWritersArr[i].contents != answerCC )
			{
				std::wstring cutRight = _T("_video.avi");
				std::wstring strVideoID = params.data.item.video.substr(0, strVideoID.length() - cutRight.length());

				strVideoID = strVideoID.substr(0, strVideoID.length() - 10);
				videoWritersArr[i].Reset( i, resultPath + _T("\\"), answerCC, strVideoID );
			}
			if ( frame % m_skip == 0 )
			{
				Arr2ub res;
				standardizer.GetStandardImage(floatImage, emptyHallChairArr[i].curve, res, 0.0f );
				videoWritersArr[i].WriteFrame( &res );
			}
		}

		frame++;
		retValue = S_OK;
	}
	catch (std::runtime_error & e)
	{
		e;
	}
	return retValue;  
}

HRESULT AlgorithmVersionTwo::Stop( IN float stopTime, bool saveResult )
{
	stopTime;
	HRESULT retValue = E_FAIL;
	if (saveResult == true) 
		SaveFinalData();
	CloseStreams();

	try
	{
		retValue = S_OK;
	}
	catch (std::runtime_error & e)
	{
		e;
	}
	return retValue;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function releases the implementation of this interface.

\return  Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
HRESULT AlgorithmVersionTwo::Release()
{
	delete this;
	return S_OK;
}

HRESULT AlgorithmVersionTwo::SaveFinalData()
{
	CloseStreams();
	//FloatArr staticEmpty;
	//FloatArr staticClothes;
	//FloatArr staticHuman;
	//FloatArr dynamicThing;
	//FloatArr dynamicHuman;
	//LoadTempData(staticEmpty,  staticClothes, staticHuman, dynamicThing, dynamicHuman);
	LearningFinalizer finalizer;
	//finalizer.Process(staticEmpty,  staticClothes, staticHuman, dynamicThing, dynamicHuman, resultPath.c_str(), pDebugOutput);



	return S_OK;
}

HRESULT AlgorithmVersionTwo::SetData( IN const csinterface::IDataType * pData )
{
	switch( pData->type() )
	{
	case csinterface::DATAID_MSG_SAVE_PARAMETERS:
		{

			csinterface::TMsgSaveParameters * pMsg = (csinterface::TMsgSaveParameters *) pData;
			std::wstring path = pMsg->data;
			csutility::SaveParameters < Parameters > ( path.c_str(), params, 0 );			
			return S_OK;
		}
		break;
	case csinterface::DATAID_MSG_LOAD_PARAMETERS:
		{
			csinterface::TMsgSaveParameters * pMsg = (csinterface::TMsgSaveParameters *) pData;
			std::wstring path = pMsg->data;
			csutility::LoadParameters < Parameters > ( path.c_str(), params, 0 );			
			return S_OK;
		}
		break;
	case csinterface::DATAID_MSG_SET_DEFAULT_PARAMS:
		{
			csutility::SetDefaultParameters < Parameters > ( params, 0, 0 );
		}
		break;
	case csinterface::DATAID_MSG_CLEAN_TEMP_DATA:
		{
			CleanTmpFiles();
			return S_OK;
		}
		break;
	case csinterface::DATAID_RESULT_FILENAME:
		{
			csinterface::TResultFilename * pFn =  (csinterface::TResultFilename *)pData;
			resultPath = pFn->data;
			return S_OK;
		}
		break;
	case csinterface::DATAID_MSG_SHOW_PARAM_DIALOG:
		{
			{	// daynight test
				DayNight daynight( 150, 57 );
	
				float ithr;
				float pthr; 

				//daynight.Learn( L"d:\\video\\DayNightVideo\\day1.avd", true );
				//daynight.Learn( L"d:\\video\\DayNightVideo\\day2.avd", true );
				daynight.Learn( L"d:\\video\\DayNightVideo\\day3.avd", true );
				daynight.Learn( L"d:\\video\\DayNightVideo\\day4.avd", true );
				daynight.Learn( L"d:\\video\\DayNightVideo\\day5.avd", true );
				daynight.Learn( L"d:\\video\\DayNightVideo\\day6.avd", true );


				daynight.Learn( L"d:\\video\\DayNightVideo\\night1.avd", false );
				daynight.Learn( L"d:\\video\\DayNightVideo\\night2.avd", false );
				daynight.Learn( L"d:\\video\\DayNightVideo\\night3.avd", false );
				//daynight.Learn( L"d:\\video\\DayNightVideo\\night4.avd", false );
				daynight.Learn( L"d:\\video\\DayNightVideo\\night5.avd", false );
				//daynight.Learn( L"d:\\video\\DayNightVideo\\night6.avd", false );
				daynight.Learn( L"d:\\video\\DayNightVideo\\night7.avd", false );
				//daynight.Learn( L"d:\\video\\DayNightVideo\\night8.avd", false );

				daynight.GetParameters( ithr, pthr );

				CString str;
				str.Format( L"%f %f", ithr, pthr );
				AfxMessageBox( str );

			}


			csutility::ShowParameterDialog < Parameters >( 0, 100, 100, params, 0, 0 );
			return S_OK;
		}

	}
	return E_FAIL;
}


HRESULT AlgorithmVersionTwo::GetData( OUT csinterface::IDataType * pData ) const
{
	CSingleLock lock( &((const_cast<AlgorithmVersionTwo*>(this))->m_critSection), TRUE );
	
	
	switch( pData->type() )
	{
	case csinterface::DATAID_QIMAGE:
		{	
			csinterface::TQImage * pImage = (csinterface::TQImage *) pData;
			pImage->data = testImage;
			return S_OK;		
		}
	}
	return E_FAIL;
}

HRESULT AlgorithmVersionTwo::QueryData( IN OUT csinterface::IDataType * pData ) const
{
	pData;
	return S_OK;
}

void AlgorithmVersionTwo::DrawCurveWithContents(const Vec2fArr& curve, const ChairContents content, QImage& image)
{
	RGBQUAD color;
	switch(content) {
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
	for (int j = 0; j < (int)curve.size(); j++)
	{
		Vec2i pt = curve[j];
		image[pt] = color;	
	}
}

void AlgorithmVersionTwo::OpenOutputStreams()
{
	USES_CONVERSION;
	std::wstring fname; 

	fname = resultPath + L"\\dynamicParamHumanFile.txt";
	dynamicParamHumanFile.clear();
	dynamicParamHumanFile.open( W2CA( fname.c_str() ), std::ios_base::out | std::ios_base::app );

	fname = resultPath + L"\\dynamicParamClothFile.txt";
	dynamicParamClothFile.clear();
	dynamicParamClothFile.open( W2CA( fname.c_str() ), std::ios_base::out | std::ios_base::app );

	fname = resultPath + L"\\staticParamHumanFile.txt";
	staticParamHumanFile.clear();
	staticParamHumanFile.open( W2CA( fname.c_str() ), std::ios_base::out | std::ios_base::app );

	fname = resultPath + L"\\staticParamClothFile.txt";
	staticParamClothFile.clear();
	staticParamClothFile.open( W2CA( fname.c_str() ), std::ios_base::out | std::ios_base::app );

	fname = resultPath + L"\\staticParamEmptyFile.txt";
	staticParamEmptyFile.clear();
	staticParamEmptyFile.open( W2CA( fname.c_str() ), std::ios_base::out | std::ios_base::app );
}

void AlgorithmVersionTwo::CleanTmpFiles()
{
	USES_CONVERSION;
	std::wstring fname; 

	fname = resultPath + L"\\dynamicParamHumanFile.txt";
	dynamicParamHumanFile.clear();
	dynamicParamHumanFile.open( W2A( fname.c_str() ), std::ios_base::out | std::ios_base::trunc );
	dynamicParamHumanFile << " ";

	fname = resultPath + L"\\dynamicParamClothFile.txt";
	dynamicParamClothFile.clear();
	dynamicParamClothFile.open( W2CA( fname.c_str() ), std::ios_base::out | std::ios_base::trunc );
	dynamicParamClothFile << " ";

	fname = resultPath + L"\\staticParamHumanFile.txt";
	staticParamHumanFile.clear();
	staticParamHumanFile.open( W2CA( fname.c_str() ), std::ios_base::out | std::ios_base::trunc );
	staticParamHumanFile << " ";

	fname = resultPath + L"\\staticParamClothFile.txt";
	staticParamClothFile.clear();
	staticParamClothFile.open( W2CA( fname.c_str() ), std::ios_base::out | std::ios_base::trunc );
	staticParamClothFile << " ";

	fname = resultPath + L"\\staticParamEmptyFile.txt";
	staticParamEmptyFile.clear();
	staticParamEmptyFile.open( W2CA( fname.c_str() ), std::ios_base::out | std::ios_base::trunc );
	staticParamEmptyFile << " ";
	CloseStreams();
}

void AlgorithmVersionTwo::OpenInputStreams()
{
	USES_CONVERSION;
	std::wstring fname; 

	fname = resultPath + L"\\dynamicParamHumanFile.txt";
	dynamicParamHumanFile.clear();
	dynamicParamHumanFile.open( W2CA( fname.c_str() ), std::ios_base::in );

	fname = resultPath + L"\\dynamicParamClothFile.txt";
	dynamicParamClothFile.clear();
	dynamicParamClothFile.open( W2CA( fname.c_str() ), std::ios_base::in );

	fname = resultPath + L"\\staticParamHumanFile.txt";
	staticParamHumanFile.clear();
	staticParamHumanFile.open( W2CA( fname.c_str() ), std::ios_base::in );

	fname = resultPath + L"\\staticParamClothFile.txt";
	staticParamClothFile.clear();
	staticParamClothFile.open( W2CA( fname.c_str() ), std::ios_base::in );

	fname = resultPath + L"\\staticParamEmptyFile.txt";
	staticParamEmptyFile.clear();
	staticParamEmptyFile.open( W2CA( fname.c_str() ), std::ios_base::in );

}

void AlgorithmVersionTwo::CloseStreams()
{
	dynamicParamHumanFile.close();
	dynamicParamClothFile.close();
	staticParamHumanFile.close();
	staticParamClothFile.close();
	staticParamEmptyFile.close();
}
void AlgorithmVersionTwo::PutChairToStream( int i )
{
	VERIFY( dynamicArr[i].first == staticArr[i].first );


	ChairContents chairType = dynamicArr[i].first;
	if ( chairType == CHAIR_HUMAN )
	{
		if ( dynamicArr[i].second.GetFrameCounter() > 250 )
			dynamicParamHumanFile <<  dynamicArr[i].second.GetDynamicCharacteristic() << std::endl;

		staticParamHumanFile << staticArr[i].second.GetLastComputedValue() << std::endl;
	}
	else if ( chairType == CHAIR_CLOTHES )
	{
		if ( dynamicArr[i].second.GetFrameCounter() > 250 )
			dynamicParamClothFile << dynamicArr[i].second.GetDynamicCharacteristic() << std::endl;

		staticParamClothFile << staticArr[i].second.GetLastComputedValue() << std::endl;
	}
	else if ( chairType == CHAIR_EMPTY )
	{

		if ( dynamicArr[i].second.GetFrameCounter() > 250 )
			dynamicParamClothFile <<  dynamicArr[i].second.GetDynamicCharacteristic() << std::endl;

		staticParamEmptyFile << staticArr[i].second.GetLastComputedValue() << std::endl;
	}
}

bool AlgorithmVersionTwo::LoadTempData( FloatArr & staticEmpty, 
										FloatArr & staticClothes,
										FloatArr & staticHuman,
										FloatArr & dynamicThing,
										FloatArr & dynamicHuman  )
{
	OpenInputStreams();
	staticEmpty.clear();

	float flt = 0.0;//loacal temporary variable

	while ( !staticParamEmptyFile.eof() )
	{
		staticParamEmptyFile >> flt;
		staticEmpty.push_back(flt);
	}

	staticClothes.clear();
	while ( !staticParamClothFile.eof() )
	{
		staticParamClothFile >> flt;
		staticClothes.push_back(flt);
	}

	staticHuman.clear();
	while ( !staticParamHumanFile.eof() )
	{
		staticParamHumanFile >> flt;
		staticHuman.push_back(flt);
	}

	dynamicThing.clear();
	while ( !dynamicParamClothFile.eof() )
	{
		dynamicParamClothFile >> flt;
		dynamicThing.push_back(flt);
	}

	dynamicHuman.clear();
	while ( !dynamicParamHumanFile.eof() )
	{
		dynamicParamHumanFile >> flt;
		dynamicHuman.push_back(flt);
	}

	return true; // returns true if succesful ends
}