//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Ёкспортируемы функции дл€ работы с обводкой
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 15.07.2005
//                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include "ChairCommonClass.h"

//////////////////////////////////////////

extern "C"
{

DWORD WINAPI 	LoadGaugeHallFromFile	(LPCWSTR szFileName, std::vector<BaseChair>&	Chairs )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	try
	{
		ChairLoader cl; 
		IChairSerializer::PositionMap_t PosMap;
		cl.Load( szFileName, Chairs, PosMap );
		return 0;
	}
	catch ( ChairSerializerException ) 
	{
		return DWORD(-1);
	}
}

DWORD WINAPI 	LoadGaugeHallFromString	(LPCWSTR szFileName, std::vector<BaseChair>&	Chairs )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	try
	{
		ChairLoader cl;
		IChairSerializer::PositionMap_t PosMap;
		cl.LoadFromString( szFileName, Chairs, PosMap );
		return 0;
	}
	catch ( ChairSerializerException ) 
	{
		return DWORD(-1);
	}
}

DWORD WINAPI 	SaveGaugeHallToFile		(LPCWSTR szFileName,		const std::vector<BaseChair>&	Chairs )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	try
	{
		ChairSaver cs;
		IChairSerializer::PositionMap_t PosMap;
		cs.Save( szFileName, Chairs, PosMap );
		return 0;
	}
	catch ( ChairSerializerException ) 
	{
		return DWORD(-1);
	}
}

DWORD WINAPI 	SaveGaugeHallToString	(std::wstring& szFileName,	const std::vector<BaseChair>&	Chairs )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	try
	{
		ChairSaver cs;
		IChairSerializer::PositionMap_t PosMap;
		cs.SaveToString( szFileName, Chairs, PosMap );
		return 0;
	}
	catch ( ChairSerializerException ) 
	{
		return DWORD(-1);
	}
}


HRESULT WINAPI 	LoadGaugeHallFromFileEx	(	LPCWSTR szFileName, 
											std::vector<BaseChair>& Chairs, 
											std::map<int, std::pair<int, int> >& PosMap )
try
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	ChairLoader cl; 
	cl.Load( szFileName, Chairs, PosMap );
	return S_OK;
}
catch ( ChairSerializerException ) 
{
	Chairs.clear();
	PosMap.clear();
	return E_FAIL;
};

}