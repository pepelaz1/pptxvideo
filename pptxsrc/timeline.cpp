#include "timeline.h"
#include <reftime.h>

CTimeline::CTimeline() : 
	m_doc(NULL),
	m_lastno(-1),
	m_time(-1),
	m_duration(-1)
{
}


CTimeline::~CTimeline()
{
}

void CTimeline::Init(CDoc *doc)
{
	m_doc = doc;
	Reset();
}

void CTimeline::Prev()
{
	if (!m_doc->IsDisableClicks())
	{
		m_lastno--;
		if (m_lastno < 0) m_lastno = 0;
		m_time = timeGetTime();
	}
}

void CTimeline::Next()
{
	if (!m_doc->IsDisableClicks())
	{
		m_lastno++;
		if (m_lastno >= m_doc->GetSlidesCnt())	m_lastno = m_doc->GetSlidesCnt()-1;
		m_time = timeGetTime();
	}
}

void CTimeline::Reset()
{
	m_lastno = 0;
	m_time = timeGetTime();
}

int CTimeline::GetSlideNo(REFERENCE_TIME time)
{
	CSlide *sld = m_doc->GetSlide(m_lastno);
	int duration = sld->GetDuration();
	if ((timeGetTime() >= m_time + duration) && duration)
	{
		m_time = timeGetTime();
		m_lastno++;
	}


	if (m_lastno >= m_doc->GetSlidesCnt())	
	{
		if (m_doc->IsLoop())
			m_lastno = 0;
		else
			m_lastno = m_doc->GetSlidesCnt()-1;
	}
	return m_lastno;
}