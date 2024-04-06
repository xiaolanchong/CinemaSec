//////////////////////////////////////////////////////////////////////////
//class ThresholdFinder
//author: Alexander Boltnev
//e-mail: Alexander.Boltnev@biones.com
//////////////////////////////////////////////////////////////////////////

template <class value_type>
class ThresholdFinder
{
public:
	
	ThresholdFinder() {};
		
	~ThresholdFinder(){};


	int CalculateThreshold( std::vector< value_type >& layerA, std::vector< value_type >& layerB ) const
	{
		ASSERT( layerA.size() == layerB.size() );

		
		std::vector< value_type > leftLayer; 
		std::vector< value_type > rightLayer;

		if ( MaximumPos( layerA ) < MaximumPos( layerB ) )
		{
			leftLayer = layerA;
			rightLayer = layerB;
		} 
		else
		{
			leftLayer = layerB;
			rightLayer = layerA;
		}




		int sz = (int) leftLayer.size();

		for ( int i = 1; i < sz; i++ )
		{
			CString s;
			s.Format(_T("%f"), leftLayer[i]);
			OutputDebugString(s);
		}

		value_type maxerr = Error( leftLayer, rightLayer, 0 );
		int candidate = 0;
		for ( int i = 1; i < sz; i++ )
		{

			value_type err = Error( leftLayer, rightLayer, i );
			if ( err < maxerr )
			{
				maxerr = err;
				candidate = i;
			}
		}
		return candidate;
	}
private:
	value_type Error( std::vector< value_type >& leftLayer, std::vector< value_type >& rightLayer, int threshold ) const
	{
		value_type err = 0;
		int sz = (int) leftLayer.size();
		for (int i = 0; i < sz; i++)
		{
			if (i < threshold )
				err += rightLayer[i];
			else
				err += leftLayer[i];
			
		}
		return err;
	}

	value_type MaximumPos( std::vector< value_type >& layer ) const
	{
		int sz = (int) layer.size();
		value_type pos = 0;
		for (int i = 0; i < sz; i++)
		{
			pos += (value_type)i * layer[i];
		}		
		return pos;
	}
};