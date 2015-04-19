#pragma once
#include "slide.h"
#include <vector>
using namespace std;

class CDoc
{
private:
	vector<CSlide *> m_Slides;
	bool m_disable_clicks;
	bool m_loop;
public:
	CDoc();
	~CDoc();
	void Clear();
	CSlide *GetSlide(int n);
	void AddSlide(CSlide *slide);
	int GetSlidesCnt();
	void SetDisableClics(bool disable);
	void SetLoop(bool loop);
	bool IsDisableClicks();
	bool IsLoop();
};

