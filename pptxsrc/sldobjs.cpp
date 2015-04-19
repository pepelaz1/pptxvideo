#include "sldobjs.h"



// --------------------------
// CSldObject
// --------------------------
CSldObject::CSldObject(void) : m_x(0), m_y(0), m_width(0), m_height(0)
{
	m_type = CSldObject::None;

}


CSldObject::~CSldObject(void)
{
}


int CSldObject::GetType()
{
	return m_type;
}

void CSldObject::SetPosition(int x, int y, int width, int height)
{
	m_x = x;
	m_y = y;
	m_width = width;
	m_height = height;
}

int CSldObject::GetX()
{
	return m_x;
}

int CSldObject::GetY()
{
	return m_y;
}

int CSldObject::GetWidth()
{
	return m_width;
}

int CSldObject::GetHeight()
{
	return m_height;
}


// --------------------------
// CSldImage
// --------------------------
CSldImage::CSldImage() 
	: m_buffsize(0)
{
	m_type = CSldObject::Image;
}

CSldImage::~CSldImage()
{
	ClearBuffer();
}


void CSldImage::ClearBuffer()
{
	if (m_buffsize)
	{
		GlobalFree(m_buffer);
		m_buffsize = 0;
	}
}

void CSldImage::Load(wstring file)
{
	ifstream strm(file,ios::binary  );
    if ( strm )
    {
		// get size of file
       strm.seekg(0,ios::end);
       m_buffsize = (int)strm.tellg();
       strm.seekg(0,ios::beg);
	
	   // allocate memory for file content
  	   m_buffer = GlobalAlloc(GMEM_MOVEABLE, m_buffsize);

       // read content of file
	   void* buffer = GlobalLock(m_buffer);
	   strm.read ((char *)buffer,m_buffsize);
	   strm.close();
	   GlobalUnlock(m_buffer);
	  
    }
}

HGLOBAL CSldImage::GetBuffer()
{
	return m_buffer;
}
// --------------------------
// CSldText
// --------------------------
CSldText::CSldText()
{
	m_type = CSldObject::Text;
}

void CSldText::AddPara(CPara &p)
{
	m_paras.push_back(p);
}

int CSldText::GetParaCount()
{
	return (int)m_paras.size();
}

CPara &CSldText::GetPara(int i)
{
	return m_paras[i];
}