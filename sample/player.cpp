#include "stdafx.h"
#pragma warning( disable : 4995)

// {B8386ADB-9F63-4670-B4E5-D0A38819E062}
DEFINE_GUID(CLSID_PptxSource, 
0xb8386adb, 0x9f63, 0x4670, 0xb4, 0xe5, 0xd0, 0xa3, 0x88, 0x19, 0xe0, 0x62);


CPlayer::CPlayer() :
	m_owner(NULL),
	m_Width(1024),
	m_Height(768),
	m_fps(30),
	m_showstat(TRUE)
{
	ZeroMemory(m_file, MAX_PATH);
}

CPlayer::~CPlayer()
{
}

HRESULT CPlayer::Init(HWND owner)
{
	HRESULT hr = S_OK;
	Reset();
	m_owner = owner;
		
	hr = m_gb.CoCreateInstance(CLSID_FilterGraph);
	if (FAILED(hr))
		return hr;

	hr = AddFilter(CLSID_PptxSource, L"Pptx Source", m_pptxsrc); 
	if (FAILED(hr))
		return hr;
		
	CComPtr<IEnumPins>enm;
	hr = m_pptxsrc->EnumPins(&enm);
	if (FAILED(hr))
		return hr;

	hr = m_pptxsrc->QueryInterface(IID_IPptxSrc, (void **)&m_ipptx);
	if (FAILED(hr))
		return hr;

	m_ipptx->SetVideoWidth(m_Width);
	m_ipptx->SetVideoHeight(m_Height);
	m_ipptx->SetFile(m_file);
	m_ipptx->SetFps(m_fps);
	m_ipptx->ShowStat(m_showstat);

	hr = AddFilter(CLSID_EnhancedVideoRenderer, L"Enchaced Video Renderer", m_evr); 
	if (FAILED(hr))
		return hr;


	CComPtr<IPin>pin;
	ULONG fetched = 0;
	enm->Next(1,&pin,&fetched);

	hr = m_gb->Render(pin);
	if (FAILED(hr))
		return hr;

	//DWORD reg;
	//AddToRot(m_gb,&reg);

	hr = m_gb.QueryInterface(&m_mc);
	if (FAILED(hr))
		return hr;

	//hr = m_gb.QueryInterface(&m_vw);
	//if (FAILED(hr))
	//		return hr;

	
	hr = m_evr.QueryInterface(&m_gs);
	if (FAILED(hr))
		return hr;

	hr = m_gs->GetService(MR_VIDEO_RENDER_SERVICE, IID_IMFVideoDisplayControl
		, (void **)&m_vdc);
	if (FAILED(hr))
		return hr;


	TuneVideoWindow();
	return hr;
}

void CPlayer::TuneVideoWindow()
{
	m_vdc->SetVideoWindow(m_owner);
	//m_vw->put_Owner((OAHWND)m_owner);
	//m_vw->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS);
	//m_vw->put_Visible(OATRUE);
	UpdateVWPos();
}

void CPlayer::UpdateVWPos()
{
	/*if (m_vw)
	{
		RECT grc;
		GetClientRect(m_owner, &grc);
		m_vw->SetWindowPosition(10, 10, grc.right-20, grc.bottom-55);
		::RedrawWindow(m_owner,NULL,NULL,0);
	}*/

	if (m_vdc)
	{	
		RECT r;
		GetClientRect(m_owner, &r);
		m_vdc->SetVideoPosition(NULL,&r);
	}
}

void CPlayer::RepaintVW()
{
	if (m_vdc)
	{
		m_vdc->RepaintVideo();
	}
}

void CPlayer::Reset()
{
	if (m_mc)
		m_mc.Release();

	//if (m_vw)
	//{
	//		m_vw->put_Visible(OAFALSE);
	//	m_vw->put_Owner(NULL);
	//	m_vw.Release();
	//}

	if (m_gs)
		m_gs.Release();

	if (m_vdc)
		m_vdc.Release();

	if (m_ipptx)
	{
		m_ipptx->ClearAll();
		m_ipptx.Release();
	}

	if (m_pptxsrc)
		m_pptxsrc.Release();

	if (m_evr)
		m_evr.Release();

	if (m_gb)
		m_gb.Release();
}

HRESULT CPlayer::Play()
{
	HRESULT hr = S_OK;
	hr = m_mc->Run();
	if (FAILED(hr))
		return hr;
	return hr;
}

HRESULT CPlayer::Pause()
{
	HRESULT hr = S_OK;
	hr = m_mc->Pause();
	if (FAILED(hr))
		return hr;
	return hr;
}

HRESULT CPlayer::Stop()
{
	HRESULT hr = S_OK;
	hr = m_mc->Stop();
	if (FAILED(hr))
		return hr;
	return hr;
}


HRESULT CPlayer::AddFilter(const GUID &guid, LPWSTR name, CComPtr<IBaseFilter> &filter)
{
	HRESULT hr = S_OK;
	CComPtr<IBaseFilter> flt;
	hr = flt.CoCreateInstance(guid);
	if (FAILED(hr))
		return hr;

	filter = flt;

	hr = m_gb->AddFilter(filter, name);
	if (FAILED(hr))
		return hr;	
	
	return hr;
}

HRESULT CPlayer::AddToRot(IUnknown *graph, DWORD *reg) 
{
    CComPtr<IMoniker> moniker = NULL;
    CComPtr<IRunningObjectTable>rot = NULL;

    if (FAILED(GetRunningObjectTable(0, &rot))) 
    {
        return E_FAIL;
    }
    
    const size_t STRING_LENGTH = 256;

    WCHAR wsz[STRING_LENGTH];
    StringCchPrintfW(wsz, STRING_LENGTH, L"FilterGraph %08x pid %08x", (DWORD_PTR)graph, GetCurrentProcessId());
    
    HRESULT hr = CreateItemMoniker(L"!", wsz, &moniker);
    if (SUCCEEDED(hr)) 
    {
        hr = rot->Register(ROTFLAGS_REGISTRATIONKEEPSALIVE, 
							graph, 
							moniker, 
							reg);
    }
    return hr;
}  

void CPlayer::ChangeResolution(int width, int height)
{
	m_Width = width;
	m_Height = height;
}

void CPlayer::SetFile(WCHAR *file)
{
	lstrcpy(m_file, file);
}

void CPlayer::SetFps(int fps)
{
	m_fps = fps;
}

void CPlayer::ShowStat(BOOL show)
{
	m_showstat = show;
}

void CPlayer::NextSlide()
{
	m_ipptx->Next();
}

void CPlayer::PrevSlide()
{
	m_ipptx->Prev();
}