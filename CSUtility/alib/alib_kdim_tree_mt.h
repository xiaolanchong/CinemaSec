/******************************************************************************
  alib_kdim_tree.h
  ---------------------
  begin     : 1998
  modified  : 23 Jul 2005
  author(s) : Albert Akhriev
  copyright : Albert Akhriev
  email     : aaah@mail.ru, Albert.Akhriev@biones.com, aaahaaah@hotmail.com
******************************************************************************/

#pragma once

//=================================================================================================
/** \class kDimTree.
    \brief K-dimensional Tree.

  Description sources:
  1. Z.Zhang, "Iterative Point Matching for Registration of Free-Form
     Curves and Surfaces," IJCV, 13:2, 119-152, 1994.
  2. F.Preparata, and M.Shamos, "Computational Geometry, An Introduction,"
     Springer, Berlin, Heidelberg, New-York, 1986.

  'SAMPLE'   - type of sample, must be a kind of vector type with operator[] and method 'size()'.
  'QUERY'    - query type (usually 'QUERY' == 'SAMPLE'), user should define method
               'distance( const SAMPLE & )' for the 'QUERY' type.
  'DISTANCE' - type of distance between samples, e.g. DISTANCE = double.
  'DIM'      - dimension of a tree (dimension of samples, SAMPLE.size() == DIM). */
//=================================================================================================
template< class SAMPLE, class QUERY, class DISTANCE, int DIM >
class kDimTree
{
private:
  typedef  kDimTree<SAMPLE,QUERY,DISTANCE,DIM>  this_type;

  //============================
  /** \brief Node of kD-tree. */
  //============================
  struct Node
  {
    Node         * pL;  //!< left child node
    Node         * pR;  //!< right child node
    const SAMPLE * pS;  //!< pointer to the sample that splits space into two parts
    int            dir; //!< direction of space cutting (index of coord. axis that is perpendicular to a cutting plane)
  };

  typedef  std::vector<Node>           NodeArr;
  typedef  std::vector</*const*/ SAMPLE*>  SamplePtrArr;

  NodeArr          m_nodes;       //!< the set of tree's nodes
  const Node *     m_pRoot;       //!< root node of this tree
  CCriticalSection m_dataLocker;  //!< object locks/unlocks the data being transferred between threads

private:
//-------------------------------------------------------------------------------------------------
/** \brief Function sorts an array in ascendant numerical order by Shell's method. */
//-------------------------------------------------------------------------------------------------
void Sort( SAMPLE ** arr, int length, int dir )
{
  for (int gap = (length>>1); gap > 0; gap >>= 1)
  {
    for (int i = gap; i < length; i++)
    {
      for (int j = (i-gap); (j >= 0) && ((*(arr[j]))[dir] > (*(arr[j+gap]))[dir]);)
      {
        SAMPLE * p = arr[j];
        arr[j] = arr[j+gap];
        arr[j+gap] = p;
        j -= gap;
      }
    }
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function recursively splits space into two halfs (direction of splitting
           is cyclically changed on each level). */
//-------------------------------------------------------------------------------------------------
Node * CreateBranch( SAMPLE ** pa, int length, const Node * pParent )
{
  Node * pn = 0;
  if (length > 0)
  {
    int half = (length >> 1);
    int dir = (pParent != 0) ? ((pParent->dir+1) % DIM) : 0;
    Sort( pa, length, dir );
    m_nodes.push_back( Node() );
    pn = &(m_nodes.back());  // allocate new node in the buffer
    pn->dir = dir;
    pn->pS = pa[half];
    if (length > 1)
    {
      pn->pL = CreateBranch( pa, half, pn );
      pn->pR = CreateBranch( pa + (half+1), length - (half+1), pn );
    }
    else pn->pL = (pn->pR = 0);
  }
  return pn;
}

public:
//-------------------------------------------------------------------------------------------------
/** \brief Function recursively creates tree from the input array of samples.

  \param  first  begin iterator of the input array.
  \param  last   end iterator of the input array.
  \return        Ok = true. */
//-------------------------------------------------------------------------------------------------
bool Create( const SAMPLE * first, const SAMPLE * last )
{
  CSingleLock lock( &m_dataLocker, TRUE );

  Clear();
  if (first >= last)
    return false;
  ASSERT( (DIM > 0) && (first->size() == DIM) );

  // Allocate nodes.
  int N = (int)(std::distance( first, last ));
  m_nodes.clear();
  m_nodes.reserve( N );

  // Create tree structure using sample pointers instead of samples themselves.
  SamplePtrArr ptrs( N );
  for (int k = 0; k < N; k++)
    ptrs[k] = const_cast<SAMPLE*>( first++ );

  m_pRoot = CreateBranch( &(*(ptrs.begin())), N, 0 );
  return true;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function recursively searches for the nearest sample to the query one,
           using distance threshold.

  \param  query       the query sample
  \param  threshold   the threshold on maximal distance between query sample and any tree's sample.
  \param  tempBuffer  temporal buffer used to simulate stack.
  \param  pBestDist   pointer to the storage that receives the minimal achieved distance or 0.
  \param  pCompNum    pointer to the storage that receives the number of made comparisons or 0.
  \return             Ok = pointer to the nearest tree's sample, otherwise 0. */
//-------------------------------------------------------------------------------------------------
const SAMPLE * FindNearestSample( const QUERY & query,
                                  DISTANCE      threshold,
                                  UByteArr    & tempBuffer,
                                  DISTANCE    * pBestDist = 0,
                                  int         * pCompNum = 0 ) const
{
  CSingleLock    lock( &((const_cast<this_type*>( this ))->m_dataLocker) , TRUE );
  const SAMPLE * pBest = 0;
  const Node   * pn = 0;
  int            count = 0, comparisonNum = 0, N = (int)(m_nodes.size());
  int            stackSize = N*sizeof(void*);

  if ((m_pRoot == 0) || m_nodes.empty())
    return 0;

  if ((int)(tempBuffer.size()) < stackSize)
    tempBuffer.resize( stackSize );

  const Node ** stack = reinterpret_cast<const Node**>( &(tempBuffer[0]) );
  ASSERT( count < N );
  stack[count++] = m_pRoot;                           // push the root node in the stack

  // Recursive search. Recursion is simulated by the means of separate stack "stack[]".
  while (count > 0)
  {
    ASSERT( count > 0 );
    pn = stack[--count];                              // pop the top node

    DISTANCE c1 = query[ pn->dir ];
    DISTANCE c2 = (*(pn->pS))[ pn->dir ];
    DISTANCE dc = (c2 >= c1) ? (c2-c1) : (c1-c2);     // DISTANCE may be unsigned!

    if (dc < threshold)
    {
      DISTANCE t = query.distance( *(pn->pS) );
      if (t < threshold)
      {
        pBest = pn->pS;
        threshold = t;
      }
    }
    ++comparisonNum;

    if ((pn->pL != 0) && (c1 < (c2+threshold)))       // launch searching in the left child node
    {
      ASSERT( count < N );
      stack[count++] = pn->pL;                        // push the left node in the stack
    }

    if ((pn->pR != 0) && (c2 < (c1+threshold)))       // launch searching in the right child node
    {
      ASSERT( count < N );
      stack[count++] = pn->pR;                        // push the right node in the stack
    }
  }

  if (pCompNum  != 0) (*pCompNum) = comparisonNum;
  if (pBestDist != 0) (*pBestDist) = threshold;
  return pBest;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function clears tree's structure. */
//-------------------------------------------------------------------------------------------------
void Clear()
{
  CSingleLock lock( &m_dataLocker, TRUE );
  NodeArr     empty;

  m_nodes.swap( empty );
  m_pRoot = 0;
}


//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
kDimTree()
{
  Clear();
}


//-------------------------------------------------------------------------------------------------
/** \brief Destructor. */
//-------------------------------------------------------------------------------------------------
~kDimTree()
{
  Clear();
}


//-------------------------------------------------------------------------------------------------
/** \brief Function returns nonzero for initialized tree. */
//-------------------------------------------------------------------------------------------------
bool IsEmpty() const
{
  return ((m_pRoot == 0) || m_nodes.empty());
}

};

