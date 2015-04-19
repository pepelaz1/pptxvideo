#include <streams.h>
#include <dvdmedia.h>
#include <InitGuid.h>
#include "pin.h"
#include "filter.h"

CFilter::CFilter(IUnknown *pUnk, HRESULT *phr)
           : CSource(NAME("PptxSource"), pUnk, CLSID_PptxSource) 
{
    
    m_pPin = new CPin(phr, this);

	if (phr)
	{
		if (m_pPin == NULL)
			*phr = E_OUTOFMEMORY;
		else
			*phr = S_OK;
	}  
}


CFilter::~CFilter()
{	
    delete m_pPin;
}


CUnknown * WINAPI CFilter::CreateInstance(IUnknown *pUnk, HRESULT *phr)
{
    CFilter *pNewFilter = new CFilter(pUnk, phr );

	if (phr)
	{
		if (pNewFilter == NULL) 
			*phr = E_OUTOFMEMORY;
		else
			*phr = S_OK;
	}
    return pNewFilter;
}

STDMETHODIMP CFilter::NonDelegatingQueryInterface(REFIID riid, void ** ppv)
{
	if (riid == IID_IPptxSrc) {
        return GetInterface((IPptxSrc *) this, ppv);
	} else {
		return CSource::NonDelegatingQueryInterface(riid, ppv);
	}
}

STDMETHODIMP CFilter::SetVideoWidth(int width)
{
	m_pPin->SetVideoWidth(width);
	return S_OK;
}

STDMETHODIMP CFilter::SetVideoHeight(int height)
{
	m_pPin->SetVideoHeight(height);
	return S_OK;
}

STDMETHODIMP CFilter::SetFps(int fps)
{
	m_pPin->SetFps(fps);
	return S_OK;
}

STDMETHODIMP CFilter::SetFile(WCHAR *file)
{
	m_pPin->SetFile(file);
	return S_OK;
}

STDMETHODIMP CFilter::ShowStat(BOOL show)
{
	m_pPin->ShowStat(show);
	return S_OK;
}

STDMETHODIMP CFilter::Prev()
{
	m_pPin->Prev();
	return S_OK;
}

STDMETHODIMP CFilter::Next()
{
	m_pPin->Next();
	return S_OK;
}

STDMETHODIMP CFilter::ClearAll()
{
	m_pPin->ClearAll();
	return S_OK;
}