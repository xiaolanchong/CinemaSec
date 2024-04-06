//////////////////////////////////////////////////////////////////////////
// SeriesAlgorithm.h
// ELVEES
// author: Alexander Boltnev
// e-mail: Alexander.Boltnev@biones.com
// date  : May 2005
//////////////////////////////////////////////////////////////////////////
#pragma once
class SeriesAlgorithm: public csinterface::ILearningAlgorithm
{
public:
	SeriesAlgorithm(void);
	~SeriesAlgorithm(void);

	virtual HRESULT Start( IN float startTime );

	virtual HRESULT ProcessImage( IN const BITMAPINFO  * pBI,
		IN const __int8      * pImage,
		IN float               timeStamp );

	virtual HRESULT Stop( IN float stopTime, bool saveResult = true );

	virtual HRESULT Release(); 

	virtual HRESULT SetData( IN const csinterface::IDataType * pData );

	virtual HRESULT GetData( OUT csinterface::IDataType * pData ) const;

	virtual HRESULT QueryData( IN OUT csinterface::IDataType * pData ) const;

	HRESULT Initialize(csinterface::IInformationQuery * pInfo, IDebugOutput * pDebugOutput);

	QImage testImage;

	IDebugOutput                   * pDebugOutput;
	csinterface::IInformationQuery * pInfo;

	bool bReady;



};
