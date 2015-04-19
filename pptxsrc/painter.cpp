#include "painter.h"


CPainter::CPainter() 
	: m_mt(NULL)
	 , m_showstat(TRUE)
{
	GdiplusStartupInput gdiplusStartupInput; 
    GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
	m_bmp = NULL;
	m_bmp_out = NULL;
	m_g = NULL;
	m_g_out = NULL;
	m_pvi = NULL;
	m_buffer = NULL;
	m_buffer2 = NULL;
}


CPainter::~CPainter()
{
	delete m_buffer;
	delete m_buffer2;
	delete m_bmp;
	delete m_bmp_out;
	delete m_g;
	delete m_g_out;
	GdiplusShutdown(m_gdiplusToken);
}

void CPainter::Init(CMediaType *mt)
{
	m_mt = mt;
	m_lastnum = -1;
	m_pvi = (VIDEOINFOHEADER*)m_mt->pbFormat;
	m_bmp = new Bitmap(m_pvi->bmiHeader.biWidth, m_pvi->bmiHeader.biHeight, PixelFormat32bppRGB);
	m_g  = new Graphics(m_bmp);

	m_bmp_out = new Bitmap(m_pvi->bmiHeader.biWidth, m_pvi->bmiHeader.biHeight, PixelFormat32bppRGB);
	m_g_out  = new Graphics(m_bmp_out);
	
	
}

void CPainter::ShowStat(BOOL show)
{
	m_showstat = show;
}

void CPainter::Draw( CSlide *slide, int frameno, REFERENCE_TIME time, BYTE *buff, long size)
{	
	if ( slide->GetNum() != m_lastnum)
	{
		// Draw slide background
		SolidBrush brush(Color(255
			,GetBValue(slide->GetBkColor())
			,GetGValue(slide->GetBkColor())
			,GetRValue(slide->GetBkColor())
			));
		m_g->FillRectangle(&brush,0,0,m_pvi->bmiHeader.biWidth,m_pvi->bmiHeader.biHeight);

		DrawObjects(slide);

		Rect r(0,0,m_pvi->bmiHeader.biWidth, m_pvi->bmiHeader.biHeight);
		BitmapData bd1;
		m_bmp->LockBits(&r, ImageLockModeRead, PixelFormat32bppRGB, &bd1); 
		
		if (m_buffer)
		{
			delete m_buffer;
			m_buffer = NULL;
		}
		m_buffer = new BYTE [bd1.Stride * bd1.Height];
		CopyMemory(m_buffer, bd1.Scan0, bd1.Stride * bd1.Height);

		if (m_buffer2)
		{
			delete m_buffer2;
			m_buffer2 = NULL;
		}
		m_buffer2 = new BYTE [bd1.Stride * bd1.Height];
		CopyMemory(m_buffer2, bd1.Scan0, bd1.Stride * bd1.Height);

		m_bmp->UnlockBits(&bd1);

		m_lastnum = slide->GetNum();
	}

	CopyMemory(m_buffer2, m_buffer, m_pvi->bmiHeader.biWidth*m_pvi->bmiHeader.biHeight*m_pvi->bmiHeader.biBitCount/8);
	BITMAPINFO bmi;
	CopyMemory(&bmi,(BITMAPINFO *)&m_pvi->bmiHeader, sizeof(BITMAPINFO));
	bmi.bmiHeader.biHeight = -bmi.bmiHeader.biHeight;
	
	delete m_bmp_out;
	m_bmp_out = Bitmap::FromBITMAPINFO( &bmi, m_buffer2);

	delete m_g_out;
	m_g_out  = new Graphics(m_bmp_out);
	

	if (m_showstat)
		DrawStat(slide,frameno, time);
	

   // Copy bitmap bits into output buffer
   BitmapData bd;
   Rect r(0,0,m_pvi->bmiHeader.biWidth, abs(m_pvi->bmiHeader.biHeight));
   m_bmp_out->LockBits(&r, ImageLockModeRead, PixelFormat32bppRGB, &bd);           
   BYTE *out = buff + (m_pvi->bmiHeader.biHeight-1)*bd.Stride;
   BYTE *in = (BYTE *)bd.Scan0;
   for ( int i = 0; i < abs(m_pvi->bmiHeader.biHeight); i++)
   {
	   CopyMemory(out, in, bd.Stride);
	   in += bd.Stride;
	   out -= bd.Stride;   
   }
   m_bmp_out->UnlockBits(&bd);
}

void CPainter::DrawStat(CSlide *slide, int frameno, REFERENCE_TIME time)
{
	CRefTime rt(time);
	UINT secs = (rt.Millisecs() / MILLISECONDS) % 60;
	UINT mins = (rt.Millisecs() / (MILLISECONDS*60)) % 60;
	UINT hrs = (rt.Millisecs() / (MILLISECONDS*3600)) % 24;

	m_g_out->SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
	
	WCHAR s[256];
	wsprintf(s,L"Frame number: %07d, Slide number: %02d, Stream time: %02d:%02d:%02d",frameno, slide->GetNum()
		,hrs, mins, secs);

	Font myFont(L"Calibri", 20);
	RectF layoutRect(0.0f, 0.0f, (Gdiplus::REAL)m_pvi->bmiHeader.biWidth, (Gdiplus::REAL)m_pvi->bmiHeader.biHeight);
	StringFormat format;
	format.SetAlignment(StringAlignmentNear);
	SolidBrush blackBrush(Color(255, 0, 0, 0));
	
	m_g_out->DrawString(
		s,
		lstrlenW(s),
		&myFont,
		layoutRect,
		&format,
		&blackBrush);
}

void CPainter::DrawObjects(CSlide *slide)
{
	if (slide)
	{
		for ( int i = 0; i < (int)slide->GetObjectsCnt(); i++)
		{
			CSldObject *o = slide->GetObj(i);
			if (o)
			{
				if ( o->GetType() == CSldObject::Image)
				{
					DrawImage((CSldImage *)o);
				}
				else if ( o->GetType() == CSldObject::Text)
				{
					DrawTxt((CSldText *)o);
				}
			}
		}
	}
}

void CPainter::DrawImage(CSldImage *image)
{
	HGLOBAL buff = image->GetBuffer();
	IStream* strm = NULL;
	if(CreateStreamOnHGlobal(buff,FALSE,&strm) == S_OK)
	{
		double totalw = 9144000.0;
		double totalh = 6858000.0;

		int vidwidth = m_pvi->bmiHeader.biWidth;
		int vidheight = m_pvi->bmiHeader.biHeight;

		int x = (int)(((double)vidwidth * (double)image->GetX()) / totalw);
		int y = (int)(((double)vidheight * (double)image->GetY()) / totalh);
		int width = (int)(((double)vidwidth * (double)image->GetWidth()) / totalw);
		int height = (int)(((double)vidheight * (double)image->GetHeight()) / totalh);

		Bitmap *bmp = Bitmap::FromStream(strm,true);
		m_g->DrawImage(bmp,x,y,width,height);
		strm->Release();
		delete bmp;		
	}
}



void CPainter::DrawTxt(CSldText *text)
{
	double totalw = 9144000.0;
	double totalh = 6858000.0;

	int vidwidth = m_pvi->bmiHeader.biWidth;
	int vidheight = m_pvi->bmiHeader.biHeight;

	int x = (int)(((double)vidwidth * (double)text->GetX()) / totalw);
	int y = (int)(((double)vidheight * (double)text->GetY()) / totalh);
	int width = (int)(((double)vidwidth * (double)text->GetWidth()) / totalw);
	int height = (int)(((double)vidheight * (double)text->GetHeight()) / totalh);

	Pen pen(Color(0,0,0));
//	m_g->DrawRectangle(&pen,x,y,width,height);

	double xpos = x;
	double ypos = y;
	double yoff = 0;
	
	for ( int i = 0; i < text->GetParaCount(); i++)
	{
		CPara &para = text->GetPara(i);
		for ( int j = 0; j < para.GetRowCount(); j++)
		{
			CRow &row  = para.GetRow(j);
			
			wstring txt = row.GetText();
			double fontsize = row.GetFontsize();

			if (para.GetBuChar() != L"" && j == 0)
			{
				// Draw bullet char
				wstring butxt = para.GetBuChar();
				Font f1(row.GetFontname().c_str(),(Gdiplus::REAL)fontsize, 0, UnitPixel);
				RectF lr1((Gdiplus::REAL)x, (Gdiplus::REAL)ypos, (Gdiplus::REAL)width-x, (Gdiplus::REAL)height);
				RectF or1;
				StringFormat fmt1;
				m_g->MeasureString(butxt.c_str(), butxt.length(),&f1,lr1,&fmt1, &or1);
				or1.Width *= (Gdiplus::REAL)(fontsize/15);
				SolidBrush br1(Color(GetBValue(row.GetTextColor()),GetGValue(row.GetTextColor()),GetRValue(row.GetTextColor())));
				m_g->DrawString(butxt.c_str(),butxt.length(),&f1,or1,&fmt1,&br1);
				xpos+=or1.Width;
			}
			

			int fstyle = 0;
			if (row.GetBold())
				fstyle |= FontStyleBold;
			if (row.GetItalic())
				fstyle |= FontStyleItalic;
			if (row.GetUnderline())
				fstyle |= FontStyleUnderline;
			
			Font fnt(row.GetFontname().c_str(),(Gdiplus::REAL)fontsize, fstyle, UnitPixel);
			SolidBrush br(Color(GetBValue(row.GetTextColor()),GetGValue(row.GetTextColor()),GetRValue(row.GetTextColor())));
			StringFormat format;
			format.SetAlignment((StringAlignment)row.GetAlign());
		
			RectF layoutr((Gdiplus::REAL)x, (Gdiplus::REAL)ypos, (Gdiplus::REAL)width-x, (Gdiplus::REAL)height);
			RectF outr;
			m_g->MeasureString(txt.c_str(), txt.length(),&fnt,layoutr,&format, &outr);
	
			if ( txt != L" " && txt.length() == 1)
			{
				wstring wsps = L" ";
				RectF outrsp;
				m_g->MeasureString(wsps.c_str(), wsps.length(),&fnt,layoutr,&format, &outrsp);

				outr.Width -= outrsp.Width+1;
			}
			yoff = outr.Height;
	
			RectF txtr((Gdiplus::REAL)xpos, (Gdiplus::REAL)ypos, (Gdiplus::REAL)xpos+outr.Width, (Gdiplus::REAL)height);
			
			//m_g->DrawRectangle(&pen,txtr);
					
			if (row.GetAlign() == 1)
				m_g->DrawString(txt.c_str(),txt.length(),&fnt,layoutr,&format,&br);
			else
				m_g->DrawString(txt.c_str(),txt.length(),&fnt,txtr,&format,&br);
			xpos+=outr.Width;	
		}
		ypos += yoff;
		xpos = x;
	}
} 

