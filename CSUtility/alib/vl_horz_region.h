#ifndef  VISLIB_HORIZONTAL_SCAN_REGION_H
#define  VISLIB_HORIZONTAL_SCAN_REGION_H

//=================================================================================================
/** \struct HScan.
    \brief  HScan. */
//=================================================================================================
struct HScan
{
  short x1, x2;    // semi-opened interval: [x1...x2)
  int   y;         // common y-coordinate of scan's points
};

typedef  std::vector<HScan>  HScanArr;

//=================================================================================================
// HRegion is a collection of horizontal scans, which cover internal area of boundary curve of this
// region. The curve, by definition, is passed through the pixels, but NOT between pixels as in
// some applications. HRegion may be created from enclosed (!) curve only.
//=================================================================================================
class /*ALIB_API*/ HRegion
{
 private:
  enum SegmentTypes          //  *               *         *
  {                          //  *               *         *
    CROSS = 1,               //    * * *           * * * *  
    TOUCH = 2                //          *                  
  };                         //          *                

  struct Segment             // Enhanced scan structure
  {
    int  x1, x2;             //!< segment occupies following coordinates: [x1,x2)
    int  y;                  //!< y-coordinate of scan-line, that contains the segment
    int  type;               //!< type is defined by neighbour points: CROSS or HORN
  };

  struct CmpX { bool operator()(const Segment & a, const Segment & b) { return (a.x1 < b.x1); } };
  struct CmpY { bool operator()(const Segment & a, const Segment & b) { return (a.y  < b.y ); } };

 protected:
  HScanArr  m_scans;         //!< array of horizontal scans that cover this region

 private:
  bool remove_overlapping_of_scans();

 public:
  HRegion();
  HRegion( const HRegion & rgn );
  virtual ~HRegion();

  bool clear( bool bCompleteClear = false );
  void copy( const HRegion & r );
  bool create( const Vec2iArr & curve );
  void reserve( unsigned int nScan );

  bool inside( Vec2i pt ) const;
  bool center( Vec2f & massCenter ) const;
  int  area() const;

  const HScanArr & operator()() const { return m_scans; }

#ifdef _WINDOWS
  void draw( CDC * pDC, Vec2i offset, COLORREF color ) const;
#endif // _WINDOWS
};

#endif // VISLIB_HORIZONTAL_SCAN_REGION_H


