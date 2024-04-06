#if !defined(AFX_RANGECTRL_H__9ADE1F59_5538_4587_9FD8_1B9722C1704E__INCLUDED_)
#define AFX_RANGECTRL_H__9ADE1F59_5538_4587_9FD8_1B9722C1704E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RangeCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRangeCtrl window

#include <list>
#include "../CSChair/IChairContentEx.h"

class CRangeCtrl : public CWnd
{
	IChairContentEx*	m_pChairInt;
	RangeList_t*					m_pRangeList;
	CMenu	m_Menu;
	bool ValidateContent() const
	{
		if( !m_pRangeList ) return true;
		RangeList_t::const_iterator it  = m_pRangeList->begin();
		for(; it != m_pRangeList->end(); ++ it)
		{
			if( it->m_nStart > m_nMax || it->m_nStart < m_nMin )	return false;
			if( it->m_nEnd > m_nMax || it->m_nEnd < m_nMin )		return false;
			if( it->m_nStart > it->m_nEnd )							return false;
			if( it->m_nContent == Range::undefined)				return false;
			RangeList_t::const_iterator itNext = it++;
			if( itNext != m_pRangeList->end()  && it->m_nEnd >= itNext->m_nStart )
			{
				return false;
			}
		}
	}

	COLORREF		m_Colors[ Range::empty + 1 ];
	int				m_nMin, m_nMax;

	bool				m_bEditMode;
	int					m_nStartPos;
	Range::CONTENT		m_CurrentContent;

	std::pair<int,int> GetCurrentRange() const
	{
		int nCurPos		= m_wndSlider.GetPos();
		int nStartPos	=
	#if _MSC_VER < 1300
		std::_cpp_min
	#else
		std::min
	#endif	
			( m_nStartPos, nCurPos );
		int nEndPos		= 
	#if _MSC_VER < 1300
		std::_cpp_max
	#else
		std::max
	#endif	
			( m_nStartPos, nCurPos );
		return std::make_pair( nStartPos, nEndPos );
	}

	void DrawChannel(CDC& dc, const CRect& rc);
	
	int		SliderToPhysicPos(int nMin, int nMax, int nPos) const
	{
		int nMinSl, nMaxSl;
		m_wndSlider.GetRange( nMinSl, nMaxSl );
		return  nMin + MulDiv( nPos , nMax - nMin , nMaxSl - nMinSl );
	}

	void	DumpRangeList() const;
	CSliderCtrl	m_wndSlider;

public:
	

	CRangeCtrl();




// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRangeCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRangeCtrl();

	BOOL	Create( DWORD dwStyle, const CRect& rcClient, CWnd* pParent, UINT nID );

	void	SetRange(int nMin, int nMax);
	void	StartEdit	( /*Range::CONTENT cont*/ );
	void	SetCurrentRangeContent( Range::CONTENT cont );
	Range::CONTENT	GetCurrentRangeContent( ) const { return m_CurrentContent; };
	void	CancelEdit	();
	void	EndEdit		();
	int		GetPos() const					{ return m_wndSlider.GetPos();	}
	void	SetPos(int nPos)				{ m_wndSlider.SetPos(nPos);		}
	
//	const RangeList_t& GetContent() const		{ return m_RangeList;}
	void	SetContent(RangeList_t* rl)	;
	bool	IsEditMode( ) const { return m_bEditMode;}

	void Repaint(  ) 
	{
		m_wndSlider.UpdateWindow();
//		Invalidate();
	}
	// Generated message map functions
protected:
	//{{AFX_MSG(CRangeCtrl)
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	afx_msg BOOL OnNotifyDraw( UINT nID, NMHDR* pnmhdr, LRESULT* result);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RANGECTRL_H__9ADE1F59_5538_4587_9FD8_1B9722C1704E__INCLUDED_)
