#pragma once

class ChairImageContainer 	
{
public:
	ChairImageContainer(void);
	~ChairImageContainer(void);

	void Load( std::wstring & filename ) throw(...);
	void Save( std::wstring & filename ) throw(...);
	void AddChairImage( int camN, int id, std::wstring & description, Arr2ub & image ) throw (...);
	bool GetChairImages( int camN, int id, std::vector <Arr2ub> & image ) throw (...);
private:
	XmlLite::XMLDocument m_doc;
};
