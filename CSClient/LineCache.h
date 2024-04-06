//========================= Copyright © 2004, Elvees ==========================
// Author: Eugene V. Gorbachev
// Purpose: 
//
//=============================================================================

#ifndef LINECACHE_H
#define LINECACHE_H
#ifdef _WIN32
#pragma once
#endif

struct LineCache
{
	CPoint	m_ptStart;
	CPoint  m_ptEnd;

	LineCache(){};
	LineCache(	CPoint pts, CPoint pte ) : 
				m_ptStart(pts), m_ptEnd(pte) {};
	LineCache(const LineCache& rhs):
				m_ptStart(rhs.m_ptStart), m_ptEnd(rhs.m_ptEnd) {};

	LineCache& operator =(const LineCache& rhs)
	{
		if(&rhs != this)
		{
			m_ptStart	= rhs.m_ptStart;
			m_ptEnd		= rhs.m_ptEnd;
		}
		return *this;
	}

	void	Offset( CSize size)
	{
		m_ptStart	+= size;
		m_ptEnd		+= size;
	}

	void	Draw(CDC* pDC)
	{
		CPen pen( PS_SOLID, 1 , RGB(0 , 96, 192) );
		CGdiObject* pOldPen = pDC->SelectObject( &pen );
		int nROP = pDC->SetROP2(R2_NOTXORPEN);

		pDC->MoveTo( m_ptStart	);
		pDC->LineTo( m_ptEnd	);

		pDC->SetROP2(nROP);
		pDC->SelectObject(pOldPen);
	}

	void	DrawInvert(CDC* pDC)
	{
		Draw(pDC);
	}	
};

#endif // LINECACHE_H
