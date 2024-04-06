#pragma once

#include "MultiImageLayout.h"
#include "COMMON/Exception.h"
#include <boost/shared_ptr.hpp>
#include "BACKGROUND/Background.h"

MACRO_EXCEPTION( LayoutException, CommonException );
// CLayoutWnd 

struct LayoutWndCB
{
	virtual void						DrawItem( Gdiplus::Graphics& gr, CRect rc, DWORD_PTR dwCookie )					= 0;
//	virtual void						GetItemMenuContent( std::vector<CString> &  MenuItems )	= 0;
	virtual std::pair<bool, DWORD_PTR>	SelectMenuItem( size_t nIndex  ) = 0;
	virtual void						DeleteItem( DWORD_PTR dwCookie ) = 0;
	virtual bool						GetItemMenuContent( std::vector<CString> &  MenuItems, WORD x, WORD y, bool& bCanDeleted ) = 0;
};

class CLayoutWnd : public CWnd
{
//	DECLARE_DYNAMIC(CLayoutWnd)
protected:
#if 0
	WORD		m_nWidth;
	WORD		m_nHeight;
#endif
	SparceRectImageLayout	m_Layout;

	DWORD GetIndex( WORD x, WORD y ) const
	{
		return (x << 16) | y;
	}

	std::pair<WORD, WORD> GetCoord( DWORD Index ) const
	{
		return std::make_pair( WORD(Index >> 16), WORD(Index & 0xFFFF ) );
	}

//	bool	DetectCell( Point pt, WORD& x, WORD& y );

	
	void	DrawGrid( Gdiplus::Graphics& gr );

	CRect	GetRect( WORD x, WORD y ) ; // throw LayoutException
	DWORD	GetIndex( CPoint pt ); // throw LayoutException

	typedef std::map< DWORD, DWORD_PTR>	CellMap_t;
	CellMap_t	m_Cells;

	LayoutWndCB*	m_pCB;
	bool			m_bSelectable;
public:
	CLayoutWnd( LayoutWndCB* pCB, bool bSelectable);
	virtual ~CLayoutWnd();

	bool					SetSize(WORD w, WORD h);
	std::pair<WORD, WORD>	GetSize();

	struct Coord
	{ 
		WORD		x, y;
		DWORD_PTR	dwCookie;
		Coord(WORD _x, WORD _y, DWORD_PTR _c) : x(_x), y(_y), dwCookie(_c){}
	};

	void	GetResultLayout( std::vector<Coord>& c );
	void	SetResultLayout( const std::vector<Coord>& c );

	void	SetAccessableMenu(  );

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg	void OnPaint();
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


