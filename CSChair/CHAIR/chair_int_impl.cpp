#include "stdafx.h"
#include "../chair_int.h"
#include "../interfaceEx.h"
#include "ChairCommonClass.h"

struct ChairSerializerImpl : IChairSerializer
{
	virtual HRESULT LoadGaugeHallFromFile		(LPCWSTR szFileName, std::vector<BaseChair>&	Chairs, PositionMap_t& PosMap  )
	{
		try
		{
			ChairLoader cl;
			cl.Load( szFileName, Chairs, PosMap );
			return S_OK;
		}
		catch ( ChairSerializerException ) 
		{
			return E_FAIL;
		}
	}
	
	virtual HRESULT LoadGaugeHallFromString		(LPCWSTR szSource, std::vector<BaseChair>&	Chairs, PositionMap_t& PosMap )
	{
		try
		{
			ChairLoader cl;
			cl.LoadFromString( szSource, Chairs, PosMap );
			return S_OK;
		}
		catch ( ChairSerializerException ) 
		{
			return E_FAIL;
		}
	}
	
	virtual HRESULT LoadGaugeHallFromBinaryData	(const void* pData, size_t nSize, std::vector<BaseChair>&	Chairs, PositionMap_t& PosMap )
	{
		try
		{
			ChairLoader cl;
			cl.LoadFromBinArray( pData, nSize, Chairs, PosMap );
			return S_OK;
		}
		catch ( ChairSerializerException ) 
		{
			return E_FAIL;
		}	
	}
	
	////// save

	virtual HRESULT SaveGaugeHallToFile		(LPCWSTR szFileName, const std::vector<BaseChair>&	Chairs, const PositionMap_t& PosMap )
	{
		try
		{
			ChairSaver cs;
			cs.Save( szFileName, Chairs, PosMap );
			return S_OK;
		}
		catch ( ChairSerializerException ) 
		{
			return E_FAIL;
		}	
	}
	
	virtual HRESULT SaveGaugeHallToString	(std::wstring& sSource,	const std::vector<BaseChair>&	Chairs, const PositionMap_t& PosMap )
	{
		try
		{
			ChairSaver cs;
			cs.SaveToString( sSource, Chairs, PosMap );
			return S_OK;
		}
		catch ( ChairSerializerException ) 
		{
			return E_FAIL;
		}
	}
	
	virtual HRESULT SaveGaugeHallToBinaryData	(std::vector<BYTE>& Data, const std::vector<BaseChair>&	Chairs, const PositionMap_t& PosMap )
	{
		try
		{
			ChairSaver cs;
			cs.SaveToBinArray( Data, Chairs, PosMap );
			return S_OK;
		}
		catch ( ChairSerializerException ) 
		{
			return E_FAIL;
		}		
	}

	virtual void Release()
	{
		delete this;
	}
};

EXPOSE_INTERFACE( ChairSerializerImpl, IChairSerializer, CHAIR_SERIALIZER_INTERFACE_0 );