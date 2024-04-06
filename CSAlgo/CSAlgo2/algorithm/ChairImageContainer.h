#pragma once

class ChairImageContainer   
{
public:
  ChairImageContainer();
  virtual ~ChairImageContainer();

  void Load( const std::wstring & filename ) throw(...);
  void Save( const std::wstring & filename ) throw(...);
  void AddChairImage( int cameraNo, int chairId, const std::wstring & description, const Arr2ub & image ) throw (...);
  void GetChairImages( int cameraNo, int chairId, std::vector<Arr2ub> & image ) throw (...);

private:
  XmlLite::XMLDocument m_doc;
};

