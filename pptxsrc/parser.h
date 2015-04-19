#pragma once
#include <string>
#include <sstream>
using namespace std;
#include "doc.h"
#include "xmlwrapper.h"
#include "archiver.h"

class CParser
{
private:
	wstring m_file;
	wstring m_path;
	void ClearDirectory(wstring directory);
	void Parse(wstring path, CDoc &doc);
	void ParseSlide(int num, CSlide *sld, CXmlDocWrapper &xml);
	void ParseSldImg( CSlide *sld, CXmlNodeWrapper &pic);
	void ParseSldText( CSlide *sld,  CXmlNodeWrapper &sp);
	void ParsePresProps(wstring path, CDoc &doc);
public:
	CParser(void);
	~CParser(void);
	void Load(wstring file, CDoc &doc);
};

