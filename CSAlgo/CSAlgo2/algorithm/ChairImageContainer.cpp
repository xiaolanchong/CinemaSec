#include "StdAfx.h"
#include "ChairImageContainer.h"

const wchar_t THIS_XML_CONTAINS_IMAGES_OF_CHAIRS[] = L"This xml contains images of chairs";

ChairImageContainer::ChairImageContainer()
{
  XmlLite::XMLElement rootElem( m_doc, L"Top" );
  rootElem << XmlLite::XMLAttribute( rootElem, L"version", THIS_XML_CONTAINS_IMAGES_OF_CHAIRS );
  m_doc.SetRoot( rootElem );
}


ChairImageContainer::~ChairImageContainer()
{
}


void ChairImageContainer::Load( const std::wstring & filename ) throw(...) 
{
  try
  {
    VARIANT_BOOL b;
    m_doc.m_pDoc->load( CComVariant(filename.c_str()), &b );

    ALIB_VERIFY( b == VARIANT_TRUE, L"Cannot load the XML document" );

    XmlLite::XMLElement elem = m_doc.GetRoot();
    std::wstring value;
    elem.GetAttr( L"version", value ); 
  
    ALIB_VERIFY( value == THIS_XML_CONTAINS_IMAGES_OF_CHAIRS, L"This XML seems not to be a chairs images container" );
  }
  catch (std::runtime_error)
  {
    XmlLite::XMLElement rootElem( m_doc, L"Top" );
    rootElem << XmlLite::XMLAttribute( rootElem, L"version", THIS_XML_CONTAINS_IMAGES_OF_CHAIRS );
    m_doc.SetRoot( rootElem );

    OutputDebugString( L"cannot load ChairImageContainer" );
    throw;
  }
}


void ChairImageContainer::Save( const std::wstring & filename ) throw(...)
{
  try
  {
    m_doc.m_pDoc->save( CComVariant(filename.c_str()));
  }
  catch (std::runtime_error)
  {
    OutputDebugString( L"cannot save ChairImageContainer" );
    throw;
  }
}


void ChairImageContainer::AddChairImage( int cameraNo, int chairId, const std::wstring & description,
                                         const Arr2ub & image ) throw (...)
{
  XmlLite::XMLElement::iterator iter;
  for (iter = m_doc.GetRoot().begin(); iter != m_doc.GetRoot().end(); iter++)
  {
    int curCam;
    iter->GetAttr( L"cam", curCam );
    int curId;
    iter->GetAttr( L"id", curId );

    if ((curId == chairId) && (curCam == cameraNo))
    {
      XmlLite::XMLElement elemimg( m_doc, L"image" );
      elemimg << XmlLite::XMLAttribute( elemimg, L"width", image.width() );
      elemimg << XmlLite::XMLAttribute( elemimg, L"height", image.height() );
      elemimg << XmlLite::XMLAttribute( elemimg, L"description", description.c_str() );
      elemimg << XmlLite::XMLBinaryOut<const unsigned char*>( image.begin(), image.size() );
      (*iter) << elemimg;
      return;
    }
  }

  XmlLite::XMLElement elemchair( m_doc, L"chair" );
  elemchair << XmlLite::XMLAttribute( elemchair, L"cam", cameraNo );
  elemchair << XmlLite::XMLAttribute( elemchair, L"id", chairId );
  
  XmlLite::XMLElement elemimg( m_doc, L"image" );
  elemimg << XmlLite::XMLAttribute( elemimg, L"width", image.width() );
  elemimg << XmlLite::XMLAttribute( elemimg, L"height", image.height() );
  elemimg << XmlLite::XMLAttribute( elemimg, L"description", description.c_str() );
  elemimg << XmlLite::XMLBinaryOut<const unsigned char*>( image.begin(), image.size() );
  elemchair << elemimg;

  m_doc.GetRoot() << elemchair;
}


void ChairImageContainer::GetChairImages( int cameraNo, int chairId, std::vector<Arr2ub> & images ) throw (...)
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

      if ((elemCamN == cameraNo) && (elemId == chairId))
      {
        XmlLite::XMLElement::iterator imgiter;

        // Calculate image number.
        int count = 0;
        for (imgiter = iter->begin(); imgiter != iter->end(); imgiter++)
          ++count;

        // now add all images of the chair into the array
        images.reserve( count );
        for (imgiter = iter->begin(); imgiter != iter->end(); imgiter++)
        {
          int w; 
          imgiter->GetAttr( L"width", w );
          int h; 
          imgiter->GetAttr( L"height", h );

          std::vector<unsigned char> img;
          XmlLite::XMLBinaryIn< std::vector<unsigned char> > xmlNode( img );
          (*imgiter) >> xmlNode;

          int sz = (int)(img.size());
          ALIB_VERIFY( sz == w*h, L"Bad image" );

          images.push_back( Arr2ub() );
          Arr2ub & destination = images.back();
          destination.resize( w, h );
          std::copy( img.begin(), img.end(), destination.begin() );
        }
      }
    }
  }
  catch (std::runtime_error e)
  {
    OutputDebugString( L"cannot get next image" );
    throw;
  }
}

