#ifndef __CSCHAIRSERIALIZER_H__
#define __CSCHAIRSERIALIZER_H__

#include <vector>
#include "../csalgo/algorithm/base_chair.h"

extern "C"
{

DWORD WINAPI 	LoadGaugeHallFromFile	(LPCWSTR szFileName, std::vector<BaseChair>&	Chairs );
DWORD WINAPI 	LoadGaugeHallFromString	(LPCWSTR szFileName, std::vector<BaseChair>&	Chairs );

// save as above but w/ camera position
// PosMap - map camera id-to-position(x, y in a rectangle array)
HRESULT WINAPI 	LoadGaugeHallFromFileEx	(	LPCWSTR szFileName, 
											std::vector<BaseChair>& Chairs, 
											std::map<int, std::pair<int, int> >& PosMap );

DWORD WINAPI 	SaveGaugeHallToFile		(LPCWSTR szFileName,		const std::vector<BaseChair>&	Chairs );
DWORD WINAPI 	SaveGaugeHallToString	(std::wstring& szFileName,	const std::vector<BaseChair>&	Chairs );

}
#endif //__CSCHAIRSERIALIZER_H__