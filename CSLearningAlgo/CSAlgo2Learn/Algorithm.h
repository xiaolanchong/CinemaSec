//////////////////////////////////////////////////////////////////////////
// Algorithm.h
// author: Alexander Boltnev
// e-mail: Alexander.Boltnev@biones.com
// date  : 30 june 2005
//////////////////////////////////////////////////////////////////////////
#pragma once
/// <summary>
/// class AlgorithmVersionTwo implements basic functionality of learning algorithm
/// </summary>

class AlgorithmVersionTwo : public csinterface::ILearningAlgorithm
{
public:
	/// <summary>
	/// default constructor
	/// </summary>
	AlgorithmVersionTwo();
	/// <summary>
	/// virtual destructor
	/// </summary>
	virtual ~AlgorithmVersionTwo();
	/// <summary>
	/// this method starts the algorithm, all additional initialization should be 
	/// in this method
	/// </summary>
	/// <param name="startTime">
	/// starting time (input)
	/// </param>
	virtual HRESULT Start( IN float startTime );

	/// <summary>
	/// this method should be called for every video frame of learning video sequence
	/// </summary>
	/// <param name="pBI">
	/// pointer to the header of the image (input)
	/// </param>
	/// <param name="pImage">
	/// pointer to the buffer containing the image (input)
	/// </param>
	/// <param name="timeStamp">
	/// the time when frame was recorded (input)
	/// </param>
	/// <remarks>
	/// this is the most important method of the class. All processing is done here
	/// </remarks>
	virtual HRESULT ProcessImage( IN const BITMAPINFO * pBI,
		IN const __int8 * pImage,
		IN float timeStamp );


	/// <summary>
	/// this method finalizes the learning
	/// </summary>
	/// <param name="stopTime">
	/// the time when video recording was ended
	/// </param>
	/// <param name="saveResult">
	/// if true the algorithm should save the result
	/// </param>
	/// <remarks>
	/// the finalization of the learning process is implemented in this method. 
	/// </remarks>
	virtual HRESULT Stop( IN float stopTime, bool saveResult = true );

	virtual HRESULT Release();

	/// <summary>
	/// call this method to change state of the object inner variables
	/// </summary>
	/// <param name="pData">
	/// pointer to valid IDataType object
	/// </param>
	/// <remarks>
	/// currently you can use only IDataType objects with the following identifiers:
	/// 
	///	DATAID_MSG_SAVE_PARAMETERS 
	/// to save parameters of the algorithm
	///
	///	DATAID_MSG_LOAD_PARAMETERS
	/// to load parameters of the algorithm
	///
	///	DATAID_MSG_SET_DEFAULT_PARAMS
	/// to set parameters to defaults 
	///
	///	DATAID_MSG_CLEAN_TEMP_DATA
	/// to clean temporary data of the algorithm
	/// 
	///	DATAID_RESULT_FILENAME         
	/// to set the resulting directory ( required to normal function )
	///
	///	DATAID_MSG_SHOW_PARAM_DIALOG
	/// to show current parameters of the algorithm
	///
	/// </remarks>
	virtual HRESULT SetData( IN const csinterface::IDataType * pData );

	/// <summary>
	/// call this method to obtain information from the algorithm
	/// </summary>
	/// <param name="pData">
	/// pointer to IDataType object that will be created inside the algorithm
	/// </param>
	/// <remarks>
	/// currently you can use only IDataType objects with the following identifiers:
	/// 
	/// DATAID_QIMAGE
	/// the image produced by algorithm for visualisation purposes
	///
	/// </remarks>

	virtual HRESULT GetData( OUT csinterface::IDataType * pData ) const;

	virtual HRESULT QueryData( IN OUT csinterface::IDataType * pData ) const;


	/// <summary>
	/// finalization is actually done in this method
	/// </summary>
	/// <remarks>
	/// now it is more flexible to call Stop( true ) to get the same result
	/// </remarks>
	HRESULT SaveFinalData();

	/// <summary>
	/// initializes the algorithm 
	/// </summary>
	/// <param name="pInfo">
	/// a call back interface which feeds the algorithm with data
	/// </param>
	/// <param name="pDebugOutput">
	/// pointer to interface for displaying messages in the main thread
	/// </param>
	/// <remarks>
	/// call this method while creating the algorithm instance
	/// </remarks>
	HRESULT Initialize( csinterface::IInformationQuery * pInfo, csinterface::IDebugOutput * pDebugOutput );

private:
	/// <summary>
	/// current frame number
	/// </summary>
	int frame; 

	/// <summary>
	/// call back interfaces for communication with playlist program
	/// </summary>
	csinterface::IDebugOutput * pDebugOutput;
	/// <summary>
	/// call back interfaces for communication with playlist program
	/// </summary>
	csinterface::IInformationQuery * pInfo; 

	/// <summary>
	/// floating point background
	/// </summary>
	Arr2f fBackground; 
	
	/// <summary>
	/// float image of the current frame
	/// </summary>
	Arr2f floatImage; 

	/// <summary>
	/// image to show on dialog
	/// </summary>
	QImage testImage;

	/// <summary>
	/// a critical section
	/// </summary>
	CCriticalSection m_critSection;


	/// <summary>
	/// array with chairs 
	/// </summary>
	BaseChairArr emptyHallChairArr; 

	/// <summary>
	/// array with "extended" chairs 
	/// </summary>
	BaseChairExArr emptyHallChairArrEx; 

	/// <summary>
	/// type of array of DynamicChairBehaviour elements
	/// </summary>
	/// <c>
	typedef std::vector < std::pair< ChairContents, DynamicChairBehaviour > > DynamicChairBehaviourLearnArr;
	/// </c>

	/// <summary>
	/// type of array of StaticChairBehaviour elements
	/// </summary>
	typedef std::vector < std::pair< ChairContents, StaticChairBehaviour > > StaticChairBehaviourLearnArr;
	
	/// <summary>
	/// array with dynamic parameters
	/// </summary>
	DynamicChairBehaviourLearnArr dynamicArr;

	/// <summary>
	/// array with static parameters
	/// </summary>
	StaticChairBehaviourLearnArr staticArr;

	
	void DrawCurveWithContents(const Vec2fArr& curve, const ChairContents content, QImage& image);

	//now members for input / output 

	// path for files with results
	std::wstring resultPath;

	// function opens output streams
	void OpenOutputStreams();
	// function opens input streams
	void OpenInputStreams();
	// function closes input/output streams
	void CloseStreams();	
	// function clears content in temp files
	void AlgorithmVersionTwo::CleanTmpFiles();
	
	// output streams
	// a) for dynamic parameters
	std::fstream dynamicParamHumanFile;
	std::fstream dynamicParamClothFile;
	// b) for static parameters
	std::fstream staticParamHumanFile;
	std::fstream staticParamClothFile;
	std::fstream staticParamEmptyFile;
	void PutChairToStream( int i );


	bool LoadTempData( FloatArr & staticEmpty, 
					   FloatArr & staticClothes,
					   FloatArr & staticHuman,
					   FloatArr & dynamicThing,
					   FloatArr & dynamicHuman );

	Parameters params;
	csalgo2::ImageAcquirer m_acquirer;

	struct VideoWriter {
		VideoWriter() 
			:cnt(0)
		{
		}

		ChairContents contents;

		VideoWriter & operator = ( const VideoWriter & rh )
		{
			this->contents = rh.contents;
			this->cnt = rh.cnt;
			this->pWriter = rh.pWriter;
			return (*this);
		}

		VideoWriter ( const VideoWriter & rh )
		{
			this->contents = rh.contents;
			this->cnt = rh.cnt;
			this->pWriter = rh.pWriter;
		}
		
		void WriteFrame( const Arr2ub * pImage )
		{
			switch( contents )
			{
			case CHAIR_EMPTY:
				break;
			case CHAIR_HUMAN:
				break;
			case CHAIR_CLOTHES:
				break;
			default:
				return;
			}
			pWriter->WriteFrame( pImage, 0 );
		}

		void Reset( int id, std::wstring videoFilename, ChairContents contents, std::wstring video_sequence)
		{
			this->contents = contents;

			std::wstring filename;

			std::wstring type;
			switch( contents ) 
			{
			case CHAIR_EMPTY:
				type = _T("E");
				break;
			case CHAIR_HUMAN:
				type = _T("H");
				break;
			case CHAIR_CLOTHES:
				type = _T("C");
				break;
			default:
				return;
			}
				
			wchar_t str_id[100];
			_ltow(id, str_id, 10);

			wchar_t str_cnt[100];
			_ltow(cnt, str_cnt, 10);

			filename = videoFilename + type + _T("_") + video_sequence + _T("_") + std::wstring(str_id) + _T("_") + std::wstring(str_cnt) + _T(".avd");
			pWriter = boost::shared_ptr < avideolib::IVideoWriter >( avideolib::CreateAVideoWriter( filename.c_str(), 0, false, true) );
			cnt++; // count how many times reset was called
		}
	private:
		boost::shared_ptr < avideolib::IVideoWriter > pWriter;
		int cnt;
	};
	std::vector < VideoWriter > videoWritersArr;
	ImageStandardizer standardizer; // one image "standadizer" for all images
	int m_skip;
};


HRESULT CreateLearningAlgorithmInstance( IN  csinterface::IInformationQuery   * pInfo,
										IN  csinterface::IDebugOutput * pDebug,
										OUT csinterface::ILearningAlgorithm ** ppAlgo )
{
	HRESULT retValue = E_FAIL;

	try
	{
		if (pInfo == 0 || pDebug == 0 || ppAlgo == 0)
			return E_FAIL;

		AlgorithmVersionTwo * pAlgo = new AlgorithmVersionTwo();
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

LPCWSTR GetLearningAlgorithmName()
{
	return L"ALGO 2";
}
