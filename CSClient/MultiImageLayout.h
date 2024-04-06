// MultiImageLayout.h: interface for the MultiImageLayout class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MULTIIMAGELAYOUT_H__5F83399D_A577_4361_820C_094D6F6148A5__INCLUDED_)
#define AFX_MULTIIMAGELAYOUT_H__5F83399D_A577_4361_820C_094D6F6148A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if _MSC_VER > 1200
#define _cpp_max	max
#define _cpp_min	min
#endif

class RectImageLayout
{
public:
  enum { DEF_GAP = 5 };          //!< the gap between image and window's frame

protected:
		DWORD				m_nX, m_nY;
  bool  m_bReady;            //!< true when layout is valid
  int   m_imgWidth;          //!< actual image width
  int   m_imgHeight;         //!< actual image height

  CRect m_rect;              //!< image rectangle in screen coordinates
	double	m_fScaleX;
	double	m_fScaleY;

	int		m_nGap;
public:

  	enum FillType
	{
		ftKeepAspectRatio,
		ftKeepSize,
		ftFillArea
	};

	void					SetMode(FillType ft)		{ m_ftMode = ft; }
	FillType				GetMode() const				{ return m_ftMode; }
	void					SetArray(int nX, int nY)	{ m_nX = nX; m_nY = nY;				 }
	std::pair<int, int>		GetArray() const			{ return std::make_pair( m_nX, m_nY);}

//	void	SetIndices(const std::vector<int>& Indices) { m_Indices = Indices; }
private:
	FillType		m_ftMode;
public:

void clear()
{
  m_bReady = false;
  m_imgWidth = 0;
  m_imgHeight = 0;
  m_rect.SetRectEmpty();
}



void recalculate( int imgW, int imgH, CWnd * pWnd )
{
  clear();
  if ((imgW <= 0) || (imgH <= 0) || (pWnd == 0))
    return;

  CRect clientRect;
  pWnd->GetClientRect( &clientRect );

  switch(m_ftMode)
  {
   case ftFillArea:
	m_rect = clientRect;
	m_rect.DeflateRect( m_nGap, m_nGap);
	 m_fScaleX = (double)(clientRect.Width()  - 2*m_nGap) / (double)imgW;
	m_fScaleY = (double)(clientRect.Height() - 2*m_nGap) / (double)imgH;	
	break;

   case ftKeepAspectRatio:
	   {
	   
	 m_fScaleX = (double)(clientRect.Width()  - 2*m_nGap) / (double)imgW;
	m_fScaleY = (double)(clientRect.Height() - 2*m_nGap) / (double)imgH;

    m_fScaleX = (m_fScaleY = 
#if _MSC_VER < 1300
		std::_cpp_min
#else
		std::min
#endif
						( m_fScaleX, m_fScaleY ));

	  int W = (int)(m_fScaleX * imgW + 0.5);
	  int H = (int)(m_fScaleY * imgH + 0.5);

	  if ((W <= 1) || (H <= 1))
		return;

	  int gapX = (clientRect.Width()  - W) / 2;
	  int gapY = (clientRect.Height() - H) / 2;

	  m_rect.SetRect( gapX, gapY, gapX+W, gapY+H );
	   }
	break;
   case ftKeepSize:
	   {
		CPoint ptStart = clientRect.CenterPoint();
		ptStart.Offset( -imgW/2, -imgH/2 );
		ptStart.x = 
#if _MSC_VER < 1300
		std::_cpp_max<int>
#else
		std::max<int>
#endif
			(0, ptStart.x);
		ptStart.y =
#if _MSC_VER < 1300
		std::_cpp_max<int>
#else
		std::max<int>
#endif
			(0, ptStart.y);
		 m_rect = CRect( ptStart, CSize( imgW, imgH ) );
		 m_fScaleX = m_fScaleY = 1.0;

	   }
	   break;
  }
  m_imgWidth = imgW;
  m_imgHeight = imgH;
  m_bReady = true;
  ASSERT(!m_rect.IsRectEmpty());
}

CRect rectangle() const { return m_rect; }

protected:

CRect  rectangle(DWORD nX, DWORD nY)
{
	ASSERT(nX >=0 && nX < m_nX);
	ASSERT(nY >=0 && nY < m_nY);
	//ASSERT(!m_rect.IsRectEmpty());

	
	int nOffX = m_rect.left + (nX ) * m_rect.Width()/m_nX;
	int nOffY = m_rect.top + (nY )* m_rect.Height()/m_nY;

	return CRect( CPoint( nOffX, nOffY ), CSize(m_rect.Width()/m_nX, m_rect.Height()/m_nY) );
}

bool p2s( DWORD nX, DWORD nY,  const POINT & point, POINT& screen )
{

  if (m_bReady)
  {
	  CRect rc = rectangle(nX, nY);
    screen.x = rc.left + alib::Round(point.x * m_fScaleX);
    screen.y = rc.top + alib::Round(point.y * m_fScaleY);
  }
  else screen.x = (screen.y = 0);

	return m_rect.PtInRect( point ) == TRUE;
}

bool p2s( DWORD nX, DWORD nY, const Vec2f & point, POINT& screen )
{
  if (m_bReady)
  {
	  CRect rc = rectangle(nX, nY);
	 screen.x = rc.left + alib::Round( point.x * m_fScaleX );
	screen.y = rc.top + alib::Round( point.y * m_fScaleY);
  }
  else screen.x = (screen.y = 0);
	return m_rect.PtInRect( screen ) == TRUE;
}

bool s2p( DWORD nX, DWORD nY, const POINT & screen, POINT& point)
{
  if (m_bReady)
  {
	  CRect rc = rectangle(nX, nY);
    point.x = long((screen.x - rc.left+1) / m_fScaleX);
    point.y = long((screen.y - rc.top+1)  / m_fScaleY);
  }
  else point.x = (point.y = 0);
  return m_rect.PtInRect( point ) == TRUE;
}

bool s2p( DWORD nX, DWORD nY, const POINT & screen, Vec2f & point )
{
  if (m_bReady)
  {
	  CRect rc = rectangle(nX, nY);
    point.x = (float)((screen.x - rc.left) / m_fScaleX);
    point.y = (float)((screen.y - rc.top)  / m_fScaleY);
  }
  else point.x = (point.y = 0);
  return m_rect.PtInRect( CPoint( int(point.x), int(point.y)) ) == TRUE;
}

public:
bool ready() const
{
  return m_bReady;
}

protected:
RectImageLayout( int nGap /*= DEF_GAP*/) : m_ftMode( ftKeepSize ), m_nX(1), m_nY(1), m_nGap( nGap )
{
  clear();
}
public:
RectImageLayout(int x, int y, FillType ft, int nGap) : m_ftMode( ft ), m_nX(x), m_nY(y), m_nGap( nGap )
{
  clear();
}

};

class SparceRectImageLayout : public RectImageLayout
{
public:
	struct Cell
	{
		int	m_nX, m_nY;
		bool IsValid() const { return m_nX != -1 && m_nY != -1;}
		Cell(int x, int y) : m_nX(x), m_nY(y){}
		Cell() : m_nX(-1), m_nY(-1){}

		Cell(const Cell& c) :m_nX(c.m_nX), m_nY(c.m_nY){}
		Cell& operator =(const Cell&c)
		{
			m_nX = c.m_nX;
			m_nY = c.m_nY;
			return *this;
		}
		bool operator == (const Cell&c) const 
		{
			return m_nX == c.m_nX && m_nY == c.m_nY;
		}
	};


	typedef std::vector< Cell > IndexArray_t;

private:
	IndexArray_t	m_Cells;
public:
	CRect	rectangle( size_t nIndex )
	{
		ASSERT(nIndex < m_Cells.size());
		return RectImageLayout::rectangle( m_Cells[nIndex].m_nX, m_Cells[nIndex].m_nY  );
	}
	void	SetCells	( const IndexArray_t& Indices ) 
	{
//		ASSERT( m_nX * m_nY == Indices.size() );
		m_Cells = Indices;
	};

	const IndexArray_t& GetCells( ) const { return m_Cells;};

	void	SetDefaultCells()
	{
		//SetArray(nX, nY);
		m_Cells.resize( m_nX * m_nY);
		DWORD dwOff = 0;
		for(DWORD j = 0; j < m_nY; ++j)
			for(DWORD i = 0; i < m_nX; ++i , ++dwOff)
			{
				m_Cells[dwOff] = Cell(i, j);
			}
	}

	SparceRectImageLayout( int nGap = DEF_GAP) : RectImageLayout( nGap )
	{
		SetDefaultCells();
	}

	SparceRectImageLayout(int x, int y, FillType ft, int nGap ) : RectImageLayout(  x, y, ft, nGap)
	{
	  SetDefaultCells();
	}
	bool p2s( DWORD nIndex,  const POINT & point, POINT& screen )
	{
		return RectImageLayout::p2s( m_Cells[nIndex].m_nX, m_Cells[nIndex].m_nY, point, screen );
	}

	bool p2s( DWORD nIndex, const Vec2f & point, POINT& screen )
	{
	  return RectImageLayout::p2s( m_Cells[nIndex].m_nX, m_Cells[nIndex].m_nY, point, screen );
	}

	bool s2p( DWORD nIndex, const POINT & screen, POINT& point)
	{
	  return RectImageLayout::s2p( m_Cells[nIndex].m_nX, m_Cells[nIndex].m_nY, screen, point );
	}

	bool s2p( DWORD nIndex, const POINT & screen, Vec2f & point )
	{
	  return RectImageLayout::s2p( m_Cells[nIndex].m_nX, m_Cells[nIndex].m_nY, screen, point );
	}

	std::pair<bool, DWORD> IsInArray(DWORD x, DWORD y) const 
	{
		IndexArray_t::const_iterator itr = std::find(  m_Cells.begin(), m_Cells.end(), Cell( x, y ) );
		if( itr == m_Cells.end() )
		{
			return std::make_pair( false, 0 );
		}
		else
		{
			return std::make_pair( true, DWORD(itr - m_Cells.begin()) );
		}
	}
	
	std::pair<DWORD, DWORD> GetMaxNonEmptyRect() const
	{
		size_t FrameNum = GetCells().size();
		DWORD dwMaxX = 0, dwMaxY = 0;
		for(size_t i=0; i < FrameNum; ++i)
		{
			dwMaxX = 
#if _MSC_VER < 1300
		std::_cpp_max<DWORD>
#else
		std::max<DWORD>
#endif		
				(GetCells()[i].m_nX, dwMaxX);
			dwMaxY = 
#if _MSC_VER < 1300
		std::_cpp_max<DWORD>
#else
		std::max<DWORD>
#endif					
				( GetCells()[i].m_nY, dwMaxY);
		}
		return std::make_pair( dwMaxX + 1, dwMaxY + 1  );
	}
};


#endif // !defined(AFX_MULTIIMAGELAYOUT_H__5F83399D_A577_4361_820C_094D6F6148A5__INCLUDED_)
