#include "../../CSAlgo/interfaces/camera_analyzer_interface.h"
#include "../../CSInterfaces/info_query_interface.h"
#include "../../CSInterfaces/learning_algo_interface.h"
#include "../PlayListFrameIterator.h"


class AlbertsAlgorithmsLoader
	:public IPlayListDataManager,
	 public csinterface::IInformationQuery
{
public:

	HRESULT LoadOneItem(const PlaylistItem& item);
	HRESULT Start();
	HRESULT Stop(bool saveResult = true);
	bool    IsReady();
	HRESULT Release();
	HRESULT SetData(const csinterface::IDataType * pData);
	HRESULT GetData(csinterface::IDataType * pData) const;
	HRESULT QueryData(csinterface::IDataType * pData) const;

	UINT    id();


	AlbertsAlgorithmsLoader()
	: bRunning(false)
	, bReady(false)
	, bAlgorithmLoaded(false)
	, bDestroyed(true)
	, pFrameIterator(0)
	, pAlgorithm(0)
	{}
	~AlbertsAlgorithmsLoader()
	{}


private:
	long GetCamID(std::wstring videoFileName);
	bool ExtractGoodChairs(BaseChairArr & chairArr, long camereNo);

  	csinterface::ILearningAlgorithm * pAlgorithm;
	IPlayListFrameIterator * pFrameIterator; 
	PlaylistItem curItem;
	
	//
	BaseChairArr emptyHallChairArr;
	Arr2f fBackground;

	// for quiting from the main cycle
	bool bReady;
	bool bRunning;
	bool bAlgorithmLoaded;
	bool bDestroyed;
	bool saveResult; // if true algorithm saves result

	// for communication with algorithm
	csinterface::TChairContentsMap chairContentsMap;
	csinterface::TFrameIteratorVideo image;

	HRESULT SaveDataInAlgo();
	PlayListParameters myParameters;
};