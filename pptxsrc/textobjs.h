#include <string>
#include <vector>
using namespace std;
#include <Windows.h>

class CRow
{
private:
	wstring m_text;
	wstring m_fontname;
	int m_fontsize;
	COLORREF m_color;
	BOOL m_bold;
	BOOL m_italic;
	BOOL m_underline;
	BOOL m_measure;
	int m_align;
public:
	CRow();
	void SetText(wstring &text);
	void SetFontname(wstring &name);
	void SetFontsize(int size);
	void SetTextColor(COLORREF color);
	void SetBold(BOOL bold);
	void SetItalic(BOOL italic);
	void SetUnderline(wstring under);
	void SetAlign(int align);

	wstring &GetText();
	wstring &GetFontname();
	int GetFontsize();
	COLORREF GetTextColor();
	BOOL GetBold();
	BOOL GetItalic();
	BOOL GetUnderline();
	int GetAlign();

};

class CPara
{
private:
	wstring m_buchar;
	vector<CRow> m_rows;
public:
	void AddRow(CRow &r);
	int GetRowCount();
	CRow &GetRow(int i);
	void SetBuChar(wstring buchar);
	wstring GetBuChar();
};