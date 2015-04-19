#pragma once
#include <Windows.h>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;
#include "textobjs.h"

class CSldObject
{
protected:
	int m_type;
protected:
   int m_x;
   int m_y;
   int m_width;
   int m_height;
public:
	enum {None = 0, Image, Text};
	CSldObject();
	virtual ~CSldObject();
	int GetType();
	void SetPosition(int x, int y, int width, int height);
	int GetX();
	int GetY();
	int GetWidth();
	int GetHeight();
};


class CSldImage : public CSldObject
{
private:
   int m_buffsize;
   HGLOBAL  m_buffer;
   void ClearBuffer();
public:
	CSldImage();
	virtual ~CSldImage();
	void Load(wstring file);
	HGLOBAL GetBuffer();
};


class CSldText : public CSldObject
{
private:
	vector<CPara> m_paras;
public:
	CSldText();
	void AddPara(CPara &p);
	int GetParaCount();
	CPara &GetPara(int i);
};

