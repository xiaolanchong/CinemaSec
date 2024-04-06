#pragma once
#include "background\background.h"
#include "common\exception.h"

MACRO_EXCEPTION(ImageBgException, CommonException);
MACRO_EXCEPTION(ImageBgFileException, ImageBgException);
MACRO_EXCEPTION(ImageBgSizeException, ImageBgException);



class CImageBackground : public CBackground
{
	IStream* CreateStream( const void* pBytes, size_t nSize)
	{
		HGLOBAL hMem = GlobalAlloc( GMEM_FIXED, nSize );
		void* pLock = GlobalLock( hMem );
		memcpy( pLock, pBytes, nSize );
		GlobalUnlock( hMem );
		IStream* pStream;
		CreateStreamOnHGlobal( hMem, TRUE, &pStream );
		return pStream;
	}	

	IStream*		m_pStream;
	Gdiplus::Image	m_Image;
public:
	CImageBackground(const std::wstring& FileName);
	CImageBackground(const void* pBytes, size_t nSize);

	void	Draw(Graphics& gr, const CRectF& rc) const;
	CSize	GetSize() const;
};