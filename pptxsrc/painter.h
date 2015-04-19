#pragma once
#include <Windows.h>
#include <streams.h>
#include <gdiplus.h>
using namespace Gdiplus;
#include "slide.h"

class CPainter
{
private:
	Graphics *m_g, *m_g_out;
	Bitmap *m_bmp, *m_bmp_out;
	VIDEOINFOHEADER *m_pvi;
	BYTE *m_buffer;
	BYTE *m_buffer2;

	int m_lastnum;
	CMediaType *m_mt;
	BOOL m_showstat;
	ULONG_PTR m_gdiplusToken;
	void DrawObjects(CSlide *slide);
	void DrawImage(CSldImage *image);
	void DrawTxt(CSldText *text);
	void DrawStat(CSlide *slide, int frameno, REFERENCE_TIME time);
public:
	CPainter();
	~CPainter();
	void Init(CMediaType *mt);
	void Draw( CSlide *slide, int frameno, REFERENCE_TIME time, BYTE *buff, long size);
	void ShowStat(BOOL show);
};

