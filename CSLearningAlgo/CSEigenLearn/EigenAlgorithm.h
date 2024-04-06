//////////////////////////////////////////////////////////////////////////
//EigenAlgorithm.cpp
//author: A. Boltnev
//e-mail: Alexander.Boltnev@biones.com
//date  : Aug 2004
//
//for research purpuses, not production quality
//////////////////////////////////////////////////////////////////////////

#pragma once

//=================================================================================================
/** \struct EigenLearningAlgo.
    \brief  Implementation of eigen-image learning algorithm. */
//=================================================================================================
class EigenLearningAlgo : public csinterface::ILearningAlgorithm
{
public:
	EigenLearningAlgo();
	virtual ~EigenLearningAlgo();

	virtual HRESULT Start( IN float startTime );
	
	virtual HRESULT ProcessImage( IN const BITMAPINFO * pBI,
                                  IN const __int8 * pImage,
                                  IN float timeStamp );

	virtual HRESULT Stop( IN float stopTime, bool saveResult = true );

	virtual HRESULT Release();

	virtual HRESULT SetData( IN const csinterface::IDataType * pData );

	virtual HRESULT GetData( OUT csinterface::IDataType * pData ) const;

	virtual HRESULT QueryData( IN OUT csinterface::IDataType * pData ) const;

	HRESULT SaveFinalData();
	
	HRESULT Initialize(csinterface::IInformationQuery * pInfo, csinterface::IDebugOutput * pDebugOutput);
	void DrawCurveWithContents(const Vec2fArr& curve, const ChairContents content, QImage& image);
private:
	BaseChairArr emptyHallChairArr; // array with chairs 
	BaseChairExArr emptyHallChairArrEx; // array with "extended" chairs 

	Arr2f fBackground; // floating point background
	Arr2ub byteFrame; // byte image of the current frame
	Arr2f floatImage; // float image of the current frame
	QImage testImage; // image to show on dialog



	EigAlgoImageHolderArr corrImages; // 

	// array of ChairBehaviour elements useful for learning algo's
	typedef std::vector < std::pair< ChairContents, DynamicChairBehaviour > > DynamicChairBehaviourLearnArr;
	DynamicChairBehaviourLearnArr behaviourArray; // 

	csinterface::IDebugOutput * pDebugOutput; // interfaces
	csinterface::IInformationQuery * pInfo; // interfaces
	int frame; // frame number
	bool bReady; // if this ready to give information?
	std::wfstream file; // file for output
	CSDataPrepareGlobals imageFunctions;
	std::wstring resFilename;

	void OpenOutputStream();
	void PutChairToStream( int index, int startFrame );
	void CloseOutputStream(); 
	void GetPlotOfEigenValues(std::vector<float> ev, QImage& img);
	void AddToTotalHisto(int i);

	std::fstream infile;
	std::wstring fileNameHumanTmp; 
	std::wstring fileNameThingsTmp;

	csutility::MultiHistogram <float, 2> allTotalHuman;
	csutility::MultiHistogram <float, 2> allTotalThings;
	MeanVarAccum < float > humanAccum;
	MeanVarAccum < float > clothAccum;

	std::wstring fileNameMeanVarCountHuman; 
	std::wstring fileNameMeanVarCountCloth;

	std::list < float > momentsListHuman;
	std::list < float > momentsListCloth;
	std::fstream dynParamFile;


	//////////////////////////////////////////////////////////////////////////
	std::fstream tmpfile; // should be deleted!
};

