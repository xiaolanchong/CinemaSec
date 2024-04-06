#pragma once


// CRangeSelect

class CRangeSelect : public CStatic
{
	DECLARE_DYNAMIC(CRangeSelect)

	CTime	m_timeMin, m_timeMax;
	CTime	m_timeStart, m_timeEnd;
public:
	CRangeSelect();
	virtual ~CRangeSelect();

	void	SetMinMax( CTime timeMin, CTime timeMax );
	void	SetStart( CTime timeStart );
	void	SetCurrent( CTime timeCur );
	void	SetEnd( CTime timeEnd );
	void	Reset();

	std::pair<CTime, CTime> GetRange() const;

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnPaint();
};


