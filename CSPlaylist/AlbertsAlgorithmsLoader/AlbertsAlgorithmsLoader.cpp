#include "stdafx.h"
#include "AlbertsAlgorithmsLoader.h"
#include "../../CSChair/chairSerializer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

HRESULT AlbertsAlgorithmsLoader::LoadOneItem(const PlaylistItem& item)
{
	curItem = item;
	myParameters.item = curItem;

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
		//long camID = GetCamID(item.video);

		std::wstring videoFileName;
        //if (ExtractGoodChairs(emptyHallChairArr, camID) == true)
		{
			if (curItem.path == _T(""))
			{
				//this code from Kostik
				TCHAR stFile[MAX_PATH];
				TCHAR stCurDir[MAX_PATH];
				GetCurrentDirectory(MAX_PATH, stCurDir);
				int nLen = lstrlen(stCurDir);
				if(nLen>0 && stCurDir[nLen-1] == TEXT('\\'))
					stCurDir[nLen-1] = 0;
				wsprintf(stFile, TEXT("%s\\%s"), stCurDir, curItem.video.c_str());
				// end code from Kostik
				videoFileName = std::wstring(stFile);
			}
			else
			{			
				videoFileName = curItem.path + _T("\\") + curItem.video;
			}
            			
			resultFI = pFrameIterator->Initialize(videoFileName, curItem.marking, emptyHallChairArr);
		}
//		else
//			resultFI = E_FAIL;

		if (resultFI != S_OK) 
		{
			msg = msg + _T("Cannot initialize video or marking file\n may be error in files or files missing?: ") + videoFileName+ _T("\n") + curItem.marking + _T("\n");
			pFrameIterator->Release();
			pFrameIterator = NULL;			
		}

		// load background
		bool resultBG = csutility::LoadFloatImageEx(curItem.background.c_str(), &fBackground, INVERT_AXIS_Y);
		if (!resultBG) 
		{	
			msg = msg +  _T("Cannot initialize background file\n may be error in file: ") + curItem.background;
		}

		if (resultEH != S_OK || resultFI != S_OK || !resultBG)
		{
			AfxMessageBox(msg.c_str());
			return E_FAIL;
		}
	}
	Sleep(1000);

			

	bDestroyed = false;
	bReady     = true;
	return S_OK;
}
HRESULT AlbertsAlgorithmsLoader::Start()
{
	if (!bReady)
		return E_FAIL; // check if we ready to start
	if (!bAlgorithmLoaded) 
		return E_FAIL; // check if algorithm is loaded

	bReady = false; // now we in Start() and not ready to start once again

	float deltat = 1000.0f / myParameters.simulated_fps;
	
	float time = 0;
    VERIFY(SUCCEEDED(pAlgorithm->Start(time)));

	bRunning = true;
	saveResult = true;
	while (!pFrameIterator->IsEnd() && bRunning)
	{
		pFrameIterator->GetFrameInfo(&chairContentsMap);
		pFrameIterator->GetFrameInfo(&image); // nCurFrame += 1
		if (FAILED(pAlgorithm->ProcessImage(image.data.pBI, (const __int8 *)image.data.bytes, time)))
			return E_FAIL;
		time += deltat;
	}
	VERIFY(SUCCEEDED(pAlgorithm->Stop(time, saveResult)));

	bRunning = false;
	Stop(); // this call for destroying FrameIterator
	return S_OK;	
}

HRESULT AlbertsAlgorithmsLoader::Stop(bool saveResult)
{  
	this->saveResult = saveResult;
	if (bRunning)
		bRunning = false; // force exiting from main cycele
	else if (!bDestroyed)
	{
		if(pFrameIterator) 
			pFrameIterator->Release();
		pFrameIterator = NULL;
		bDestroyed = true;
	}
	return S_OK;
}

bool AlbertsAlgorithmsLoader::IsReady()
{
	return bReady;
}
HRESULT AlbertsAlgorithmsLoader::Release()
{
	ASSERT(bRunning == false);
	delete this;
	return S_OK;
}

UINT AlbertsAlgorithmsLoader::id()
{
	return 2; // Alberts algorithms
}
HRESULT AlbertsAlgorithmsLoader::SetData(const csinterface::IDataType * pData)
{
	switch( pData->type() )
	{
	case csinterface::DATAID_PLEARN_ALGORITHM:
		{
			csinterface::TPLearnAlgorithm * pAlgo = (csinterface::TPLearnAlgorithm *) pData;
			pAlgorithm = pAlgo->data;
			bAlgorithmLoaded = true;
		}
		break;
	case csinterface::DATAID_MSG_IAMREADY:
		{
			bAlgorithmLoaded = true;
		}
		break;
	case csinterface::DATAID_MSG_SAVE_TEMP_DATA:
		{
			if(bAlgorithmLoaded)
			{
				pAlgorithm->SetData(pData);
				return S_OK;
			}
			else
				return E_FAIL;
		}
		break;
	case csinterface::DATAID_MSG_LOAD_TEMP_DATA:
		{
			if(bAlgorithmLoaded)
			{
				pAlgorithm->SetData(pData);
				return S_OK;
			}
			else
				return E_FAIL;
		}
		break;
	case csinterface::DATAID_PLAYLIST_OPTIONS:
		{
			csinterface::TPlaylistParameters * pParams = (csinterface::TPlaylistParameters *)pData;
			myParameters = pParams->data;
		}
		break;
	}
	return S_OK;
}
HRESULT AlbertsAlgorithmsLoader::GetData(csinterface::IDataType * pData) const
{
	switch( pData->type() )
	{
	case csinterface::DATAID_CHAIR_CONTENTS_MAP:
		{
			csinterface::TChairContentsMap * contents = (csinterface::TChairContentsMap *)pData;
			contents->data = chairContentsMap.data;
			return S_OK;
		}
		break;
	case csinterface::DATAID_BACKGROUND:
		{
			csinterface::TBackground *bkgrnd = (csinterface::TBackground *)pData;
			bkgrnd->data = fBackground;
			return S_OK;
		}
		break;
	case csinterface::DATAID_EMPTYHALL_CHAIRSET:
		{
			csinterface::TEmptyHallArr *emptyHall = (csinterface::TEmptyHallArr *)pData;
			emptyHall->data = emptyHallChairArr;
			return S_OK;
		}
		break;
	case csinterface::DATAID_PLAYLIST_OPTIONS:
		{
			csinterface::TPlaylistParameters * pParams =  (csinterface::TPlaylistParameters *) pData;
			pParams->data = myParameters;
		}  
		break;

	case csinterface::DATAID_QIMAGE:
		{
			if (bRunning) 
			{
				// very simpified version, several check-ups omited
				csinterface::TQImage qimage;
				pAlgorithm->GetData(&qimage);
				csinterface::TQImage * pImageOut = (csinterface::TQImage *) pData;
				pImageOut->data = qimage.data;
				return S_OK;		
			}
			else
				return E_FAIL;
		}
		break;
	case csinterface::DATAID_FRAMEITERATOR_PROGRESS:
		{
			if (bRunning)
			{
				pFrameIterator->GetFrameInfo(pData);
			}
			else
			{
				return E_FAIL;
			}
		}  
		break;
	default:
		return E_FAIL;
	}
	return S_OK;
}


long AlbertsAlgorithmsLoader::GetCamID(std::wstring videoFileName)
{
	long sp = (long)videoFileName.find_first_of(_T("_"));

	for (long k = 0; k  < 2; k++ )
	{
		sp = (long)videoFileName.find_first_of(_T("_"), sp + 1);
	}
	std::wstring del = _T("cam"); //this will be deleted
	long cnt = (long)videoFileName.find_first_of(_T("_"), sp + 1) - sp;

	std::wstring strID = videoFileName.substr(sp + del.length() + 1, cnt - del.length() - 1);
	long camID = _wtol(strID.data());
	return camID;
}
bool AlbertsAlgorithmsLoader::ExtractGoodChairs(BaseChairArr & chairArr, long camereNo)
{
	BaseChairArr tmp;
	for (BaseChairArr::iterator i = chairArr.begin(); i != chairArr.end(); i++)
	{
		if ((*i).cameraNo == camereNo) tmp.push_back(*i);
	}
	chairArr = tmp;
	return !(chairArr.empty());
}


HRESULT AlbertsAlgorithmsLoader::QueryData(csinterface::IDataType * pData) const
{
	pData;
/*	switch( pData->type() )
	{
	
	}*/
	return S_OK;
}
