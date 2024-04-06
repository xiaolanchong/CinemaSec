#include "stdafx.h"
#include "PlayListFrameIterator.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/*
interface IPlayListFrameIterator
{
	// initialize
	virtual HRESULT Initialize(const std::string& sVideoFileName, const std::string& sMarkingFile) = 0;
	// use this method if you want to restart the iterator
	virtual HRESULT Restart();
	// get to the next frame
	virtual HRESULT SeekNext();
	// check if it is the end
	virtual bool    IsEnd();
	//gets the info about chairs
	virtual HRESULT GetFrameInfo(csalgo::IDataType * pData); 
};

*/
HRESULT PlayListFrameIterator::Initialize(const std::tstring& sVideoFileName, const std::tstring& sMarkingFile, const BaseChairArr& emptyHallArr)
{
    sVideofile  = sVideoFileName;
	sMarkfile   = sMarkingFile;
	bool bPerFrame = true;
	bool bLoop     = false;
	
    
	HRESULT resultFG = CreateVideoFileGrabber( &pFrameGrabber, nBitsPerPixel, bPerFrame, bLoop);
	if ( resultFG == S_OK)
	{
        FrameGrabberParameters grabParams;
	
		if (!pFrameGrabber->Start(sVideoFileName.c_str(), grabParams))
		{	
			//may be videofile is not avi but avd.
			//try open avd
			BaseFrameGrabber * pAvdGrabber = csutility::CreateWrappedAVideoReader( sVideoFileName.c_str(), 0);

			if ( pAvdGrabber != 0 )
			{
				DeleteVideoFileGrabber( pFrameGrabber );
				pFrameGrabber = pAvdGrabber;
			}
			else
			{
				return E_FAIL;		
			}
		}
	}
	else
		return resultFG;
	
	HRESULT resultCC = pChairContent->Load(sMarkingFile.c_str(), emptyHallArr);
	if (resultCC != S_OK) return resultCC;

	nCurFrame = 0;
	return S_OK;
}

HRESULT PlayListFrameIterator::Restart()
{
	FrameGrabberParameters grabParams;
    if(!pFrameGrabber->Start(sVideofile.c_str(), grabParams))
		return E_FAIL;
	nCurFrame = 0;
	return S_OK;
}
bool PlayListFrameIterator::IsEnd()
{
	return pFrameGrabber->IsEnd();
}

HRESULT PlayListFrameIterator::SeekNext()
{
	pFrameGrabber->UnlockProcessedFrame();
    return S_OK;
}

inline HRESULT PlayListFrameIterator::GetFrameInfo(csinterface::IDataType * pData)
{

	switch( pData->type() )
	{
	//video frame
	case csinterface::DATAID_FRAMEITERATOR_VIDEOFRAME:
		{
			if (pFrameGrabber->HasFreshFrame())		
			{
				pFrameGrabber->LockLatestFrame(pBI, pImage, size);
				csinterface::TFrameIteratorVideo * pVideo = (csinterface::TFrameIteratorVideo *)(pData);
				VERIFY(pVideo != NULL);
				pVideo->data.pBI  = pBI;
				pVideo->data.bytes= pImage;
				pVideo->data.size = size;
				nCurFrame += 1;
				return S_OK;
			}
			else
			{
				return E_FAIL;
			}
		}
		break;
	// progress
	case csinterface::DATAID_FRAMEITERATOR_PROGRESS:
		{
			BaseFrameGrabberEx* pGrabber = dynamic_cast<BaseFrameGrabberEx*> (pFrameGrabber);
			VERIFY(pGrabber != NULL);
			csinterface::TFrameIteratorProgress* pProgress = (csinterface::TFrameIteratorProgress*)pData;
			pGrabber->GetPos(pProgress->data);
			return S_OK;
		}
		break;



	case csinterface::DATAID_CHAIR_CONTENTS_MAP:
		{
			csinterface::TChairContentsMap *contents = (csinterface::TChairContentsMap *)pData;
			pChairContent->GetContent(nCurFrame, contents->data);
		//	size_t sz = contents->data.size();
			return S_OK;
        }
		break;
	default:
		return E_FAIL;
	}
}

HRESULT CreatePlayListFrameIterator(IPlayListFrameIterator** ppFrameIterator)
{
	(*ppFrameIterator) = new PlayListFrameIterator;
	return S_OK;
}

