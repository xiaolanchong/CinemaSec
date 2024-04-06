///////////////////////////////////////////////////////////////////////////////////////////////////
// print_histogram_set.h
// ---------------------
// begin     : Dec 2005
// modified  : 5 Dec 2005
// author(s) : Albert Akhriev
// email     : Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef  __DEFINE_PRINT_HISTOGRAM_SET_H__
#define  __DEFINE_PRINT_HISTOGRAM_SET_H__

namespace csutility
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \class PrintHistogramSet.
/// \brief Class prints a set of histograms into GNU-PLOT compatible file.
///////////////////////////////////////////////////////////////////////////////////////////////////
class PrintHistogramSet
{
private:
  std::fstream m_file;  //!< output file in GNU-PLOT format

  PrintHistogramSet();  // no default constructor

public:
  /////////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Constructor opens output file.
  /////////////////////////////////////////////////////////////////////////////////////////////////
  PrintHistogramSet( const char * fileName )
  {
    if (fileName != 0)
    {
      m_file.open( fileName, std::ios::out | std::ios::trunc );
      m_file << "reset" << std::endl << "set data style lines" << std::endl;
    }
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Function returns nonzero if the object was properly initialized.
  /////////////////////////////////////////////////////////////////////////////////////////////////
  bool IsReady() const
  {
    return (m_file.good() && m_file.is_open());
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////
  /// \brief Function appends a new diagram (that contains several histograms)
  ///        to the end of output file.
  ///
  /// \param  first        array of begin iterators of histogram set.
  /// \param  last         array of end iterators of histogram set.
  /// \param  titles       array of histogram titles.
  /// \param  commonTitle  the common title of the diagram, that contains several histograms.
  /// \param  format       the format string to be printed before 'plot' operator.
  /////////////////////////////////////////////////////////////////////////////////////////////////
  template< class ITER, int HISTONUM >
  void PrintHistogram( ITER first[HISTONUM], ITER last[HISTONUM], const char * titles[HISTONUM],
                       const char * commonTitle = 0, const char * format = 0 )
  {
    ALIB_ASSERT( IsReady() && (HISTONUM > 0) );
    if (format != 0) m_file << format << std::endl;
    if (commonTitle != 0) m_file << "set title \"" << commonTitle << "\"" << std::endl;
    m_file << "plot ";
    for (int q = 0; q < HISTONUM; q++)
    {
      m_file << "'-'";
      if ((titles[q] != 0) && (titles[q] != '\0')) m_file << " t \"" << titles[q] << "\"";
      if ((q+1) < HISTONUM) m_file << ", ";
    }
    m_file << std::endl;
    for (int i = 0; i < HISTONUM; i++)
    {
      for (ITER F = first[i], L = last[i]; F != L; ++F) m_file << (*F) << std::endl;
      m_file << "e" << std::endl;
    }
    m_file << std::endl << "pause -1 \"Hit 'Enter' to continue\"" << std::endl << std::endl;
  }
};

} // namespace csutility

#endif // __DEFINE_PRINT_HISTOGRAM_SET_H__

