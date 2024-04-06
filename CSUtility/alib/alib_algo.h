#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
// F U N C T I O N S:
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// ITER max_fabs_element(ITER F, ITER L)
// ITER min_fabs_element(ITER F, ITER L)
//
// void Copy(II sF, II sL, OI dF, OI dL)
// void Copy(II sF, II sL, OI dF, OI dL, TYPE type)
// void Copy(II sF, II sL, OI dF, OI dL, unsigned srcInc, unsigned dstInc)
//
// void addvec(InType1 * F1, InType1 * L1, InType2 * F2, InType2 * L2, OutType * F3, OutType * L3)
// void subvec(InType1 * F1, InType1 * L1, InType2 * F2, InType2 * L2, OutType * F3, OutType * L3)
// void multiply(IT F, IT L, MULTIPLIER m)
// void module(ITER F, ITER L)
// T    mean_digit(T * F, T * L)
//
// void init_ptr_container(II sF, II sL, OI dF, OI dL)
//
// TYPE * lower_nearest_in_ascendant_sorted(TYPE * first, TYPE * last, const TYPE & value)
//
///////////////////////////////////////////////////////////////////////////////////////////////////

namespace alib
{

//-------------------------------------------------------------------------------------------------
/** \brief Function searches for element of a sequence with maximal absolute value.

  N O T E: Use function for a sequence of digits of floating type only.

  \param  F  begin iterator of input sequence.
  \param  L  end iterator of input sequence.
  \return    Ok = pair<maximal absolute value, iterator of maximal value>, otherwise pair<0,L>.*/
//-------------------------------------------------------------------------------------------------
template<class ITER>
inline std::pair<double,ITER> max_fabs_element(ITER F, ITER L)
{
  ITER   it = F;
  double max = 0.0;

  if (F != L)
  {
    max = fabs( (double)(*F) );
    while(++F != L)
    {
      double t = fabs( (double)(*F) );
      if (max < t)
      {
        max = t;
        it = F;
      }
    }
  }
  return std::pair<double,ITER>( max, it );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function searches for element of a sequence with minimal absolute value.

  N O T E: Use function for a sequence of digits of floating type only.

  \param  F  begin iterator of input sequence.
  \param  L  end iterator of input sequence.
  \return    Ok = pair<minimal alsolute value, iterator of minimal value>, otherwise pair<0,L>.*/
//-------------------------------------------------------------------------------------------------
template<class ITER>
inline std::pair<double,ITER> min_fabs_element( ITER F, ITER L )
{
  ITER   it = F;
  double min = 0.0;

  if (F != L)
  {
    min = fabs( (double)(*F) );
    while(++F != L)
    {
      double t = fabs( (double)(*F) );
      if (min > t)
      {
        min = t;
        it = F;
      }
    }
  }
  return std::pair<double,ITER>( min, it );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function copies the source sequence into destination one with size checking.

  \param  sF  begin iterator of source sequence.
  \param  sL  end iterator of source sequence.
  \param  dF  begin iterator of destination sequence.
  \param  dL  end iterator of destination sequence. */
//-------------------------------------------------------------------------------------------------
template< class II, class OI >
void Copy( II sF, II sL, OI dF, OI dL )
{
  sL;
  while (dF != dL)
  {
    ASSERT( sF != sL );

    (*dF) = (*sF);
    ++dF;
    ++sF;
  }
  ASSERT( sF == sL );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function copies the source sequence into destination one with type convertion.

  \param  sF    begin iterator of source sequence.
  \param  sL    end iterator of source sequence.
  \param  dF    begin iterator of destination sequence.
  \param  dL    end iterator of destination sequence.
  \param  type  parameter needs to be known for type convertion, the value is <i>ignored</i>. */
//-------------------------------------------------------------------------------------------------
template< class II, class OI, class TYPE >
void Copy( II sF, II sL, OI dF, OI dL, TYPE type )
{
  sL;type;
  while (dF != dL)
  {
    ASSERT( sF != sL );

    (*dF) = (TYPE)(*sF);
    ++dF;
    ++sF;
  }
  ASSERT( sF == sL );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function copies the source sequence into destination one with type convertion.

  \param  s  source sequence.
  \param  d  destination sequence. */
//-------------------------------------------------------------------------------------------------
template< class SOURCE, class DESTINATION >
void Copy( SOURCE & s, DESTINATION & d )
{
  typename SOURCE::const_iterator sF = s.begin();
  typename DESTINATION::iterator  dF = d.begin();
  typename DESTINATION::iterator  dL = d.end();

  ASSERT( (int)(d.size()) == (int)(s.size()) );

  while (dF != dL)
  {
    (*dF) = (typename DESTINATION::value_type)(*sF);
    ++dF;
    ++sF;
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function copies the source sequence into destination one with optional incrementations.

  \param  sF    begin iterator of source sequence.
  \param  sL    end iterator of source sequence.
  \param  dF    begin iterator of destination sequence.
  \param  dL    end iterator of destination sequence.
  \param  sInc  increment of source iterator.
  \param  dInc  increment of destination iterator.
  \param  type  parameter needs to be known for type convertion, the value is <i>ignored</i>. */
//-------------------------------------------------------------------------------------------------
template< class II, class OI, class TYPE >
void Copy( II sF, II sL, OI dF, OI dL, unsigned int sInc, unsigned int dInc, TYPE type )
{
  sL;type;
  while (dF != dL)
  {
    ASSERT( sF != sL );

    (*dF) = (TYPE)(*sF);
    dF += dInc;
    sF += sInc;
  }
  ASSERT( sF == sL );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function adds multiplied source sequence to the destination one:<br>
           destination[i] += source[i] * multiplier.

  \param  source       source container.
  \param  destination  destination container.
  \param  multiplier	 multiplier.
  \return              reference to the destination container. */
//-------------------------------------------------------------------------------------------------
template< class ST, class DT, class MT >
DT & AddMultiplied( const ST & source, DT & destination, MT multiplier )
{
  ASSERT( (int)(destination.size()) == (int)(source.size()) );

  typename DT::iterator       d = destination.begin();
  typename DT::iterator       e = destination.end();
  typename ST::const_iterator s = source.begin();

  while (d != e)
  {
    (*d) += (typename DT::value_type)((*s) * multiplier);
    ++d;
    ++s;
  }
  return destination;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function calculates sum of squared items of input sequence.
    \param  sequence  any sequence: matrix, vector, list, etc.
    \param  sum       parameter is needed to identify the type of sum, its value is IGNORED!
    \return           sum of squared items. */
//-------------------------------------------------------------------------------------------------
template< class II, class SUMTYPE >
SUMTYPE SumOfSquares( II F, II L, SUMTYPE )
{
  return std::inner_product( F, L, F, ((SUMTYPE)0) );
}



//-------------------------------------------------------------------------------------------------
/** \brief Function calculates sum of squared items of input container.
    \param  container  any container: matrix, vector, list, etc.
    \param  sum        parameter is needed to identify the type of sum, its value is IGNORED!
    \return            sum of squared items. */
//-------------------------------------------------------------------------------------------------
template< class CONTAINER, class SUMTYPE >
SUMTYPE SumOfSquares( CONTAINER x, SUMTYPE )
{
  return std::inner_product( x.begin(), x.end(), x.begin(), ((SUMTYPE)0) );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function calculates square root of sum of squared items of input sequence.
    \param  sequence  any sequence: matrix, vector, list, etc.
    \return           length = square root of sum of squared items. */
//-------------------------------------------------------------------------------------------------
template< class II >
double VectorLength(II F, II L)
{
  return sqrt( std::inner_product( F, L, F, double() ) );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function calculates square root of sum of squared items of input container.
    \param  container  any container: matrix, vector, list, etc.
    \return            length = square root of sum of squared items. */
//-------------------------------------------------------------------------------------------------
template< class CONTAINER >
double VectorLength(CONTAINER x)
{
  return sqrt( std::inner_product( x.begin(), x.end(), x.begin(), double() ) );
}

/*
//-------------------------------------------------------------------------------------------------
// Add two arrays: (*F3++) = (OutType)((*F1++) + (*F2++))
//-------------------------------------------------------------------------------------------------
template< class InType1, class InType2, class OutType >
void addvec(InType1 * F1, InType1 * L1, InType2 * F2, InType2 * L2, OutType * F3, OutType * L3)
{
  ASSERT(std::distance(F3,L3) == std::distance(F1,L1));  L1;
  ASSERT(std::distance(F3,L3) == std::distance(F2,L2));  L2;

  while(F3 != L3)
  {
    (*F3) = (OutType)((*F1) + (*F2));
    F1++;
    F2++;
    F3++;
  }
}


//-------------------------------------------------------------------------------------------------
// Subtract two array: (*F3++) = (OutType)((*F1++) - (*F2++))
//-------------------------------------------------------------------------------------------------
template< class InType1, class InType2, class OutType >
void subvec(InType1 * F1, InType1 * L1, InType2 * F2, InType2 * L2, OutType * F3, OutType * L3)
{
  ASSERT(std::distance(F3,L3) == std::distance(F1,L1));  L1;
  ASSERT(std::distance(F3,L3) == std::distance(F2,L2));  L2;

  while(F3 != L3)
  {
    (*F3) = (OutType)((*F1) - (*F2));
    F1++;

    F2++;
    F3++;
  }
}
*/

//-------------------------------------------------------------------------------------------------
/** \brief Function fills a sequence by specified value.

  \param  x      sequence to be filled.
  \param  vfill  fill value. */
//-------------------------------------------------------------------------------------------------
template< class CONTAINER, class FILL_VALUE >
void Fill( CONTAINER & x, FILL_VALUE vfill )
{
  typename CONTAINER::iterator F = x.begin();
  typename CONTAINER::iterator L = x.end();

  for (; F != L; ++F)
  {
    (*F) = vfill;
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function multiplies all elements on the sequence by specified value.

  \param  F  begin iterator of the sequence.
  \param  L  end iterator of the sequence.
  \param  m  multiplier. */
//-------------------------------------------------------------------------------------------------
template< class II, class MULTIPLIER >
void Multiply( II F, II L, MULTIPLIER m )
{
  for (; F != L; ++F)
  {
    (*F) *= m;
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function multiplies all elements on the sequence by specified value.

  \param  x  sequence to be multiplied.
  \param  m  multiplier. */
//-------------------------------------------------------------------------------------------------
template< class CONTAINER, class MULTIPLIER >
void Multiply( CONTAINER & x, MULTIPLIER m )
{
  typename CONTAINER::iterator F = x.begin();
  typename CONTAINER::iterator L = x.end();

  for (; F != L; ++F)
  {
    (*F) *= (typename CONTAINER::value_type)m;
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function changes sign of all elements of the sequence.

  \param  F  begin iterator of the sequence.
  \param  L  end iterator of the sequence. */
//-------------------------------------------------------------------------------------------------
template< class II >
void Negate( II F, II L )
{
  for (; F != L; ++F)
  {
    (*F) = -(*F);
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function applies unary function to the source sequence and stores result in
           the destination one.
    \param  src   source sequence (any container).
    \param  dst   destination sequence (any container).
    \param  func  unary function. */
//-------------------------------------------------------------------------------------------------
template< class IC, class OC, class UF >
inline void Transform( const IC & src, OC & dst, UF func )
{
  ASSERT( (int)(src.size()) == (int)(dst.size()) );

  typename IC::const_iterator  s = src.begin();
  typename OC::iterator        F = dst.begin();
  typename OC::iterator        L = dst.end();

  while(F != L)
  {
    (*F) = (typename OC::value_type)(func( *s ));
    ++F;
    ++s;
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function applies binary function to the couple of source sequences and stores result in
           the destination one.
    \param  src1  first source sequence (any container).
    \param  src2  second source sequence (any container).
    \param  dst   destination sequence (any container).
    \param  func  binary function. */
//-------------------------------------------------------------------------------------------------
template< class IC1, class IC2, class OC, class BF >
inline void Transform( const IC1 & src1, const IC2 & src2, OC & dst, BF func )
{
  ASSERT( (int)(src1.size()) == (int)(dst.size()) );
  ASSERT( (int)(src2.size()) == (int)(dst.size()) );

  typename IC1::const_iterator  a = src1.begin();
  typename IC2::const_iterator  b = src2.begin();
  typename OC::iterator         F = dst.begin();
  typename OC::iterator         L = dst.end();

  while(F != L)
  {
    (*F) = (typename OC::value_type)(func( (*a), (*b) ));
    ++F;
    ++a;
    ++b;
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function adds specified item to all elements on the sequence.
    \param  F  begin iterator of the sequence.
    \param  L  end iterator of the sequence.
    \param  a  item to add. */
//-------------------------------------------------------------------------------------------------
/*
template< class IT, class ITEM >
void add(IT F, IT L, ITEM a)
{
  for(; F != L; ++F)
  {
    (*F) += a;
  }
}
*/


//-------------------------------------------------------------------------------------------------
/** \brief Function subtracts specified item from all elements on the sequence.
    \param  F  begin iterator of the sequence.
    \param  L  end iterator of the sequence.
    \param  s  item to subtract. */
//-------------------------------------------------------------------------------------------------
/*
template< class IT, class ITEM >
void subtract(IT F, IT L, ITEM s)
{
  for(; F != L; ++F)
  {
    (*F) -= s;
  }
}
*/


//-------------------------------------------------------------------------------------------------
// Replace container's values by their modules: *F++ = abs(*F)
//-------------------------------------------------------------------------------------------------
template< class ITER >
void Module( ITER F, ITER L )
{
  for (; F != L; F++)
  {
    if (!((*F) >= 0))
      (*F) = -(*F);
  }
}


//-------------------------------------------------------------------------------------------------
// Initialize container of pointers: (*dF++) = &(*sF++)
//-------------------------------------------------------------------------------------------------
template< class II, class OI, class PTR >
void FillSequenceOfPointers( II sF, II sL, OI dF, OI dL, PTR p )
{
  while (sF != sL)
  {
    ASSERT( dF != dL );
    p = &(*sF);
    (*dF) = p;
    ++dF;
    ++sF;
  }
  ASSERT( dF == dL );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function applies specified function to each element of the input sequence.

  \param  F     begin iterator.
  \param  L     end iterator.
  \param  func  function to be applied. */
//-------------------------------------------------------------------------------------------------
template< class ITER, class FUNC >
void ApplyToEach( ITER F, ITER L, FUNC func )
{
  for (; F != L; ++F)
  {
    func( F );
  }
}


/*
//-------------------------------------------------------------------------------------------------
// Function searches an ascendantly sorted sequence for the lower-nearest value for the specified
// one, i.e. if specified value falls between first[i] and first[i+1], then index 'i' will be
// returned. If several successive elements of the sequence have the same value, then index of one
// of them will be returned. Returns -1 for empty sequence.
//-------------------------------------------------------------------------------------------------
template<class TYPE>
int LoNearInAscendSorted(TYPE * first, TYPE * last, const TYPE & value)
{
  TYPE * lo = first;
  TYPE * hi = last;
  TYPE * it = last;

  ASSERT(first <= last);
  if (first == last)
    return -1;

  while((lo+1) < hi)
  {
    it = lo + (hi-lo)/2;
    if ((*it) == value)
    {
      return (int)(it-first);
    }
    else if ((*it) < value)
    {
      lo = it;
    }
    else // ((*it) > value)
    {
      hi = it;
    }
  }
  return (int)(lo-first);
}
*/

/*
//--------------------------------------------------------------------------
// Searches an ascendantly sorted sequence for the given value.
//--------------------------------------------------------------------------
template<class ITER, class TYPE>
ITER find_ascendant_sorted(ITER first, ITER last, const TYPE & value)
{
  ITER lo = first, hi = last, it = last;

  while(lo < hi)
  {
    it = lo + (hi - lo) / 2;
    if ((*it) == value)
    {
      return it;
    }
    else if ((*it) > value)

    {
      lo = it + 1;
    }
    else // ((*it) < value)
    {
      hi = it;
    }
  }
  return last;
}


//--------------------------------------------------------------------------
// Searches a sorted sequence for given value
//--------------------------------------------------------------------------
template<class ITER, class CMP>
ITER find_sorted(ITER first, ITER last, CMP cmp)
{
  ITER lo = first, hi = last, it = last;

  while(lo < hi)
  {
    it = lo + (hi - lo) / 2;
    int code = cmp(*it);

    if (code == 0)
    {
      return it;
    }
    else if (code > 0)
    {
      lo = it + 1;
    }
    else // (code < 0)
    {
      hi = it;
    }
  }
  return last;
}
*/


//-------------------------------------------------------------------------------------------------
/** \brief Function adds the source sequence to the destination one.

  \param  source       source container.
  \param  destination  destination container.
  \return              reference to the destination container. */
//-------------------------------------------------------------------------------------------------
template< class ST, class DT >
DT & Add( const ST & source, DT & destination )
{
  ASSERT( (int)(destination.size()) == (int)(source.size()) );

  typename DT::iterator       d = destination.begin();
  typename DT::iterator       e = destination.end();
  typename ST::const_iterator s = source.begin();

  while (d != e)
  {
    (*d) += (typename DT::value_type)(*s);
    ++d;
    ++s;
  }
  return destination;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function subtracts the source sequence from the destination one.

  \param  source       source container.
  \param  destination  destination container.
  \return              reference to the destination container. */
//-------------------------------------------------------------------------------------------------
template< class ST, class DT >
DT & Subtract( const ST & source, DT & destination )
{
  ASSERT( (int)(destination.size()) == (int)(source.size()) );

  typename DT::iterator       d = destination.begin();
  typename DT::iterator       e = destination.end();
  typename ST::const_iterator s = source.begin();

  while (d != e)
  {
    (*d) -= (typename DT::value_type)(*s);
    ++d;
    ++s;
  }
  return destination;
}

} // namespace alib

