#include "stdafx.h"
#include "CSHistogramPreparer.h"
#include "../../CSChair/chairSerializer.h"
#include "../MyDebugOutput.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

HRESULT CSHistogramPreparer::LoadOneItem(const PlaylistItem& item)
{
	curItem = item;
	
	if(pFrameIterator) pFrameIterator->Release();
	if(pCameraAnalyzer)
	{
		pCameraAnalyzer->Stop(0);
		pCameraAnalyzer->Release();
	}

	// section: initializations
	{
		// load empty hall chairs
		HRESULT resultEH = LoadGaugeHallFromFile(curItem.emptyHall.c_str(), emptyHallChairArr);
		if (resultEH != S_OK) return resultEH;

		// create and initialize FrameIterator
		CreatePlayListFrameIterator(&pFrameIterator);
		HRESULT resultFI = pFrameIterator->Initialize(curItem.video, curItem.marking, emptyHallChairArr);
		if (resultFI != S_OK) return resultFI;

		// load background
		bool    resultBG = LoadFloatImage(curItem.background.c_str(), fBackground, INVERT_AXIS_Y);
		if (!resultBG) return E_FAIL;

		// create and initialize CameraAnalyzer
		HRESULT resultCA = InitCamAnalyzer(&pCameraAnalyzer);		
		if (resultCA != S_OK) return resultCA;

		//wait some time
		Sleep(1000);

	}
	return S_OK;
}
HRESULT CSHistogramPreparer::Start()
{
	statHist.Clear();
	std::vector< csinterface::HistoPair > histoArr;
	histoArr.resize(emptyHallChairArr.size());
	for (size_t i = 0; i < histoArr.size(); i++) 
	{
		histoArr[i].second.Initialize(0.0f, 2.0f, 256);
		histoArr[i].first = csinterface::CHAIR_UNDEFINED;
	}
	
	// start main cycle
	__int64 curTime = 0;
    
	HRESULT startResult = pCameraAnalyzer->Start(curTime);

	bReady = true;
	bRunning = true;

	while (!pFrameIterator->IsEnd() && bReady)
	{		
		csinterface::TChairContentsMap chairContentsMap;
		pFrameIterator->GetFrameInfo(&chairContentsMap);

		csinterface::TFrameIteratorVideo image;
		pFrameIterator->GetFrameInfo(&image); // nCurFrame += 1
		
		HRESULT res = pCameraAnalyzer->ProcessImage(image.data.pBI, image.data.bytes, curTime);
		
		csutility::GrayImageToQImage(image.data.pBI, image.data.bytes, &testImage);
		
		if( res == S_OK )
		{			
			// from cam analyzer
			csalgo::TChairLearnArr chairLearnArr;
			if (FAILED(pCameraAnalyzer->GetData( &chairLearnArr ))) 
				AfxMessageBox(_T("error in pCameraAnalyzer->GetData()"));
			// save to histogram
			for (size_t i = 0; i < chairLearnArr.data.size(); i++) // go through chairs and save data to the total histogram
			{
				long index = chairLearnArr.data[i].index;
				//////////////////////////////////////////////////////////////////////////
				//////////////////////////////////////////////////////////////////////////
				//////////////////////////////////////////////////////////////////////////
				ChairContents answer = chairContentsMap.data.find(index)->second; 
				//VERIFY(chairContentsMap.data.find(index) != chairContentsMap.data.end());
				//////////////////////////////////////////////////////////////////////////
				//////////////////////////////////////////////////////////////////////////
				//////////////////////////////////////////////////////////////////////////
				

				if (histoArr[i].first != answer)
				{
					histoArr[i].second.Normilize();
					statHist = statHist + histoArr[i].second;
					histoArr[i].second.Clear();
					histoArr[i].first = (csinterface::ChairContents)answer;
				}
			
				switch( answer )
				{
				case csinterface::CHAIR_EMPTY:
					//statHist.push_value(chairLearnArr.data[i].statNCC, CinemaHistogram<float>::LayerName::EMPTY);
					histoArr[i].second.push_value(chairLearnArr.data[i].statNCC, CinemaHistogram<float>::LayerName::EMPTY);
					break;
				case csinterface::CHAIR_HUMAN:

					//statHist.push_value(chairLearnArr.data[i].statNCC, CinemaHistogram<float>::LayerName::HUMAN);
					histoArr[i].second.push_value(chairLearnArr.data[i].statNCC, CinemaHistogram<float>::LayerName::HUMAN);

					//if (chairLearnArr.data[i].statNCC < 30.0f/256.0f*2.0f)
					//	AfxMessageBox(curItem.video.c_str());
					break;
				case csinterface::CHAIR_CLOTHES:
					//statHist.push_value(chairLearnArr.data[i].statNCC, CinemaHistogram<float>::LayerName::CLOTH);
					histoArr[i].second.push_value(chairLearnArr.data[i].statNCC, CinemaHistogram<float>::LayerName::CLOTH);
					break;
				}			
			}
		}
		curTime += 50; /////Don't forget about the time!///////////////////////////////////////
	}
	bRunning = false;


	if(pFrameIterator) pFrameIterator->Release();
	if(pCameraAnalyzer)
	{
		pCameraAnalyzer->Stop(0);
		pCameraAnalyzer->Release();
	}

	for (size_t i = 0; i < histoArr.size(); i++) 
	{
		histoArr[i].second.Normilize(); 
		statHist = statHist + histoArr[i].second;
	}
	
	//statHist.Normilize();
	return S_OK;	
};

HRESULT CSHistogramPreparer::Stop()
{  
	bReady = false;	
	return S_OK;
};

bool CSHistogramPreparer::IsReady()
{
	return bReady;
}
HRESULT CSHistogramPreparer::Release()
{
	delete this;
	return S_OK;
}
// Properties of DataManager
HRESULT  CSHistogramPreparer::SetData(const csinterface::IDataType * pData)
{ 
	return S_OK; 
};
HRESULT  CSHistogramPreparer::GetData(csinterface::IDataType * pData) const
{ 
	switch( pData->type() )
	{
	case csinterface::DATAID_QIMAGE:
		{
			if (bReady == true)
			{
				csinterface::TDemoImage demoImage;
				HRESULT res = pCameraAnalyzer->GetData(&demoImage);
				if (res == S_OK) OutputDebugString(_T("It works!"));
		
				csinterface::TQImage * pHistImage = dynamic_cast<csinterface::TQImage *> (pData);
				VERIFY(pHistImage != NULL);
				pHistImage->data = demoImage.data.second;
				
				//pHistImage->data = testImage;

			//CString s;
			//s.Format(_T("%d %d %d"), demoImage.image.width(), demoImage.image.height(), res);
			//AfxMessageBox(s);
			}
			else
			{
				return E_FAIL;
			}
		}
    	break;
	case csinterface::DATAID_FRAMEITERATOR_PROGRESS:
		{
			if (bReady == true)
			{
				pFrameIterator->GetFrameInfo(pData);
			}
			else
			{
				return E_FAIL;
			}
		}  
		break;
	case csinterface::DATAID_HISTOGRAM_PSTATHIST:
		{
			const csinterface::TPStaticHistogram* ppHist = (csinterface::TPStaticHistogram* ) (pData);
			(*ppHist->data) = statHist;
		}
	}
	return E_FAIL;
};

// Initializes camera analyzer
HRESULT CSHistogramPreparer::InitCamAnalyzer(ICameraAnalyzer** ppCameraAnalyzer)
{
	Int8Arr binParams;
	SetDefaultParameters(&binParams, NULL);

	pDebugOutput = new MyDebugOutput;	

	//TO DO: // show parameters dialog!!!
	HRESULT res = CreateCameraAnalyzer(ppCameraAnalyzer, &binParams, &emptyHallChairArr, &fBackground, emptyHallChairArr[0].cameraNo, pDebugOutput);
	return res;
}
UINT CSHistogramPreparer::id()
{
	return 1; // histogram preparer
}
