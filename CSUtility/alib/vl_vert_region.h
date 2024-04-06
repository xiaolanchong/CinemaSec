#ifndef  VISLIB_VERTICAL_SCAN_REGION_H
#define  VISLIB_VERTICAL_SCAN_REGION_H

//=================================================================================================
/** \struct VScan.
    \brief  VScan. */
//=================================================================================================
struct VScan
{
  short y1, y2;    //!< semi-opened interval: [y1...y2)
  int   x;         //!< common abscissa of scan's points
};

typedef  std::vector<VScan>  VScanArr;


//=================================================================================================
/** \class VRegion.
    \brief VRegion is a collection of vertical scans that cover
           internal area of boundary curve of a region.

  The boundary curve, by definition, is passed through the pixel centers, but NOT between pixels
  as in some applications, i.e. the boundary curve occupies the skin points of a region.
  VRegion may be created from enclosed (!) curve only. */
//=================================================================================================
class /*ALIB_API*/ VRegion
{
 private:
  enum SegmentTypes          //  * *               * *
  {                          //      *                 *
    CROSS = 1,               //      *                 *
    TOUCH = 2                //      *                 *
  };                         //        * *         * *

  struct Segment             // Enhanced scan structure
  {
    int  y1, y2;             //!< segment occupies following coordinates: [y1,y2)
    int  x;                  //!< abscissa of a scan-line, that contains the segment
    int  type;               //!< type is defined by neighbour points: CROSS or TOUCH
  };

  struct CmpX { bool operator()( const Segment & a, const Segment & b ) { return (a.x  < b.x ); } };
  struct CmpY { bool operator()( const Segment & a, const Segment & b ) { return (a.y1 < b.y1); } };

 protected:
  VScanArr  m_scans;         //!< array of vertical scans that cover this region

 private:
  bool remove_overlapping_of_scans();

 public:
  VRegion();
  VRegion( const VRegion & rgn );
  virtual ~VRegion();

  bool clear( bool bCompleteClear = false );
  void copy( const VRegion & r );
  bool create( const Vec2iArr & curve );
  void reserve( unsigned int nScan );

  bool inside( Vec2i pt ) const;
  bool center( Vec2f & massCenter ) const;
  int  area() const;

  const VScanArr & operator()() const { return m_scans; }

#ifdef _WINDOWS
  void draw( CDC * pDC, Vec2i offset, COLORREF color ) const;
#endif // _WINDOWS
};

#endif // VISLIB_VERTICAL_SCAN_REGION_H


