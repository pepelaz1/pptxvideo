#include "slide.h"


CSlide::CSlide() :
	m_num(-1),
	m_duration(-1)
{
	m_backclr = RGB(255,255,255); 
}



CSlide::~CSlide()
{
	Clear();
}


void CSlide::Clear()
{
	for( int i = 0; i < (int)m_Objects.size(); i++)
	{
		delete m_Objects[i];
	}
	m_Objects.clear();
}

int CSlide::GetObjectsCnt()
{
	return m_Objects.size();
}

CSldObject *CSlide::GetObj(int n)
{
	return m_Objects[n];
}

void CSlide::AddObj(CSldObject *obj)
{
	m_Objects.push_back(obj);
}

void CSlide::SetNum(int num)
{
	m_num = num;
}

int CSlide::GetNum()
{
	return m_num;
}

void CSlide::SetDuration(int duration)
{
	m_duration = duration;
}

int CSlide::GetDuration()
{
	return m_duration;
}

void  CSlide::SetBkColor(COLORREF clr)
{
	m_backclr = clr;
}

COLORREF CSlide::GetBkColor()
{
	return m_backclr;
}