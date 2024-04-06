/****************************************************************************
  EigenAlgorithm.cpp
  ---------------------
  begin     : Aug 2004
  author(s) : A.Akhriev, A. Boltnev
  email     : aaah@mail.ru, Alexander.Boltnev@biones.com
****************************************************************************/

#include "stdafx.h"

#include "../../CSPlayList/CSDataPrepareGlobals.h"
#include "EigAlgoImageHolder.h"
#include "SpectrumApproximator.h"
#include "EigenAlgorithm.h"
#include "../../CSUtility/utility/multi_histogram.h"



//-------------------------------------------------------------------------------------------------
/** \brief Function creates the instance of any learning algorithm.

  \param  pInfo   pointer to the external object that could supply additional information.
  \param  pDebug  pointer to the object that prints debug information.
  \param  ppAlgo  address of pointer that will point to the instance of created object.
  \return         Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
HRESULT CreateLearningAlgorithmInstance( IN  csinterface::IInformationQuery   * pInfo,
                                         IN  csinterface::IDebugOutput        * pDebug,
                                         OUT csinterface::ILearningAlgorithm ** ppAlgo )
{
	HRESULT retValue = E_FAIL;

	try
	{
		if (ppAlgo == 0)
			return E_FAIL;
		(*ppAlgo) = 0;

		EigenLearningAlgo * pAlgo = new EigenLearningAlgo();
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


//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
EigenLearningAlgo::EigenLearningAlgo()
	: bReady(false)
	, imageFunctions(64, 8)
	, allTotalHuman (0.0f, 256.0f, 256)
	, allTotalThings (0.0f, 256.0f, 256)
{
}


//-------------------------------------------------------------------------------------------------
/** \brief Destructor. */
//-------------------------------------------------------------------------------------------------
EigenLearningAlgo::~EigenLearningAlgo()
{
}

HRESULT EigenLearningAlgo::Initialize( csinterface::IInformationQuery * pInfo, 
									   csinterface::IDebugOutput * pDebugOutput) 
{
	this->pInfo = pInfo;
	this->pDebugOutput = pDebugOutput;
	return S_OK;
}

//-------------------------------------------------------------------------------------------------
/** \brief Function launches computational process.

  \param  startTime  the start time of surveillance process in seconds.
  \return            Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
HRESULT EigenLearningAlgo::Start( IN float startTime )
{
	HRESULT retValue = E_FAIL;
	try
	{
		startTime;
		csinterface::TBackground   bkgrnd;
		csinterface::TEmptyHallArr emptyHall;

		pInfo->GetData(&bkgrnd);
		fBackground = bkgrnd.data;
		pInfo->GetData(&emptyHall);
		emptyHallChairArr = emptyHall.data;

		int numOfChairs = (int)emptyHallChairArr.size();
		
		//corrImages.resize(numOfChairs);

		
		momentsListHuman.clear();
		momentsListCloth.clear();
		
		corrImages.clear();
		for (int i = 0; i < numOfChairs; i++)
		{
			std::pair < ChairContents, EigAlgoImageHolder> oneElem;
			oneElem.first = CHAIR_UNDEFINED;
			corrImages.push_back(oneElem);			
		}

		OpenOutputStream();

		HRESULT startResult = S_OK;

		if (startResult == S_OK) 
		{
			csinterface::TMsgIAmReady msg;
			pInfo->SetData(&msg);
			bReady = true;
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

		behaviourArray.clear();
		for (int i = 0; i < numOfChairs; i++)
		{
			std::pair < ChairContents, DynamicChairBehaviour > oneElem;
			oneElem.first = CHAIR_UNDEFINED;
			behaviourArray.push_back(oneElem);			
		}


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
HRESULT EigenLearningAlgo::ProcessImage( IN const BITMAPINFO * pBI,
                                         IN const __int8     * pImage,
                                         IN float              timeStamp )
{
	HRESULT retValue = E_FAIL;

	try
	{
		timeStamp;
		csutility::CopyDIBToColorImage(pBI, (LPCUBYTE)pImage, &testImage);
		csutility::CopyDIBToGrayImage(pBI,  (LPCUBYTE)pImage, &byteFrame);
		csutility::CopyDIBToFloatImage(pBI, (LPCUBYTE)pImage, &floatImage);


		// get information about chair states
		csinterface::TChairContentsMap chairContentsMap;
		pInfo->GetData(&chairContentsMap);


		for (int i = 0; i < (int)corrImages.size(); i++)
		{
			long index = emptyHallChairArr[i].index;
			ChairContents answerCC = chairContentsMap.data.find(index)->second; 
			if (chairContentsMap.data.find(index) == chairContentsMap.data.end())
			{
				//VERIFY(chairContentsMap.data.find(index) != chairContentsMap.data.end());
				continue;
			}

			switch(answerCC) {
				case CHAIR_EMPTY:
					break;
				case CHAIR_HUMAN:
					break;
				case CHAIR_CLOTHES:
					break;
				default:
					continue;
			}

			// here we obtain curve of the chair and then image of the chair
			Vec2fArr curve = emptyHallChairArr[i].curve;
		

			Arr2f chairImage;
			imageFunctions.GetSquareImageFromCurve(byteFrame, curve, chairImage);

			if (answerCC != corrImages[i].first)
			{
				if (frame != 0)
				{
			     	PutChairToStream(i, frame);
					corrImages[i].second.Clear();
					behaviourArray[i].second.Initialize(true);
				}

				corrImages[i].second.SetFirstImage(chairImage);				
				corrImages[i].first = answerCC;
				behaviourArray[i].first = answerCC;
			}

			if ( frame % 300 == 0 && frame > 1)
			{
				corrImages[i].second.UpdateHisto();
			}

			Arr2f loResImage;
			imageFunctions.GetLowResolutionImage(chairImage, loResImage);
			imageFunctions.NormalizeImageBrightness(loResImage);
			corrImages[i].second.AddImage(loResImage);

			
			behaviourArray[i].second.Update( floatImage, emptyHallChairArrEx[i]);


			{
				// test code for output, should be deleted
				if ( frame % 50 == 0 && frame > 1 && behaviourArray[i].first == CHAIR_HUMAN )
				{
					float d;
					behaviourArray[i].second.GetDynamicCharacteristic(d);
					tmpfile << d << std::endl;
				}
			}



			// draw frame number
			char str[25];
			ltoa(frame, str, 10);
			csutility::DrawDigits5x10(str, 50, 25, true, &testImage, RGB(255,255,255), RGB(0,0,0));

			DrawCurveWithContents(curve, answerCC, testImage);
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
void EigenLearningAlgo::DrawCurveWithContents(const Vec2fArr& curve, const ChairContents content, QImage& image)
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

//-------------------------------------------------------------------------------------------------
/** \brief Function finalizes computational process.

  \param  stopTime  the stop time of surveillance process in seconds.
  \return           Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
HRESULT EigenLearningAlgo::Stop( IN float stopTime, bool saveResult )
{
	stopTime;
	if (saveResult)
		SaveFinalData();

	CloseOutputStream();

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
/** \brief Function releases the implementation of this interface.

  \return  Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
HRESULT EigenLearningAlgo::Release()
{
  delete this;
  return S_OK;
}


void EigenLearningAlgo::AddToTotalHisto(int i)
{
	csutility::MultiHistogram <float, 2> tmp(0.0f, 256.0f, 256);
	int cnt;
	corrImages[i].second.GetHisto(tmp, cnt);
	
	MeanVarAccum < float > tmpAccum;

	corrImages[i].second.GetAverages(tmpAccum);	
	
	if (cnt > 1)
	{
		if (corrImages[i].first == CHAIR_HUMAN)
		{
			allTotalHuman += tmp;
			humanAccum += tmpAccum;
			corrImages[i].second.SpliceMomentsListToThisList(momentsListHuman);
		}
		else
		{
			allTotalThings += tmp;
			clothAccum += tmpAccum;
			corrImages[i].second.SpliceMomentsListToThisList(momentsListCloth);
		}
	}
}
HRESULT EigenLearningAlgo::SaveFinalData()
{
	USES_CONVERSION;
	
	for (int i = 0; i < (int)corrImages.size(); i++)
	{
		PutChairToStream(i, frame);
	}
	CString s;
	s.Format(_T("%d"), i);
	OutputDebugString(s);

	//histosaving

	for (int i = 0; i < (int)corrImages.size(); i++)
	{
		AddToTotalHisto(i);
	}

	// Save not normalized (temporary) histograms
	infile.clear();
	infile.open(  W2CA(fileNameHumanTmp.c_str()),  std::ios_base::out | std::ios_base::trunc);
	allTotalHuman.Save(infile);
	infile.close();

	infile.clear();
	infile.open(  W2CA(fileNameThingsTmp.c_str()),  std::ios_base::out | std::ios_base::trunc);
	allTotalThings.Save(infile);
	infile.close();

	// save mean and var counters
	infile.clear();
	infile.open(  W2CA(fileNameMeanVarCountHuman.c_str()),  std::ios_base::out | std::ios_base::trunc);
	humanAccum.Save(infile);
	infile.close();

	infile.clear();
	infile.open(  W2CA(fileNameMeanVarCountCloth.c_str()),  std::ios_base::out | std::ios_base::trunc);
	clothAccum.Save(infile);
	infile.close();

	// Save normalized histograms

	allTotalHuman.Normilize();
	allTotalThings.Normilize();


	csutility::MultiHistogram <float, 2> parameterA (0.0f, 256.0f, 256);
	csutility::MultiHistogram <float, 2> parameterB (0.0f, 256.0f, 256);
	std::vector < float > tmp;

	//here swap layers
	allTotalHuman.GetLayer(0, tmp);
	parameterA.SetLayer(0, tmp);
	allTotalThings.GetLayer(0, tmp);
	parameterA.SetLayer(1, tmp);

	allTotalHuman.GetLayer(1, tmp);
	parameterB.SetLayer(0, tmp);
	allTotalThings.GetLayer(1, tmp);
	parameterB.SetLayer(1, tmp);

	// save result
	std::wstring outfileNameParamA = resFilename + L"\\ParamA.txt";
	std::wstring outfileNameParamB = resFilename + L"\\ParamB.txt";
	

	const char * labelParamA[2] = {"paramter A human", "paramter A things"};
	
	double cmean;
	double cdev;
	clothAccum.statistics(&cmean, 0, &cdev);
	
	double hmean;
	double hdev;
	humanAccum.statistics(&hmean, 0, &hdev);

	//"(x - %f)*(x - %f)"
	char expr[500];
	std::string formula = "exp(log(10)*(x*4/255-5))/64*log(10)*exp(- (exp(log(10)*(x*4/255-5)) - %1.16f) * (exp(log(10)*(x*4/255-5)) - %1.16f)/ %1.16f) / %1.16f, exp(log(10)*(x*4/255-5))/64*log(10)*exp(- (exp(log(10)*(x*4/255-5)) - %1.16f) * (exp(log(10)*(x*4/255-5)) - %1.16f)/ %1.16f) / %1.16f";

	sprintf(expr, formula.c_str(), cmean, cmean, 2.0 * cdev * cdev, std::sqrt(6.283185307179586)*cdev, hmean, hmean, 2.0 * hdev * hdev, std::sqrt(6.283185307179586)*hdev );
	parameterA.SaveInGNUPlotFormat( W2CA(outfileNameParamA.c_str()), labelParamA , expr);
	const char * labelParamB[2] = {"paramter B human", "paramter B things"};
	parameterB.SaveInGNUPlotFormat( W2CA(outfileNameParamB.c_str()), labelParamB );

	// list with all intermediate results

	{
	
		std::wstring dynParamFileName = resFilename + _T("\\dynParamFileHuman.txt");
		dynParamFile.clear();
		dynParamFile.open( W2CA(dynParamFileName.c_str() ), std::ios_base::out | std::ios_base::app );
	
		for (std::list <float>::iterator i = momentsListHuman.begin(); i !=  momentsListHuman.end(); i++ )	
		{
			dynParamFile << (*i) << std::endl;
		}
		dynParamFile.close();
	}
	
	{
		std::wstring dynParamFileName = resFilename + _T("\\dynParamFileCloth.txt");
		dynParamFile.clear();
		dynParamFile.open( W2CA(dynParamFileName.c_str() ), std::ios_base::out | std::ios_base::app );

		for (std::list <float>::iterator i = momentsListCloth.begin(); i !=  momentsListCloth.end(); i++ )
		{
			dynParamFile << (*i) << std::endl;
		}
		dynParamFile.close();
	}
	

	tmpfile.close();

	return S_OK;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function uploads specified data to this object.

  \param  pData  pointer to the input data.
  \return        Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
HRESULT EigenLearningAlgo::SetData( IN const csinterface::IDataType * pData )
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
	case csinterface::DATAID_RESULT_FILENAME:
		{
			csinterface::TResultFilename * pFn =  (csinterface::TResultFilename *)pData;
			resFilename = pFn->data;
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
HRESULT EigenLearningAlgo::GetData( OUT csinterface::IDataType * pData ) const
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
HRESULT EigenLearningAlgo::QueryData( IN OUT csinterface::IDataType * pData ) const
{
	pData;
	return S_OK;
}

/* this class specific */
void EigenLearningAlgo::OpenOutputStream() 
{
	USES_CONVERSION;
	std::wstring fname = resFilename + _T("\\eigvals.html");
	file.open( W2CA(fname.c_str() ), std::ios_base::out | std::ios_base::trunc );
	file << L"<html><body>\n";
	file << L"<table border = 0>\n";

	fileNameHumanTmp = resFilename + L"\\dataHumanTmp.txt";
	fileNameThingsTmp = resFilename + L"\\dataThingTmp.txt";

	//here we open old histo and update it
	infile.clear();
	infile.open(  W2CA(fileNameHumanTmp.c_str()), std::ios_base::in );
	allTotalHuman.Load(infile);
	infile.close();

	infile.clear();
	infile.open(  W2CA(fileNameThingsTmp.c_str()), std::ios_base::in );
	allTotalThings.Load(infile);
	infile.close();

	fileNameMeanVarCountCloth = resFilename + L"\\meanVarCountCloth.txt";
	infile.clear();
	infile.open(  W2CA(fileNameMeanVarCountCloth.c_str()), std::ios_base::in );
	ALIB_ASSERT( clothAccum.Load(infile) );
	infile.close();

	fileNameMeanVarCountHuman = resFilename + L"\\meanVarCountHuman.txt";
    infile.clear();
	infile.open(  W2CA(fileNameMeanVarCountHuman.c_str()), std::ios_base::in );
	ALIB_ASSERT( humanAccum.Load(infile) );
	infile.close();

	std::wstring strtmp = resFilename + L"\\humanTemp.txt";

	tmpfile.clear();
	tmpfile.open(  W2CA(strtmp.c_str()), std::ios_base::out | std::ios_base::trunc );
}


/* this class specific */
void EigenLearningAlgo::PutChairToStream( int index, int startFrame ) 
{
	std::vector <float> ev;
	corrImages[index].second.GetEigenValues(ev);

	file << L"<tr><td>";
	//write image of the chair 
	
	std::wstring fname;
	WCHAR num[10];
	_itow(index, num, 10);

	WCHAR fnum[10];
	_itow(frame, fnum, 10);

	fname = resFilename + std::wstring(L"\\images\\pic_") + fnum + L"_"+ std::wstring(num) + L".bmp";

	file << L"<img src = ";
	file << fname;
	file << L" width = 100 height = 100>";


	Arr2f img = corrImages[index].second.GetFirstImage();
	Arr2ub res;
	MyFloatImageToByteImage(img, res, 1, false);
	SaveByteImage( fname.c_str(), res, INVERT_AXIS_Y, false);

	QImage evPlot;
	GetPlotOfEigenValues(ev, evPlot);
	std::wstring plotfname = resFilename + std::wstring(L"\\images\\plotpic_") + fnum + L"_" + std::wstring(num) + L".bmp";
	SaveFloatImage(plotfname.c_str(), reinterpret_cast<Arr2f &>(evPlot), INVERT_AXIS_Y);

	file << L"<img src = ";
	file << plotfname;
	file << L" width = 500 height = 100>";
	file << L"</td></tr>";
	file << L"<tr><td>\n";

	WCHAR sIndex[10];
	_itow(index, sIndex, 10);
	file << std::wstring(sIndex) << ",     ";
	_itow(startFrame, sIndex, 10);
	file << std::wstring(sIndex) << " ";


	switch(corrImages[index].first) {
	case CHAIR_EMPTY:
		file << L"<b>EMPTY</b>";
		//OutputDebugString(_T("Empty"));
		break;
	case CHAIR_HUMAN:
		file << L"<b>HUMAN</b>";
		//OutputDebugString(_T("Human"));
		break;
	case CHAIR_CLOTHES:
		file << L"<b>CLOTHES</b>";
		//OutputDebugString(_T("Clothes"));
		break;
	default:
		return;
	}

	SpectrumApproximator <float> myAppoximator (ev);
	float a, b;
	myAppoximator.GetApproximationParams(a, b);

	file << L"<br>a = " << a << L" b = " << b;

	float m1;
	float m2;
	myAppoximator.GetMoments(m1, m2);
	file << L"<br>average = " << m1 << L" deviation = " << m2;

	
	file << L"<table border = 1 width = 100% cellpadding = 0 cellspacing = 1 >\n";
	file << L"<tr>";
	for (int i = 0; i < (int)ev.size(); i++)
	{
		file << L"<td><b>"<< i << L"</b></td>";
	}
	file << L"</tr>";
	file << L"<tr>";
	for (int i = 0; i < (int)ev.size(); i++)
	{
		file << L"<td><Font size=-1><b>" << std::sqrt(std::max <float> (0.0f, ev[i])) << L"</Font></b></td>";
	}
	file << L"</tr>";
	file << L"</table>";

	file << L"<br>";
	file << L"<br>";
	file << L"<br>";
	file << L"\n</td></tr>";


	//////////////////////////////////////////////////////////////////////////
	//add values to the total histogram

	AddToTotalHisto( index );

}

void EigenLearningAlgo::CloseOutputStream() 
{
	file << L"</table>";
	file << L"</body></html>";
	file.close();
}

void EigenLearningAlgo::GetPlotOfEigenValues(std::vector<float> ev, QImage& img)
{
	RGBQUAD color;

	color.rgbRed = 20;
	color.rgbGreen = 25;
	color.rgbBlue = 255;
	color.rgbReserved = 0;

	RGBQUAD color2;

	color2.rgbRed = 255;
	color2.rgbGreen = 0;
	color2.rgbBlue = 0;
	color2.rgbReserved = 0;

	int height = 255 ;
	img.resize(height, (int)ev.size(), color);


	std::vector < int > plotValues;

	for (int i = 0; i < (int)ev.size(); i++)
	{
		ev[i] = std::sqrt((std::max<float>(0.0f, ev[i])));
		plotValues.push_back(std::min <int>(height, (int) ((float)height * ev[i])));
	}

	color.rgbRed = 55;
	color.rgbGreen = 235;
	color.rgbBlue = 25;

	SpectrumApproximator <float> myAppoximator (ev);
	float a, b;
    myAppoximator.GetApproximationParams(a, b);

	for (int w = 0; w < (int)ev.size(); w++)
		for (int h = 0; h < plotValues[w]; h++)
		{
			img(h, w) = color;
			img((int)((float)height * myAppoximator.f(a, b, (float)w)), w) = color2;			
		}
}