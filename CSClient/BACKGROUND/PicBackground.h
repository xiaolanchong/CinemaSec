// PicBackground.h: interface for the CPicBackground class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PICBACKGROUND_H__276E5C2A_999D_4FFA_9E8C_6FACE7074484__INCLUDED_)
#define AFX_PICBACKGROUND_H__276E5C2A_999D_4FFA_9E8C_6FACE7074484__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if _MSC_VER <= 1200
#pragma warning( disable : 4097 )
#endif 

#include "../Common/Exception.h"
#include "Background.h"
#include <string>
#include <boost/scoped_array.hpp>

MACRO_EXCEPTION(ImageException, CommonException);
MACRO_EXCEPTION(ImageFileException, ImageException);
MACRO_EXCEPTION(ImageSizeException, ImageException);

class CBitmapBackground : public CBackground  
{
	template<class T> struct Copy
	{
		Copy( T* dst, const T* src, int size )
		{
			memcpy( dst, src, size* sizeof(T) );
		}
		Copy(){}
	};
//#pragma pack(push)
//#pragma pack(1)
	struct BitmapInfoEx : BITMAPINFO					
	{
		RGBQUAD	rgb[255];
		BitmapInfoEx(const BITMAPINFO& bi)
		{
			memcpy( this, &bi, sizeof(BITMAPINFO) );
			if( bi.bmiHeader.biBitCount > 0 && bi.bmiHeader.biBitCount <= 8)
			{
				DWORD dwPaletteSize = 0;
				if ( bi.bmiHeader.biClrUsed )
				{
					dwPaletteSize = bi.bmiHeader.biClrUsed;
				}
				else
				{
					dwPaletteSize = (1 << bi.bmiHeader.biBitCount) ;
				}
				memcpy( &this->bmiColors, & bi.bmiColors, dwPaletteSize * sizeof(RGBQUAD) );
			}
		}
		BitmapInfoEx(){	}
	} m_bi;
//#pragma pack(pop)
	boost::scoped_array<BYTE>	m_bits;	
	Copy<BYTE>					m_copy;

	Bitmap			m_Image;


	
public:
	CBitmapBackground(const std::wstring& FileName);
	CBitmapBackground(const BITMAPINFO* pbi, const void * pbits);
	virtual ~CBitmapBackground();

	virtual void	Draw(Graphics& gr, const CRectF& rc) const;
			CSize	GetSize() const;			
};

#endif // !defined(AFX_PICBACKGROUND_H__276E5C2A_999D_4FFA_9E8C_6FACE7074484__INCLUDED_)
