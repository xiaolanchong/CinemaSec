#include "stdafx.h"

#ifdef ALIB_CHECK_MEMORY_LEAKAGE
#define new DEBUG_NEW
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
// Functions were adopted from Intel Small Matrix Library.
//
// Matrices should be stored in row by row manner:
//
// (matrix element)[row][column] == matrix[ row*nColumns + column ]
///////////////////////////////////////////////////////////////////////////////////////////////////

namespace matrixlib
{

//-------------------------------------------------------------------------------------------------
/** \brief Function inverts 4x4 matrix in-place.

  Matrix should be stored in row by row manner.

  \param  m     pointer to the matrix beginning.
  \param  mEnd  pointer to the next after last entry (STL style).
  \return       Ok = true. */
//-------------------------------------------------------------------------------------------------
bool Invert4x4( double * m, double * mEnd )
{
  if (std::distance( m, mEnd ) != (4*4))
    return false;

  double d, di;
  di = m[0];
  m[0] = d = 1.0 / di;
  m[4] *= -d;
  m[8] *= -d;
  m[12] *= -d;
  m[1] *= d;
  m[2] *= d;
  m[3] *= d;
  m[5] += m[4] * m[1] * di;
  m[6] += m[4] * m[2] * di;
  m[7] += m[4] * m[3] * di;
  m[9] += m[8] * m[1] * di;
  m[10] += m[8] * m[2] * di;
  m[11] += m[8] * m[3] * di;
  m[13] += m[12] * m[1] * di;
  m[14] += m[12] * m[2] * di;
  m[15] += m[12] * m[3] * di;
  di = m[5];
  m[5] = d = 1.0 / di;
  m[1] *= -d;
  m[9] *= -d;
  m[13] *= -d;
  m[4] *= d;
  m[6] *= d;
  m[7] *= d;
  m[0] += m[1] * m[4] * di;
  m[2] += m[1] * m[6] * di;
  m[3] += m[1] * m[7] * di;
  m[8] += m[9] * m[4] * di;
  m[10] += m[9] * m[6] * di;
  m[11] += m[9] * m[7] * di;
  m[12] += m[13] * m[4] * di;
  m[14] += m[13] * m[6] * di;
  m[15] += m[13] * m[7] * di;
  di = m[10];
  m[10] = d = 1.0 / di;
  m[2] *= -d;
  m[6] *= -d;
  m[14] *= -d;
  m[8] *= d;
  m[9] *= d;
  m[11] *= d;
  m[0] += m[2] * m[8] * di;
  m[1] += m[2] * m[9] * di;
  m[3] += m[2] * m[11] * di;
  m[4] += m[6] * m[8] * di;
  m[5] += m[6] * m[9] * di;
  m[7] += m[6] * m[11] * di;
  m[12] += m[14] * m[8] * di;
  m[13] += m[14] * m[9] * di;
  m[15] += m[14] * m[11] * di;
  di = m[15];
  m[15] = d = 1.0 / di;
  m[3] *= -d;
  m[7] *= -d;
  m[11] *= -d;
  m[12] *= d;
  m[13] *= d;
  m[14] *= d;
  m[0] += m[3] * m[12] * di;
  m[1] += m[3] * m[13] * di;
  m[2] += m[3] * m[14] * di;
  m[4] += m[7] * m[12] * di;
  m[5] += m[7] * m[13] * di;
  m[6] += m[7] * m[14] * di;
  m[8] += m[11] * m[12] * di;
  m[9] += m[11] * m[13] * di;
  m[10] += m[11] * m[14] * di;
  return true;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function inverts 6x6 matrix in-place.

  Matrix should be stored in row by row manner.

  \param  m     pointer to the matrix beginning.
  \param  mEnd  pointer to the next after last entry (STL style).
  \return       Ok = true. */
//-------------------------------------------------------------------------------------------------
bool Invert6x6( double * m, double * mEnd )
{
  if (std::distance( m, mEnd ) != (6*6))
    return false;

  double d, di;
  di = m[0];
  m[0] = d = 1.0f / di;
  m[6] *= -d;
  m[12] *= -d;
  m[18] *= -d;
  m[24] *= -d;
  m[30] *= -d;
  m[1] *= d;
  m[2] *= d;
  m[3] *= d;
  m[4] *= d;
  m[5] *= d;
  m[7] += m[6] * m[1] * di;
  m[8] += m[6] * m[2] * di;
  m[9] += m[6] * m[3] * di;
  m[10] += m[6] * m[4] * di;
  m[11] += m[6] * m[5] * di;
  m[13] += m[12] * m[1] * di;
  m[14] += m[12] * m[2] * di;
  m[15] += m[12] * m[3] * di;
  m[16] += m[12] * m[4] * di;
  m[17] += m[12] * m[5] * di;
  m[19] += m[18] * m[1] * di;
  m[20] += m[18] * m[2] * di;
  m[21] += m[18] * m[3] * di;
  m[22] += m[18] * m[4] * di;
  m[23] += m[18] * m[5] * di;
  m[25] += m[24] * m[1] * di;
  m[26] += m[24] * m[2] * di;
  m[27] += m[24] * m[3] * di;
  m[28] += m[24] * m[4] * di;
  m[29] += m[24] * m[5] * di;
  m[31] += m[30] * m[1] * di;
  m[32] += m[30] * m[2] * di;
  m[33] += m[30] * m[3] * di;
  m[34] += m[30] * m[4] * di;
  m[35] += m[30] * m[5] * di;
  di = m[7];
  m[7] = d = 1.0f / di;
  m[1] *= -d;
  m[13] *= -d;
  m[19] *= -d;
  m[25] *= -d;
  m[31] *= -d;
  m[6] *= d;
  m[8] *= d;
  m[9] *= d;
  m[10] *= d;
  m[11] *= d;
  m[0] += m[1] * m[6] * di;
  m[2] += m[1] * m[8] * di;
  m[3] += m[1] * m[9] * di;
  m[4] += m[1] * m[10] * di;
  m[5] += m[1] * m[11] * di;
  m[12] += m[13] * m[6] * di;
  m[14] += m[13] * m[8] * di;
  m[15] += m[13] * m[9] * di;
  m[16] += m[13] * m[10] * di;
  m[17] += m[13] * m[11] * di;
  m[18] += m[19] * m[6] * di;
  m[20] += m[19] * m[8] * di;
  m[21] += m[19] * m[9] * di;
  m[22] += m[19] * m[10] * di;
  m[23] += m[19] * m[11] * di;
  m[24] += m[25] * m[6] * di;
  m[26] += m[25] * m[8] * di;
  m[27] += m[25] * m[9] * di;
  m[28] += m[25] * m[10] * di;
  m[29] += m[25] * m[11] * di;
  m[30] += m[31] * m[6] * di;
  m[32] += m[31] * m[8] * di;
  m[33] += m[31] * m[9] * di;
  m[34] += m[31] * m[10] * di;
  m[35] += m[31] * m[11] * di;
  di = m[14];
  m[14] = d = 1.0f / di;
  m[2] *= -d;
  m[8] *= -d;
  m[20] *= -d;
  m[26] *= -d;
  m[32] *= -d;
  m[12] *= d;
  m[13] *= d;
  m[15] *= d;
  m[16] *= d;
  m[17] *= d;
  m[0] += m[2] * m[12] * di;
  m[1] += m[2] * m[13] * di;
  m[3] += m[2] * m[15] * di;
  m[4] += m[2] * m[16] * di;
  m[5] += m[2] * m[17] * di;
  m[6] += m[8] * m[12] * di;
  m[7] += m[8] * m[13] * di;
  m[9] += m[8] * m[15] * di;
  m[10] += m[8] * m[16] * di;
  m[11] += m[8] * m[17] * di;
  m[18] += m[20] * m[12] * di;
  m[19] += m[20] * m[13] * di;
  m[21] += m[20] * m[15] * di;
  m[22] += m[20] * m[16] * di;
  m[23] += m[20] * m[17] * di;
  m[24] += m[26] * m[12] * di;
  m[25] += m[26] * m[13] * di;
  m[27] += m[26] * m[15] * di;
  m[28] += m[26] * m[16] * di;
  m[29] += m[26] * m[17] * di;
  m[30] += m[32] * m[12] * di;
  m[31] += m[32] * m[13] * di;
  m[33] += m[32] * m[15] * di;
  m[34] += m[32] * m[16] * di;
  m[35] += m[32] * m[17] * di;
  di = m[21];
  m[21] = d = 1.0f / di;
  m[3] *= -d;
  m[9] *= -d;
  m[15] *= -d;
  m[27] *= -d;
  m[33] *= -d;
  m[18] *= d;
  m[19] *= d;
  m[20] *= d;
  m[22] *= d;
  m[23] *= d;
  m[0] += m[3] * m[18] * di;
  m[1] += m[3] * m[19] * di;
  m[2] += m[3] * m[20] * di;
  m[4] += m[3] * m[22] * di;
  m[5] += m[3] * m[23] * di;
  m[6] += m[9] * m[18] * di;
  m[7] += m[9] * m[19] * di;
  m[8] += m[9] * m[20] * di;
  m[10] += m[9] * m[22] * di;
  m[11] += m[9] * m[23] * di;
  m[12] += m[15] * m[18] * di;
  m[13] += m[15] * m[19] * di;
  m[14] += m[15] * m[20] * di;
  m[16] += m[15] * m[22] * di;
  m[17] += m[15] * m[23] * di;
  m[24] += m[27] * m[18] * di;
  m[25] += m[27] * m[19] * di;
  m[26] += m[27] * m[20] * di;
  m[28] += m[27] * m[22] * di;
  m[29] += m[27] * m[23] * di;
  m[30] += m[33] * m[18] * di;
  m[31] += m[33] * m[19] * di;
  m[32] += m[33] * m[20] * di;
  m[34] += m[33] * m[22] * di;
  m[35] += m[33] * m[23] * di;
  di = m[28];
  m[28] = d = 1.0f / di;
  m[4] *= -d;
  m[10] *= -d;
  m[16] *= -d;
  m[22] *= -d;
  m[34] *= -d;
  m[24] *= d;
  m[25] *= d;
  m[26] *= d;
  m[27] *= d;
  m[29] *= d;
  m[0] += m[4] * m[24] * di;
  m[1] += m[4] * m[25] * di;
  m[2] += m[4] * m[26] * di;
  m[3] += m[4] * m[27] * di;
  m[5] += m[4] * m[29] * di;
  m[6] += m[10] * m[24] * di;
  m[7] += m[10] * m[25] * di;
  m[8] += m[10] * m[26] * di;
  m[9] += m[10] * m[27] * di;
  m[11] += m[10] * m[29] * di;
  m[12] += m[16] * m[24] * di;
  m[13] += m[16] * m[25] * di;
  m[14] += m[16] * m[26] * di;
  m[15] += m[16] * m[27] * di;
  m[17] += m[16] * m[29] * di;
  m[18] += m[22] * m[24] * di;
  m[19] += m[22] * m[25] * di;
  m[20] += m[22] * m[26] * di;
  m[21] += m[22] * m[27] * di;
  m[23] += m[22] * m[29] * di;
  m[30] += m[34] * m[24] * di;
  m[31] += m[34] * m[25] * di;
  m[32] += m[34] * m[26] * di;
  m[33] += m[34] * m[27] * di;
  m[35] += m[34] * m[29] * di;
  di = m[35];
  m[35] = d = 1.0f / di;
  m[5] *= -d;
  m[11] *= -d;
  m[17] *= -d;
  m[23] *= -d;
  m[29] *= -d;
  m[30] *= d;
  m[31] *= d;
  m[32] *= d;
  m[33] *= d;
  m[34] *= d;
  m[0] += m[5] * m[30] * di;
  m[1] += m[5] * m[31] * di;
  m[2] += m[5] * m[32] * di;
  m[3] += m[5] * m[33] * di;
  m[4] += m[5] * m[34] * di;
  m[6] += m[11] * m[30] * di;
  m[7] += m[11] * m[31] * di;
  m[8] += m[11] * m[32] * di;
  m[9] += m[11] * m[33] * di;
  m[10] += m[11] * m[34] * di;
  m[12] += m[17] * m[30] * di;
  m[13] += m[17] * m[31] * di;
  m[14] += m[17] * m[32] * di;
  m[15] += m[17] * m[33] * di;
  m[16] += m[17] * m[34] * di;
  m[18] += m[23] * m[30] * di;
  m[19] += m[23] * m[31] * di;
  m[20] += m[23] * m[32] * di;
  m[21] += m[23] * m[33] * di;
  m[22] += m[23] * m[34] * di;
  m[24] += m[29] * m[30] * di;
  m[25] += m[29] * m[31] * di;
  m[26] += m[29] * m[32] * di;
  m[27] += m[29] * m[33] * di;
  m[28] += m[29] * m[34] * di;
  return true;
}

} // namespace matrixlib
