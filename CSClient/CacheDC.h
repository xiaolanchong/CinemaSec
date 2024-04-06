//========================= Copyright © 2004, Elvees ==========================
// Author: Eugene V. Gorbachev
// Purpose: Cachable dc for context drawing
//
//=============================================================================

#ifndef CACHEDC_H
#define CACHEDC_H
#ifdef _WIN32
#pragma once
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

template<typename T> class CCacheDC
{
	bool	m_bValidCache;
	T		m_Cache;
	CSize	m_ScrollPos;
public:
	CCacheDC();

	void	Draw(CDC *	pDC, const T& NewCache);
	void	Draw(CWnd*	pWnd, const T& NewCache);
	T&		GetCache() { return m_Cache;}
	void	InvalidateCache(CWnd* pWnd);
};

template<typename T> CCacheDC<T>::CCacheDC(): 
	m_bValidCache(false), 
	m_Cache()
{
	
}

template<typename T> void CCacheDC<T>::Draw(CDC * pDC, const T& NewCache)
{
	m_bValidCache = true;
	m_Cache = NewCache;
	m_Cache.Draw(pDC);
	m_ScrollPos = CSize(0,0);
}

template<typename T> void CCacheDC<T>::Draw(CWnd * pWnd, const T& NewCache)
{
	CSize sizeScroll (	pWnd->GetScrollPos( SB_HORZ ),
						pWnd->GetScrollPos( SB_VERT ) );

	CDC* pDC = pWnd->GetDC();
	if(m_bValidCache)
	{
		CSize sizeOff = sizeScroll - m_ScrollPos;
		m_Cache.Offset( -sizeOff );
		m_Cache.DrawInvert(pDC);
	}
	m_bValidCache = true;
	m_Cache = NewCache;
	m_Cache.Offset( -sizeScroll );
	m_ScrollPos = sizeScroll;
	m_Cache.Draw(pDC);
	pWnd->ReleaseDC(pDC);
}

template<typename T> void CCacheDC<T>::InvalidateCache(CWnd * pWnd)
{
	if(m_bValidCache)
	{
		CSize sizeScroll (	pWnd->GetScrollPos( SB_HORZ ),
							pWnd->GetScrollPos( SB_VERT ) );

		CSize sizeOff = sizeScroll - m_ScrollPos;
		m_Cache.Offset( sizeOff );
		m_ScrollPos = sizeScroll;
		CDC* pDC = pWnd->GetDC();
		m_Cache.DrawInvert(pDC);
		m_bValidCache = false;
		pWnd->ReleaseDC(pDC);
	}
}
#endif // CACHEDC_H
