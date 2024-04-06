/****************************************************************************
  live_snake.h
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : Albert.Akhriev@biones.com
****************************************************************************/

#ifndef  VISLIB_LIVE_SNAKE_H
#define  VISLIB_LIVE_SNAKE_H

//=================================================================================================
/** \class LiveSnake.
    \brief Opened snake implementation with capability to continue snake
           adjusting starting from the current position. */
//=================================================================================================
class CS_UTIL_API LiveSnake  
{
private:
  Vec2fArr  m_curve;    //!< current snake
  FloatArr  m_a;        //!< 
  FloatArr  m_c;        //!< 
  FloatArr  m_d;        //!< 
  FloatArr  m_e;        //!< 
  FloatArr  m_f;        //!< 
  FloatArr  m_bx;       //!< 
  FloatArr  m_by;       //!< 
  FloatArr  m_rx;       //!< 
  FloatArr  m_ry;       //!< 
  DoubleArr m_buffer;   //!< 

public:                 // P A R A M E T E R S:
  float  m_alpha;       //!< 
  float  m_beta;        //!< 
  float  m_minMove;     //!< 

private:
  void CalcSnake( const Vec2fImg & energyGrads );

public:
  LiveSnake();
  virtual ~LiveSnake();

  void SetDefaultParam();
  void ValidateParam();

  bool Clear();
  bool Run( const Vec2fImg & energyGrads, Vec2f p1, Vec2f p2, bool bContinue = false );

  const Vec2fArr & GetCurve() const { return m_curve; }
  void EraseEndPoints( unsigned n );
  void MakeStraightLine( Vec2f p1, Vec2f p2 );
  void GetEquilibriumPoint( const Vec2fImg & energyGrads, Vec2f & pt );
};

#endif   // VISLIB_LIVE_SNAKE_H


