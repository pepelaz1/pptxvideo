#pragma once
#include "parser.h"
#include "doc.h"
#include "timeline.h"
#include "painter.h"

class CPin : public CSourceStream
{
private:
	CParser m_parser;
	CDoc m_doc;
	CTimeline m_timeline;
	CPainter m_painter;
protected:

    CRefTime m_rtSampleTime;	        // The time stamp for each sample
    REFERENCE_TIME m_rtFrameLength;

    CMediaType m_MediaType;
    CCritSec m_cSharedState;            // Protects our internal state
    CImageDisplay m_Display;            // Figures out our media type for us

	UINT m_frameno;
	int m_videoWidth, m_videoHeight;
	int m_fps;
	WCHAR m_file[MAX_PATH];

	HRESULT OnThreadStartPlay();
public:

    CPin(HRESULT *phr, CSource *pFilter);
    ~CPin();

    // Override the version that offers exactly one media type
    HRESULT DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *pRequest);
    HRESULT FillBuffer(IMediaSample *pSample);
    
    // Set the agreed media type and set up the necessary parameters
    HRESULT SetMediaType(const CMediaType *pMediaType);

    // Support multiple display formats
    HRESULT CheckMediaType(const CMediaType *pMediaType);
    HRESULT GetMediaType(int iPosition, CMediaType *pmt);

    // Quality control
	// Not implemented because we aren't going in real time.
	// If the file-writing filter slows the graph down, we just do nothing, which means
	// wait until we're unblocked. No frames are ever dropped.
    STDMETHODIMP Notify(IBaseFilter *pSelf, Quality q)
    {
        return E_FAIL;
    }

	void SetVideoWidth(int width);
	void SetVideoHeight(int height);
	void SetFps(int fps);
	void SetFile(WCHAR *file);
	void ShowStat(BOOL show);
	void Prev();
	void Next();
	void ClearAll();
};
