#include "doc.h"


CDoc::CDoc() :
	m_disable_clicks(false)
  , m_loop(false)
{
}


CDoc::~CDoc()
{
}

void CDoc::Clear()
{
	for( int i = 0; i < (int)m_Slides.size(); i++)
	{
		delete m_Slides[i];
		m_Slides[i] = NULL;
	}
	m_Slides.clear();
}

void CDoc::AddSlide(CSlide *slide)
{
	m_Slides.push_back(slide);
}

CSlide *CDoc::GetSlide(int n)
{
	return m_Slides[n];
}

int CDoc::GetSlidesCnt()
{
	return (int)m_Slides.size();
}

void CDoc::SetDisableClics(bool disable)
{
	m_disable_clicks = disable;
}

void CDoc::SetLoop(bool loop)
{
	m_loop = loop;
}

bool CDoc::IsDisableClicks()
{
	return m_disable_clicks;
}

bool CDoc::IsLoop()
{
	return m_loop;
}