#pragma once

#include <vector>
// CImageStatic

class CImageStatic : public CStatic
{
	DECLARE_DYNAMIC(CImageStatic)

	BITMAPINFO			m_bi;
	std::vector<BYTE>	m_Image;
	CString				m_sDesc;
public:
	CImageStatic();
	virtual ~CImageStatic();

	bool	SetImage( const BITMAPINFOHEADER* bih, const BYTE* pData, CString sDesc );

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnPaint() ;
};


