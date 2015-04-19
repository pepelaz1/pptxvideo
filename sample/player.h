#pragma once

#include "..\pptxsrc\ipptxsrc.h"
#include <evr.h>


using namespace std;
class CPlayer
{
private:
	HWND m_owner;
	int m_Width;
	int m_Height;
	WCHAR m_file[MAX_PATH];
	int m_fps;
	int m_showstat;

	CComPtr<IGraphBuilder>m_gb;
	CComPtr<IBaseFilter>m_pptxsrc;
	CComPtr<IPptxSrc>m_ipptx;
	CComPtr<IBaseFilter>m_evr;
	CComPtr<IMFGetService> m_gs;
	CComPtr<IMFVideoDisplayControl> m_vdc;

	CComPtr<IMediaControl>m_mc;
	CComPtr<IVideoWindow>m_vw;
	
	
	HRESULT AddFilter(const GUID &guid, LPWSTR name, CComPtr<IBaseFilter>&filter);
	HRESULT AddToRot(IUnknown *graph, DWORD *reg);
	void TuneVideoWindow();
public:
	CPlayer();
	~CPlayer();

	HRESULT Init(HWND owner);
	void Reset();

	HRESULT Play();
	HRESULT Pause();
	HRESULT Stop();
	void UpdateVWPos();
	void RepaintVW();
	void ChangeResolution(int width, int height);
	void SetFile(WCHAR *file);
	void SetFps(int fps);
	void ShowStat(BOOL show);
	void NextSlide();
	void PrevSlide();
};

