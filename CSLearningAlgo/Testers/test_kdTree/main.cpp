#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

template< class DISTANCE, int DIM >
class SampleVector
{
public:
  typedef  unsigned char               value_type;
  typedef  unsigned int                precise_type;
  typedef  DISTANCE                    distance_type;
  typedef  SampleVector<DISTANCE,DIM>  this_type;

private:
  value_type m_vec[DIM];

public:
  distance_type distance( const this_type & x ) const
  {
    precise_type sum = 0;
    for (int i = 0; i < DIM; i++)
    {
      precise_type c1 = m_vec[i];
      precise_type c2 = x.m_vec[i];
      sum += ((c2 >= c1) ? (c2-c1) : (c1-c2));
    }
    return ((distance_type)sum);
  }

  value_type & operator[]( precise_type n )
  {
    ASSERT( n < (precise_type)DIM );
    return m_vec[n];
  }

  value_type operator[]( precise_type n ) const
  {
    ASSERT( n < (precise_type)DIM );
    return m_vec[n];
  }

  int size() const
  {
    return DIM;
  }

  void fill( precise_type vfill = 0 )
  {
    memset( m_vec, vfill, sizeof(m_vec) );
  }

  SampleVector()
  {
    ASSERT( (0 < DIM) && (DIM < (1<<15)) );
    fill();
  }
};


template< class SAMPLE >
void RandomFill( SAMPLE & s )
{
  for (int i = 0, n = s.size(); i < n; i++)
  {
    s[i] = (unsigned char)(rand() % 256);
  }
}


CWinApp theApp;

int main( int argc, char * argv[] )
{
  argc;argv;
  srand( (unsigned int)(time(0)) );
  if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
    return 1;

  typedef  unsigned int  DISTANCE;

  const int DIM = (1<<12);
  const int SAMPLE_NUM = 10000;
  const int ITER_NUM = 1000;

  typedef  SampleVector<DISTANCE,DIM>            Sample;
  typedef  std::vector<Sample>                   SampleArr;
  typedef  kDimTree<Sample,Sample,DISTANCE,DIM>  kDTree;

  ATimer    timer;
  SampleArr sampleArr( SAMPLE_NUM );
  kDTree    tree;
  Sample    query;
  double    directEnumTime = 0.0;
  double    treeTime = 0.0;
  double    nComparisons = 0;
  UByteArr  tempBuffer;

  for (int i = 0; i < SAMPLE_NUM; i++)
  {
    RandomFill( sampleArr[i] );
  }

  if (!(tree.Create( &(*(sampleArr.begin())), &(*(sampleArr.end())) )))
  {
    printf( "Failed to create kD-tree\n" );
    return 1;
  }
  printf( "kD-tree's dimension = %d\n", DIM );
  printf( "sample number = %d\n", SAMPLE_NUM );
  printf( "number of test iterations to be made = %d\n", ITER_NUM );

  for (int iter = 0; iter < ITER_NUM; iter++)
  {
    RandomFill( query );

    timer.Start();
    const Sample * pCandidate = 0;
    DISTANCE       candidateDistance = 0;
    {
      DISTANCE threshold = std::numeric_limits<DISTANCE>::max();
      for (int i = 0; i < SAMPLE_NUM; i++)
      {
        DISTANCE d = query.distance( sampleArr[i] );
        if (d < threshold)
        {
          threshold = d;
          pCandidate = &(sampleArr[i]);
          candidateDistance = d;
        }
      }
    }
    timer.Stop();
    directEnumTime += timer.Time();

    DISTANCE bestDist = 0;
    int      nComp = 0;

    timer.Start();
    const Sample * pSample = tree.FindNearestSample( query, std::numeric_limits<DISTANCE>::max(),
                                                     tempBuffer, &bestDist, &nComp );
    timer.Stop();
    treeTime += timer.Time();
    nComparisons += nComp;

    if ((pCandidate != pSample) && (candidateDistance != bestDist))
      printf( "Error\n" );
    printf( "." );
  }

  nComparisons /= ITER_NUM;
  printf( "\nkD-tree time: %f\ndirect enumeration time: %f\n", treeTime, directEnumTime );
  printf( "Mean comparison number in kD-tree: %d\n", ((int)(nComparisons+0.5)) );
  getch();
  return 0;
}

