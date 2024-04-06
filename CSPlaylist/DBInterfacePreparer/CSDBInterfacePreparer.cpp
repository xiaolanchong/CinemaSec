#include "stdafx.h"
#include "CSDBInterfacePreparer.h"
#include "../PlayListFrameIterator.h"
#include "../../CSChair/ChairSerializer.h"
#include "../CSDataPrepareGlobals.h"
#include "AlgoDBInterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

HRESULT CSDBInterfacePreparer::GetCurveByID(long index, Vec2fArr& curve)
{
	for (size_t i = 0; i < emptyHallChairArr.size(); i++)
		if (index == emptyHallChairArr[i].index)
		{
			curve = emptyHallChairArr[i].curve;
			return S_OK;
		}
	return E_FAIL;
}

HRESULT CSDBInterfacePreparer::LoadOneItem(const PlaylistItem& item)
{
	curItem = item;
	
	if(pFrameIterator) 
	{
		pFrameIterator->Release();
		pFrameIterator = 0;
	}
	if(pCameraAnalyzer)
	{
		pCameraAnalyzer->Stop(0);
		pCameraAnalyzer->Release();
		pCameraAnalyzer = 0;
	}

	// section: initializations
	{
		std::tstring msg; // message to output in case of error
		// load empty hall chairs
		HRESULT resultEH = LoadGaugeHallFromFile(curItem.chairs.c_str(), emptyHallChairArr);
		if (resultEH != S_OK) 
		{
			msg = msg + _T("Cannot initialize empty hall marking\n may be error in file or file's missing?: ") + curItem.chairs + _T("\n");
		}

		// create and initialize FrameIterator
		HRESULT resultFI = CreatePlayListFrameIterator(&pFrameIterator);
		resultFI = pFrameIterator->Initialize(curItem.video, curItem.marking, emptyHallChairArr);

		if (resultFI != S_OK) 
		{
			msg = msg + _T("Cannot initialize video or marking file\n may be error in files or files missing?: ") + curItem.video + _T("\n") + curItem.marking + _T("\n");
			pFrameIterator->Release();
			pFrameIterator = NULL;			
		}

		// load background
		bool resultBG = csutility::LoadFloatImageEx(curItem.background.c_str(), &fBackground, INVERT_AXIS_Y);
		if (!resultBG) 
		{	
			msg = msg +  _T("Cannot initialize background file\n may be error in file: ") + curItem.background;
		}

		// create and initialize CameraAnalyzer
		HRESULT resultCA = InitCamAnalyzer(&pCameraAnalyzer);		
		if (resultCA != S_OK) 
		{
			msg = msg +  _T("Cannot initialize camera analyzer ");
		}

		if (resultEH != S_OK || resultFI != S_OK || !resultBG || resultCA != S_OK)
		{
			AfxMessageBox(msg.c_str());
			return E_FAIL;
		}
	}
	Sleep(1000); // Let fuckin DirectX up and go
	bReady = true;
	return S_OK;
}

HRESULT CSDBInterfacePreparer::DrawContentAndCurves(csinterface::TChairContentsMap& chairContentsMap, csalgo::TChairLearnArr& chairLearnArr, QImage& img)
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

HRESULT CSDBInterfacePreparer::Start()
{
	if (bReady == false)
		return E_FAIL;
	else
		bReady = false; // we are not ready to start once again

	/// open file for writing
	HANDLE hFile;
	DWORD dwBytesWritten;
	std::tstring outfilename = curItem.video + _T("out.adb");
	hFile = CreateFile(outfilename.c_str(), // file name 
		GENERIC_READ | GENERIC_WRITE,		// open for reading 
		0,					                // do not share 
		NULL,					            // no security 
		CREATE_ALWAYS,				       
		FILE_ATTRIBUTE_NORMAL,			    // normal file 
		NULL);                         

	///here we write some header record
	AlgoDBFileHeader hdr;

	//	hdr.Description = "";
	hdr.HeaderLen = sizeof(AlgoDBFileHeader);
	hdr.id = 3;
	CopyMemory((char*)(hdr.label), "ALGO",4);
	hdr.NumOfRecords = 0;
	hdr.UserFlags = 0;
	WriteFile(hFile, &hdr, sizeof(AlgoDBFileHeader), &dwBytesWritten, NULL); 

	long recordCount = 0;

	// start main cycle
	__int64 curTime = 0;
	HRESULT startResult = pCameraAnalyzer->Start(curTime);

	Arr2ub curByteFrame;
	int nFrame = 0;

	// DataPrepare helper class
	CSDataPrepareGlobals dataPreparer;
	
	bRunning = true;	
	while (!pFrameIterator->IsEnd() && bRunning)
	{	
		csinterface::TChairContentsMap chairContentsMap;
		pFrameIterator->GetFrameInfo(&chairContentsMap);

		csinterface::TFrameIteratorVideo image;
		pFrameIterator->GetFrameInfo(&image); // nCurFrame += 1
		
		HRESULT res = pCameraAnalyzer->ProcessImage(image.data.pBI, image.data.bytes, curTime);
		
		csutility::CopyDIBToColorImage(image.data.pBI, image.data.bytes, &testImage );
		csutility::CopyDIBToGrayImage(image.data.pBI, image.data.bytes, &curByteFrame);
		
		// from cam analyzer
		csalgo::TChairLearnArr chairLearnArr;
		
		char str[255];
		itoa(nFrame, str, 10);
		csutility::DrawDigits5x10(str, 50, 25, true, &testImage, RGB(255,255,255), RGB(0,0,0));

		if (res == S_OK)
		{
			if (FAILED(pCameraAnalyzer->GetData( &chairLearnArr ))) 
				AfxMessageBox(_T("error in pCameraAnalyzer->GetData()"));
			DrawContentAndCurves(chairContentsMap, chairLearnArr, testImage);
		}
		
		if( res == S_OK && nFrame % (nSkip + 1) == 0 )
		{			
			for (size_t i = 0; i < chairLearnArr.data.size(); i++) // go through chairs and save data to the total histogram
			{
				long index = chairLearnArr.data[i].index;

				DataBlockInRecord record;

				record.s_correlation = chairLearnArr.data[i].statNCC;
				record.d_correlation = chairLearnArr.data[i].dynaNCC;

				ChairContents answer = chairContentsMap.data.find(index)->second; 
				
				switch( answer )
				{
				case CHAIR_EMPTY:
					record.answer = 0;
					break;
				case CHAIR_HUMAN:
					record.answer = 1;
					break;
				case CHAIR_CLOTHES:
					record.answer = 2;
					break;
				default:
					continue;
				}

				record.recordVersionID = 3;
				Arr2f chairImage;
				Vec2fArr curve;
				VERIFY(GetCurveByID(index, curve) == S_OK);

				dataPreparer.GetSquareImageFromCurve(curByteFrame, curve,  chairImage);
				//CSDataPrepareGlobals::GetSquareImageFromCurve(curByteFrame, curve,  chairImage);

				/*	
				if  (answer == CHAIR_EMPTY && record.s_correlation > 0.5)
				{
					CString s;
					s.Format(_T("index %d x i %d x Frame number %d x correlation %f.bmp"), index, i, nFrame, record.s_correlation);
					OutputDebugString(s);
					Arr2ub bi;
					MyFloatImageToByteImage(chairImage, bi);
			//		SaveByteImage(s, bi, INVERT_AXIS_Y);
				}
				*/


				// fill newDataBlock.image
				int m, n;
				for (m = 0; m < chairImage.width(); m++)
					for (n = 0; n < chairImage.height(); n++)
					{
							record.image[n + dataPreparer.GetPictureSide() * m] = (unsigned char) chairImage( n,  chairImage.height() - m - 1);
					}

				// fill newDataBlock.levels_image
				Arr2f levels_image = dataPreparer.GetLevelsNoParams(chairImage);

				for (m = 0; m < chairImage.width(); m++)
					for (n = 0; n < chairImage.height(); n++)
						record.levels_image[n + dataPreparer.GetPictureSide() * m] = (unsigned char) levels_image( n,  chairImage.height() - m - 1);

				std::vector <float> vWalshCoefs;
				dataPreparer.GetWalshCoefs(chairImage, vWalshCoefs);
				dataPreparer.NormalizeVector(vWalshCoefs, 1);

				for (m = 0; m < 16; m++)
					for (n = 0; n < 16; n++)
						record.walshComponents[n + m * 16] = (double) vWalshCoefs[n + m * dataPreparer.GetPictureSide()];

				std::vector <float> vAverage;
				dataPreparer.GetAverageVector(chairImage, vAverage);
				dataPreparer.NormalizeVector(vAverage, 1);
				for (m = 0; m < 256; m++) record.averageColors[m] = (double) vAverage[m];

				// now write to the file
				long recordLength = sizeof(DataBlockInRecord) + sizeof(long);
				WriteFile(hFile, &recordLength, sizeof(long), &dwBytesWritten, NULL); 
				WriteFile(hFile, &record, sizeof(DataBlockInRecord), &dwBytesWritten, NULL);
				recordCount++;
			}
		}
		curTime += 50; /////Don't forget about the time!///////////////////////////////////////
		nFrame++;

	}
	bRunning = false;

	//close file handle and writing normal header
	SetFilePointer(hFile, -(long)SetFilePointer(hFile, 0, NULL, FILE_BEGIN), NULL, FILE_CURRENT);
	hdr.NumOfRecords = recordCount;
	WriteFile(hFile, &hdr, sizeof(AlgoDBFileHeader), &dwBytesWritten, NULL); 
	CloseHandle(hFile);




	if(pFrameIterator) 
	{
		pFrameIterator->Release();
		pFrameIterator = 0;
	}
	if(pCameraAnalyzer)
	{
		pCameraAnalyzer->Stop(0);
		pCameraAnalyzer->Release();
		pCameraAnalyzer = 0;
	}
	return S_OK;	
}
//
//HRESULT Stop(bool saveResult = true);
//
HRESULT CSDBInterfacePreparer::Stop(bool saveResult)
{  
	saveResult;
	bRunning = false;	
	return S_OK;
}

bool CSDBInterfacePreparer::IsReady()
{
	return bReady;
}
HRESULT CSDBInterfacePreparer::Release()
{
	delete this;
	return S_OK;
}
// Properties of DataManager
HRESULT  CSDBInterfacePreparer::SetData(const csinterface::IDataType * pData)
{ 
	switch( pData->type() )
	{
	case csinterface::DATAID_SKIPFRAMES_NUMBER:
		csinterface::TSkipFramesNum* pNum =  (csinterface::TSkipFramesNum* )pData;
		nSkip = pNum->data;
		break;
	}

	return S_OK; 
}
HRESULT  CSDBInterfacePreparer::GetData(csinterface::IDataType * pData) const
{ 
	switch( pData->type() )
	{
	case csinterface::DATAID_QIMAGE:
		{
			if (bRunning == true)
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
    	break;
	case csinterface::DATAID_FRAMEITERATOR_PROGRESS:
		{
			if (bRunning == true)
			{
				pFrameIterator->GetFrameInfo(pData);
				return S_OK;
			}
			else
			{
				return E_FAIL;
			}
		}  
		break;
	}
	return E_FAIL;
}
// Initializes camera analyzer
HRESULT CSDBInterfacePreparer::InitCamAnalyzer(ICameraAnalyzer** ppCameraAnalyzer)
{
	Int8Arr binParams;
	VERIFY(!FAILED(SetDefaultParameters(&binParams, NULL)));
	
	//TO DO: // show parameters dialog!!!
	HRESULT res = CreateCameraAnalyzer(ppCameraAnalyzer, &binParams, &emptyHallChairArr, &fBackground, emptyHallChairArr[0].cameraNo, NULL);
	return res;
}
UINT CSDBInterfacePreparer::id()
{
	return 3; // DBInterfacePreparer
}


