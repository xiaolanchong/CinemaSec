

#define N_SMALL_SIZE 64
#define N_SMALL_SQUARE_SIDE 4
#define N_USED_WALSH_COMPONENTS 256
#define N_MOVE_DENSITY_ARR_SMALL 1
#define N_MOVE_DENSITY_ARR_MIDDLE 4
#define N_MOVE_DENSITY_ARR_BIG 9



class CSDBInterfacePreparer
	:public IPlayListDataManager 
{
public:

	struct DataBlockInRecord
	{
		long recordVersionID;
		unsigned char image[N_SMALL_SIZE * N_SMALL_SIZE];
		unsigned char levels_image[N_SMALL_SIZE * N_SMALL_SIZE];
		double walshComponents[N_USED_WALSH_COMPONENTS];
		double averageColors[N_SMALL_SIZE * N_SMALL_SIZE / N_SMALL_SQUARE_SIDE / N_SMALL_SQUARE_SIDE];
		double s_correlation;
		double d_correlation;
		long answer;
	};

	HRESULT LoadOneItem(const PlaylistItem& item);
	HRESULT Start();
	HRESULT Stop(bool saveResult = true);
	bool    IsReady();
	HRESULT Release();
	HRESULT SetData(const csinterface::IDataType * pData);
	HRESULT GetData(csinterface::IDataType * pData) const;
	
	UINT    id();


	CSDBInterfacePreparer()
	: bReady(false)
	, bRunning(false)
	, pCameraAnalyzer(0)
	, pFrameIterator(0)
	, nSkip(0)
	{}
	~CSDBInterfacePreparer()
	{}


private:
    bool                    bReady;
	ICameraAnalyzer       * pCameraAnalyzer;
	IPlayListFrameIterator* pFrameIterator; 
	PlaylistItem            curItem;
	
	//
	BaseChairArr            emptyHallChairArr;
	Arr2f                   fBackground;
	Arr2ub                  byteBackground;

	//camera analyzer output 
	QImage                  testImage;

	// for quiting from the main cycle
	bool                    bRunning;

	int                     nSkip;

	//member functions
	HRESULT InitCamAnalyzer(ICameraAnalyzer** pCameraAnalyzer);
	HRESULT GetCurveByID(long index, Vec2fArr& curve);
	HRESULT DrawContentAndCurves(csinterface::TChairContentsMap& chairContentsMap, csalgo::TChairLearnArr& chairLearnArr, QImage& img);
};