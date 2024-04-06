#include "..\..\CSAlgo\interfaces\camera_analyzer_interface.h"
#include "CinemaHistogram.h"
#include "..\PlayListFrameIterator.h"



class CSHistogramPreparer
	:public IPlayListDataManager 
{
public:

	HRESULT LoadOneItem(const PlaylistItem& item);
	HRESULT Start();
	HRESULT Stop();
	bool    IsReady();
	HRESULT Release();
	HRESULT SetData( const csinterface::IDataType * pData );
	HRESULT GetData( csinterface::IDataType * pData ) const;
	UINT    id();


	CSHistogramPreparer()
	:bReady(false),
	 bRunning(false),
	 statHist(0.0f, 2.0f, 256),
	 pCameraAnalyzer(0),
	 pFrameIterator(0)
	{
		//CreateCameraAnalyzer(&pCameraAnalyzer);		no longer needed
	}
	~CSHistogramPreparer()
	{
		delete pDebugOutput;
	}


private:
    bool                    bReady;
	ICameraAnalyzer       * pCameraAnalyzer;
	IPlayListFrameIterator* pFrameIterator; 
	PlaylistItem            curItem;
	
	//the answer - histogram
	CinemaHistogram <float> statHist;

	//
	BaseChairArr            emptyHallChairArr;
	Arr2f                   fBackground;
	IDebugOutput          * pDebugOutput;
	//camera analyzer output 
	QImage                  testImage;

	// for quiting from the main cycle
	bool bRunning;

	//member functions
	HRESULT InitCamAnalyzer(ICameraAnalyzer** pCameraAnalyzer);
};