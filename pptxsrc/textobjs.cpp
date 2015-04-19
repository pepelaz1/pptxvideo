#include "textobjs.h"


// --------------------------
// CRow
// --------------------------
CRow::CRow() :m_color(RGB(0,0,0))
{
	m_bold = false;
	m_italic = false;
	m_underline = false;
	m_align = 0;
}

void CRow::SetText(wstring &text)
{
	m_text = text;
}
void CRow::SetFontname(wstring &name)
{
	m_fontname = name;
	if (m_fontname == L"")
		m_fontname = L"Arial";
}

void CRow::SetFontsize(int size)
{
	m_fontsize = size/100;
	if (m_fontsize == 0) 
		m_fontsize = 10;
}

void CRow::SetTextColor(COLORREF color)
{
	m_color = color;
}

void CRow::SetBold(BOOL bold)
{
	m_bold = bold;
}

void CRow::SetItalic(BOOL italic)
{
	m_italic = italic;
}
void CRow::SetUnderline(wstring under)
{
	if (under == L"sng")
		m_underline = true;
	else
		m_underline = false;
}

void CRow::SetAlign(int align)
{
	m_align = align;
}

wstring &CRow::GetText()
{
	return m_text;
}

wstring &CRow::GetFontname()
{
	return m_fontname;
}

int CRow::GetFontsize()
{
	return m_fontsize;
}

COLORREF CRow::GetTextColor()
{
	return m_color;
}

BOOL CRow::GetBold()
{
	return m_bold;
}

BOOL CRow::GetItalic()
{
	return m_italic;
}

BOOL CRow::GetUnderline()
{
	return m_underline;
}

int CRow::GetAlign()
{
	return m_align;
}

// --------------------------
// CPara
// --------------------------
void CPara::AddRow(CRow &r)
{
	m_rows.push_back(r);
}

int CPara::GetRowCount()
{
	return (int)m_rows.size();
}

CRow &CPara::GetRow(int i)
{
	return m_rows[i];
}

void CPara::SetBuChar(wstring buchar)
{
	m_buchar = buchar;
}

wstring  CPara::GetBuChar()
{
	return m_buchar;
}