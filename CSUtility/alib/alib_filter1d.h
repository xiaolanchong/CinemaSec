#ifndef  ALIB_FILTER_1_DIMENSION_H
#define  ALIB_FILTER_1_DIMENSION_H

namespace alib
{

//=================================================================================================
// Class delivers common interface for any one-dimensional, one-parameter filter.
// Class keeps right half of a filter and its central element (filter[0]).<br>
//
// Following methods should be declared in an inheritor:
// <ol><li>int    calculate_filter_size();</li>
//     <li>double validate_parameter(double param);</li>
//     <li>double normalization_constant();</li>
//     <li>double kernel(double x);</li></ol><br>
//
// <b>Definition:</b> Normalization constant = Integral( abs( kernel(x) ), x=[0..infinity] ).
//=================================================================================================

class ALIB_API Filter1D
{
 public:
  enum FilterType
  {
    NO_TYPE           = 0,
    GAUSS             = 1,
    GAUSS_COMPENSATED = 2,
    GAUSS_DIFF        = 3,
    GAUSS_CURVATURE   = 4
  };

 protected:
  std::vector<float> m_filter;    //!< filter
  double             m_param;     //!< filter's parameter
  bool               m_bCreated;  //!< creation flag
  FilterType         m_type;      //!< type of this filter

  /** \brief Function calculates size of filter from its type and parameter value. */
  virtual int calculate_filter_size() const = 0;

  /** \brief Function ensures that parameter lies within the valid range. */
  virtual double validate_parameter(double param) const = 0;

  /** \brief Function returns normalization constant according to filter type. */
  virtual double normalization_constant() const = 0;

  /** \brief Function returns the value of filter kernel function given the argument. */
  virtual double kernel(double x) const = 0;

 public:
  
	/** \brief Function returns actual value of filter's parameter. */
	double parameter() const
	{
    return m_param;
	}

	/** \brief Function gives access to the content of this filter. */
	const std::vector<float> & filter() const
	{
    ASSERT( !(m_filter.empty()) );
		return m_filter;
	}

	/** \brief Function retrieves type of this filter. */
	FilterType type() const
	{
		return m_type;
	}

	Filter1D();
	virtual ~Filter1D();

	void clear();
	bool create(double param);
};


//=================================================================================================
// 1D Gauss filter.
//=================================================================================================

struct ALIB_API GaussFilter1D : public Filter1D
{
  GaussFilter1D();
  int    calculate_filter_size() const;
  double validate_parameter(double param) const;
  double normalization_constant() const;
  double kernel(double x) const;
};


//=================================================================================================
// 1D Gauss filter with shrinkage compensation.
//=================================================================================================

struct ALIB_API GaussCompensFilter1D : public Filter1D
{
  GaussCompensFilter1D();
  int    calculate_filter_size() const;
  double validate_parameter(double param) const;
  double normalization_constant() const;
  double kernel(double x) const;
};


//=================================================================================================
// 1D Gauss differentiating filter.
//=================================================================================================

struct ALIB_API GaussDiffFilter1D : public Filter1D
{
  GaussDiffFilter1D();
  int    calculate_filter_size() const;
  double validate_parameter(double param) const;
  double normalization_constant() const;
  double kernel(double x) const;
};


//=================================================================================================
// Convolution with this filter gives estimation of curvature at any curve's point.
//=================================================================================================

struct ALIB_API GaussCurvatureFilter1D : public Filter1D
{
  GaussCurvatureFilter1D();
  int    calculate_filter_size() const;
  double validate_parameter(double param) const;
  double normalization_constant() const;
  double kernel(double x) const;
};

} // namespace alib

#endif // ALIB_FILTER_1_DIMENSION_H
