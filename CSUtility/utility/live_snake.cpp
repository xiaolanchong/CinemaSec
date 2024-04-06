/****************************************************************************
  live_snake.cpp
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : Albert.Akhriev@biones.com
****************************************************************************/

#include "stdafx.h"
#include "live_snake.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


LiveSnake::LiveSnake()
{
  Clear();
  SetDefaultParam();
  m_curve.reserve( 1<<12 );
  m_a.reserve( 1<<12 );
  m_c.reserve( 1<<12 );
  m_d.reserve( 1<<12 );
  m_e.reserve( 1<<12 );
  m_f.reserve( 1<<12 );
  m_bx.reserve( 1<<12 );
  m_by.reserve( 1<<12 );
  m_rx.reserve( 1<<12 );
  m_ry.reserve( 1<<12 );
  m_buffer.reserve( 1<<15 );
}


LiveSnake::~LiveSnake()
{
  Clear();
}


bool LiveSnake::Clear()
{
  m_curve.clear();
  m_a.clear();
  m_c.clear();
  m_d.clear();
  m_e.clear();
  m_f.clear();
  m_bx.clear();
  m_by.clear();
  m_rx.clear();
  m_ry.clear();
  m_buffer.clear();
  return false;
}


void LiveSnake::SetDefaultParam()
{
  m_alpha   = 1.00f;
  m_beta    = 0.50f;
  m_minMove = 0.01f;
}


void LiveSnake::ValidateParam()
{
  m_alpha   = alib::Limit( m_alpha, 0.0f, 10.0f );
  m_beta    = alib::Limit( m_beta,  0.0f, 10.0f );
  m_minMove = alib::Limit( m_minMove, 0.001f, 0.1f );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function calculates opened snake.

  Function will either proceed from the current curve's location  prolonged to the second end point
  ('bContinue' == true) or start from the straight line given by end points 'p1' and 'p2'
  ('bContinue' == false). In both cases 'p1' and 'p2' should be actual end points.

  \param  energyGrads  the map of potential energy gradient vectors.
  \param  p1           the first fixed snake's end point.
  \param  p2           the second fixed snake's end point.
  \param  bContinue    if true, then proceed from the current curve. */
//-------------------------------------------------------------------------------------------------
bool LiveSnake::Run( const Vec2fImg & energyGrads, Vec2f p1, Vec2f p2, bool bContinue )
{
  USES_CONVERSION;

  try
  {
    ValidateParam();
    if (energyGrads.empty())
      return Clear();

    if (!bContinue)
    {
      Clear();
      m_curve.push_back( p1 );
      m_curve.push_back( p2 );
    }
    vislib::ResampleCurve( m_curve, 1.0, false );

    if (m_curve.size() < 5)
    {
      MakeStraightLine( p1, p2 );
    }
    else
    {
      m_curve.front() = p1;
      m_curve.back() = p2;
      CalcSnake( energyGrads );
    }
  }
  catch (std::exception & e)
  {
    alib::ErrorMessage( CString( e.what() ) );
    Clear();
    return false;
  }
  catch (...)
  {
    alib::ErrorMessage( ALIB_UNSUPPORTED_EXCEPTION );
    Clear();
    return false;
  }
  return true;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function calculates opened snake given initial curve's location.

  \param  energyGrads  the map of potential energy gradient vectors. */
//-------------------------------------------------------------------------------------------------
void LiveSnake::CalcSnake( const Vec2fImg & energyGrads )
{
  const int    MAX_ITER_NUM = 150;
  const double MAX_MOVE = 0.1;
  int          n = (int)(m_curve.size());
  Vec2f *      curve = &(*(m_curve.begin()));
  Vec2f        grad;
  double       move = FLT_MAX;
  float        gamma = 1.0f + 1.5f*(2.0f*m_alpha+8.0f*m_beta);
  float        subDiagValue = -(m_alpha + 4.0f*m_beta);
  float        diagValue = 2.0f*m_alpha + 6.0f*m_beta + gamma;

  // Resize temporal buffers.
  m_a.resize( n );
  m_c.resize( n );
  m_d.resize( n );
  m_e.resize( n );
  m_f.resize( n );
  m_bx.resize( n );
  m_by.resize( n );
  m_rx.resize( n );
  m_ry.resize( n );
  m_buffer.resize( 9*n );

  // Compute matrix elements.
  {
    ASSERT( n >= 5 );
    for (int i = 0; i < n; i++)
    {
      m_e[i] = m_f[i] = m_beta;
      m_a[i] = m_c[i] = subDiagValue;
      m_d[i] = diagValue;
    }
  }

  // Correct to meet boundary conditions.
  m_c[0] = m_f[0] = m_e[n-3] = m_a[n-2] = 0.0f;
  m_d[0] = m_d[n-1] = 1.0f;
  m_a[0] = m_c[n-2] = -(m_alpha + 3.0f*m_beta);

  // Snake iterations.
  for (int nIter = 0; (nIter < MAX_ITER_NUM) && (move > m_minMove); nIter++)
  {
    // Prepare right-size vector of snake equation.
    {
      for (int i = 1; i < (n-1); i++)
      {
        grad = energyGrads.bilin_at( curve[i] );
        m_bx[i] = gamma*(curve[i]).x - grad.x;
        m_by[i] = gamma*(curve[i]).y - grad.y;
      }
    }

    // End points are fixed.
    m_bx[0] = (curve[0]).x;   m_bx[n-1] = (curve[n-1]).x;
    m_by[0] = (curve[0]).y;   m_by[n-1] = (curve[n-1]).y;

    // Solve snake equation.
    matrixlib::SolvPentaDiagLinSys( n,
      &(*(m_e.begin())), &(*(m_a.begin())), &(*(m_d.begin())), &(*(m_c.begin())), &(*(m_f.begin())),
      &(*(m_bx.begin())), &(*(m_rx.begin())),
      &(*(m_buffer.begin())), &(*(m_buffer.end())) );

    matrixlib::SolvPentaDiagLinSys( n,
      &(*(m_e.begin())), &(*(m_a.begin())), &(*(m_d.begin())), &(*(m_c.begin())), &(*(m_f.begin())),
      &(*(m_by.begin())), &(*(m_ry.begin())),
      &(*(m_buffer.begin())), &(*(m_buffer.end())) );

    // Calculate maximal movement on this iteration made by any point.
    {
      move = 0.0;
      for (int i = 1; i < (n-1); i++)
      {
        move = std::max( move, fabs( (double)(m_rx[i] -= (curve[i]).x) ) );
        move = std::max( move, fabs( (double)(m_ry[i] -= (curve[i]).y) ) );
      }
    }

    // Move curve in new position. Restrict movement to MAX_MOVE pixels.
    if (move > MAX_MOVE)
    {
      double scale = MAX_MOVE/move;
      for (int i = 1; i < (n-1); i++)
      {
        (curve[i]).x += m_rx[i] * (float)scale;
        (curve[i]).y += m_ry[i] * (float)scale;
      }
    }
    else if (move > FLT_EPSILON)
    {
      for (int i = 1; i < (n-1); i++)
      {
        (curve[i]).x += m_rx[i];
        (curve[i]).y += m_ry[i];
      }
    }
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function initializes snake with equally spaced
           points of straight line between two end ones.

  \param  p1  the first end point.
  \param  p2  the second end point. */
//-------------------------------------------------------------------------------------------------
void LiveSnake::MakeStraightLine( Vec2f p1, Vec2f p2 )
{
  Vec2f  chord = p2-p1;
  double length = chord.normalize();
  
  m_curve.clear();
  if (length < 1.0)
  {
    if (length >= 0.5)
    {
      m_curve.push_back( p1 );
      m_curve.push_back( p2 );
    }
  }
  else
  {
    int    n = std::max( ALIB_ROUND(length), 1 );
    double t = 0.0, dt = length/n;

    for (int i = 0; i <= n; i++)
    {
      m_curve.push_back( p1 + chord*(float)t );
      t += dt;
    }
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function finds nearest edge point (equilibrium point) starting from specified one.

  \param  energyGrads  the map of potential energy gradient vectors.
  \param  pt           the starting point. */
//-------------------------------------------------------------------------------------------------
void LiveSnake::GetEquilibriumPoint( const Vec2fImg & energyGrads, Vec2f & pt )
{
  const int    MAX_ITER_NUM = 150;
  const double MAX_MOVE = 0.1;
  Vec2f        cur, pre;
  double       scale = 1.0, move = FLT_MAX;
  
  if (energyGrads.empty())
    return;

  pre = energyGrads.bilin_at( pt );
  move = pre.normalize();
  if (move < FLT_EPSILON)
    return;
  pre *= (float)(MAX_MOVE/move);

  for (int nIter = 0; (nIter < MAX_ITER_NUM) && (move > m_minMove); nIter++)
  {
    cur = energyGrads.bilin_at( pt );
    move = cur.length();
    
    if (move > MAX_MOVE)
      cur *= (float)(MAX_MOVE/move);

    if ((pre & cur) < 0)
      scale *= 0.5;

    cur *= (float)scale;
    pt -= cur;
    pre = cur;
    move = cur.length();
  }
}


//-------------------------------------------------------------------------------------------------
// Function removes last 'n' points from the snake curve.
//-------------------------------------------------------------------------------------------------
void LiveSnake::EraseEndPoints( unsigned int n )
{
  m_curve.erase( m_curve.end() - std::min( m_curve.size(), n ), m_curve.end() );
}


