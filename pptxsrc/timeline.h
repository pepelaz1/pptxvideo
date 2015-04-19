#pragma once
#include <strmif.h>
#include "doc.h"

class CTimeline
{
private:
	CDoc *m_doc;
	int m_lastno;
	int m_time;
	int m_last_time;
	int m_duration;
public:
	CTimeline();
	~CTimeline();
	void Init(CDoc *doc);
	int GetSlideNo(REFERENCE_TIME time);
	void Reset();	
	void Prev();
	void Next();
};

