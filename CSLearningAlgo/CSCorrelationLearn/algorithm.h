/****************************************************************************
algorithm.cpp
---------------------
begin     : Aug 2004
author(s) : A.Akhriev, A. Boltnev
email     : aaah@mail.ru, Alexander.Boltnev@biones.com
****************************************************************************/

#pragma once

#include "../../CSUtility/utility/multi_histogram.h"

//=================================================================================================
/** \struct CrossCorrelationLearningAlgo.
    \brief  Implementation of cross-correlation learning algorithm. */
//=================================================================================================
struct CrossCorrelationLearningAlgo : public csinterface::ILearningAlgorithm
{

	const int totalIntervals;
	float min_v;
	float max_v;

	enum LayerName
	{
		EMPTY = 0,
		HUMAN = 1,
		CLOTH = 2		
	};
	struct HistogramAndContents 
	{
		LayerName			                    content;
		csutility::MultiHistogram < float , 3 > histo;

		HistogramAndContents& operator = (const HistogramAndContents & rh)
		{
			content = rh.content;
			histo   = rh.histo;
			return (*this);
		}
	};

	
	CrossCorrelationLearningAlgo();
	virtual ~CrossCorrelationLearningAlgo();

	virtual HRESULT Start( IN float startTime );

	virtual HRESULT ProcessImage( IN const BITMAPINFO  * pBI,
								  IN const __int8      * pImage,
		                          IN float               timeStamp );

	virtual HRESULT Stop( IN float stopTime, bool saveResult = true );

	HRESULT SaveFinalData();

	virtual HRESULT Release(); 

	virtual HRESULT SetData( IN const csinterface::IDataType * pData );

	virtual HRESULT GetData( OUT csinterface::IDataType * pData ) const;

	virtual HRESULT QueryData( IN OUT csinterface::IDataType * pData ) const;

    
	HRESULT InitCamAnalyzer();

	HRESULT Initialize(csinterface::IInformationQuery * pInfo, 
					   IDebugOutput                   * pDebugOutput);

	//
	BaseChairArr emptyHallChairArr;
	Arr2f fBackground;
	IDebugOutput * pDebugOutput;
	
	csinterface::IInformationQuery * pInfo;
	ICameraAnalyzer * pCameraAnalyzer;       
	//camera analyzer output 
	QImage testImage;
	bool bReady;
	long frame;

	std::vector< HistogramAndContents > histoArr;
	csutility::MultiHistogram < float, 3 > totalHisto;

private:
	HRESULT DrawContentAndCurves(csinterface::TChairContentsMap& chairContentsMap, csalgo::TChairLearnArr& chairLearnArr, QImage& img);

};

