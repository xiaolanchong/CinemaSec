#include "StdAfx.h"
#include "ChairImageContainer.h"

ChairImageContainer::ChairImageContainer(void)
{
	XmlLite::XMLElement rootElem( m_doc, L"Top" );
	rootElem << XmlLite::XMLAttribute( rootElem, L"version", L"This xml contains images of chairs" );
	m_doc.SetRoot( rootElem );
}

ChairImageContainer::~ChairImageContainer(void)
{
}

void ChairImageContainer::Load( std::wstring & filename ) throw(...) 
{
	try
	{
		VARIANT_BOOL b;
		m_doc.m_pDoc->load( CComVariant(filename.c_str()), &b );

		if ( b != VARIANT_TRUE)
			throw std::runtime_error( "cannot load the document" );

		XmlLite::XMLElement elem = m_doc.GetRoot();
		std::wstring value;
		elem.GetAttr( L"version", value ); 
	
		if ( value !=  L"This xml contains images of chairs" )
			throw std::runtime_error( "this xml seems not to be a chairs images container" );
	}
	catch(...)
	{
		XmlLite::XMLElement rootElem( m_doc, L"Top" );
		rootElem << XmlLite::XMLAttribute( rootElem, L"version", L"This xml contains images of chairs" );
		m_doc.SetRoot( rootElem );

		OutputDebugString( L"cannot load ChairImageContainer" );
		throw;
	}
}

void ChairImageContainer::Save(  std::wstring & filename ) throw(...)
{
	try
	{
		m_doc.m_pDoc->save( CComVariant(filename.c_str()));
	}
	catch(...)
	{
		OutputDebugString( L"cannot save ChairImageContainer" );
		throw;
	}
}

void ChairImageContainer::AddChairImage( int camN, int id, std::wstring & description, Arr2ub & image ) throw (...)
{
	XmlLite::XMLElement::iterator iter;
	for (iter = m_doc.GetRoot().begin(); iter != m_doc.GetRoot().end(); iter++)
	{
		int curCam;
		iter->GetAttr( L"cam", curCam );
		int curId;
		iter->GetAttr( L"id", curId );

		if ( curId == id && curCam == camN )
		{
			XmlLite::XMLElement elemimg( m_doc, L"image" );
			elemimg << XmlLite::XMLAttribute( elemimg, L"width", image.width() );
			elemimg << XmlLite::XMLAttribute( elemimg, L"height", image.height() );
			elemimg << XmlLite::XMLAttribute( elemimg, L"description", description.c_str() );
			elemimg << XmlLite::XMLBinaryOut< unsigned char * >(( unsigned char *)(&image[0]), image.width() * image.height() );
			(*iter) << elemimg;
			return;
		}
	}

	XmlLite::XMLElement elemchair( m_doc, L"chair" );
	elemchair << XmlLite::XMLAttribute( elemchair, L"cam", camN );
	elemchair << XmlLite::XMLAttribute( elemchair, L"id", id );
	
	XmlLite::XMLElement elemimg( m_doc, L"image" );
	elemimg << XmlLite::XMLAttribute( elemimg, L"width", image.width() );
	elemimg << XmlLite::XMLAttribute( elemimg, L"height", image.height() );
	elemimg << XmlLite::XMLAttribute( elemimg, L"description", description.c_str() );
	elemimg << XmlLite::XMLBinaryOut< unsigned char * >(( unsigned char *)(&image[0]), image.width() * image.height() );
	elemchair << elemimg;

	m_doc.GetRoot() << elemchair;
}

bool ChairImageContainer::GetChairImages( int camN, int id, std::vector <Arr2ub> & images ) throw (...)
{
	try
	{
		images.clear();
		XmlLite::XMLElement::iterator iter;
		for (iter = m_doc.GetRoot().begin(); iter != m_doc.GetRoot().end(); iter++)
		{
			int elemCamN;
			iter->GetAttr( L"cam", elemCamN);
			int elemId;
			iter->GetAttr( L"id", elemId );

			if ( elemCamN == camN && elemId == id )
			{
				// now add all images of the chair into the array
				XmlLite::XMLElement::iterator imgiter;
				for (imgiter = iter->begin(); imgiter != iter->end(); imgiter++)
				{
					int w; 
					imgiter->GetAttr( L"width", w );
					int h; 
					imgiter->GetAttr( L"height", h );

					std::vector < unsigned char > img;
					(*imgiter) >> XmlLite::XMLBinaryIn< std::vector< unsigned char > >( img ) ;

					int sz = (int)img.size();
					if (sz != w * h )
					{
						throw std::runtime_error( "bad image" );
					}
					Arr2ub image;
					image.resize( w, h );

					int i = -1;
					for ( int x = 0; x < w; x++ )
						for( int y = 0; y < h; y++)
						{
							image( x, y ) = img[ x + y * w ];
							i = image( x, y );
						}

						images.push_back( image );
				}
			}
		}
		return false;
	}
	catch(...)
	{
		OutputDebugString( L"cannot get next image" );
		throw;
	}
}
