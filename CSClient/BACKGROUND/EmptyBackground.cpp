// EmptyBackground.cpp: implementation of the CEmptyBackground class.
//
//////////////////////////////////////////////////////////////////////
#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "EmptyBackground.h"
#include "..\res\resource.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//std::wstring CEmptyBackground::s_NoImage;


CEmptyBackground::CEmptyBackground()
{
/*	if( s_NoImage.empty() )	
	{
		CString str;
		str.LoadString( IDS_NOIMAGE );
		s_NoImage = Helper::Convert( str );
	}*/
}

CEmptyBackground::~CEmptyBackground()
{

}

void CEmptyBackground::Draw(Graphics& gr, const CRectF& rc) const
{
   Gdiplus::FontFamily  fontFamily(L"Arial");
   Gdiplus::Font        font(&fontFamily, 24, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
   
   Gdiplus::StringFormat stringFormat;
   stringFormat.SetLineAlignment(Gdiplus::StringAlignmentCenter);
	stringFormat.SetAlignment(Gdiplus::StringAlignmentCenter);

//	GetResourceMgr().GetAnsi( IDS_NOIMAGE )
	//FIXME: Unicode hack
	CString strImage;
	strImage.LoadString( IDS_NO_IMAGE );
	std::wstring str = strImage;			// RESOURCE!!!

	SolidBrush brString( static_cast<DWORD>(Color::Black) );
	gr.DrawString( str.c_str(), -1, &font, rc, &stringFormat, &brString);


}
