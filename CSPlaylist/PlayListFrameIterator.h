#include "..\CSUtility\utility\base_frame_grabber.h"
#include "..\CSChair\IChairContent.h"
#include "..\CSChair\interface.h"
#include "interfaces\IPlayListFrameIterator.h"

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
class PlayListFrameIterator
	:public IPlayListFrameIterator
{
	// initialize
public:

	HRESULT Initialize(const std::tstring& sVideoFileName, const std::tstring& sMarkingFile, const BaseChairArr& emptyHallArr);

	// use this method if you want to restart the iterator
	HRESULT Restart();
	// get to the next frame
	HRESULT SeekNext();
	// check if it is the end
	bool    IsEnd();
	//gets the info about chairs
	HRESULT GetFrameInfo(csinterface::IDataType * pData);
	/***/
	PlayListFrameIterator()
		:nBitsPerPixel(MY_BI_Y800),
		 nCurFrame(0)
	{
		CreateInterface(CHAIR_CONTENT_INTERFACE, (void **)(&pChairContent));
	}
	~PlayListFrameIterator()
	{
		if(pFrameGrabber != NULL) 
		{
			//pFrameGrabber->Stop(); // HERE IS SOMETHING LIKE ERROR!!!
			DeleteVideoFileGrabber(pFrameGrabber);
			pFrameGrabber = NULL;
		}
		if(pChairContent != NULL) 
			pChairContent->Release();
	}
	HRESULT Release()
	{
		delete this;
		return S_OK;
	}

private:

	IChairContent         * pChairContent;
	BaseFrameGrabber      * pFrameGrabber;
	std::tstring            sVideofile;
	std::tstring            sMarkfile;

	int                     nBitsPerPixel;
	//information about current frame
	int                     nCurFrame;
	LPBITMAPINFO            pBI;
	LPCUBYTE                pImage;
	int                     size;
	csalgo::TChairLearnArr  chairsInfo;
};
HRESULT CreatePlayListFrameIterator(IPlayListFrameIterator** ppFrameIterator);



