#pragma once
#include <Windows.h>
#include "sldobjs.h"
#include <vector>
using namespace std;

class CSlide
{
private:
	vector<CSldObject *>m_Objects;
	int m_num;
	int m_duration;
	COLORREF m_backclr;

	void Clear();
public:
	CSlide();
	~CSlide();
	int GetObjectsCnt();
	CSldObject *GetObj(int n);
	void AddObj(CSldObject *obj);
	void SetNum(int num);
	int GetNum();
	void SetDuration(int duration);
	int GetDuration();
	void SetBkColor(COLORREF clr);
	COLORREF GetBkColor();
};

