#pragma once

#include "ipptxsrc.h"

// {B8386ADB-9F63-4670-B4E5-D0A38819E062}
DEFINE_GUID(CLSID_PptxSource, 
0xb8386adb, 0x9f63, 0x4670, 0xb4, 0xe5, 0xd0, 0xa3, 0x88, 0x19, 0xe0, 0x62);

class CPin;
class CFilter : public CSource, public IPptxSrc
{
private:
    CFilter(IUnknown *pUnk, HRESULT *phr);
    ~CFilter();

    CPin *m_pPin;
public:
	DECLARE_IUNKNOWN;
    static CUnknown * WINAPI CreateInstance(IUnknown *pUnk, HRESULT *phr); 
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);
	STDMETHODIMP SetVideoWidth(int width);
	STDMETHODIMP SetVideoHeight(int height);
	STDMETHODIMP SetFps(int fps);
	STDMETHODIMP SetFile(WCHAR *file);
	STDMETHODIMP ShowStat(BOOL show);
	STDMETHODIMP Prev();
	STDMETHODIMP Next();
	STDMETHODIMP ClearAll();
};


