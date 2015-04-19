#include <streams.h>
#include "pin.h"



CPin::CPin(HRESULT *phr, CSource *pFilter)
        : CSourceStream(NAME("Pptx source filter"), phr, pFilter, L"Out"),
        m_frameno(0),
		m_videoWidth(1024),
		m_videoHeight(768),
		m_fps(30)
{
	ZeroMemory(m_file, MAX_PATH);
	m_rtFrameLength = UNITS / m_fps;
}

CPin::~CPin()
{   
}


//
// GetMediaType
//
HRESULT CPin::GetMediaType(int iPosition, CMediaType *pmt)
{
    CheckPointer(pmt,E_POINTER);
    CAutoLock cAutoLock(m_pFilter->pStateLock());

    if(iPosition < 0)
        return E_INVALIDARG;

    // Have we run off the end of types?
    if(iPosition > 0)
        return VFW_S_NO_MORE_ITEMS;

    VIDEOINFO *pvi = (VIDEOINFO *) pmt->AllocFormatBuffer(sizeof(VIDEOINFO));
    if(NULL == pvi)
        return(E_OUTOFMEMORY);

    // Initialize the VideoInfo structure before configuring its members
    ZeroMemory(pvi, sizeof(VIDEOINFO));

    switch(iPosition)
    {
        case 0:
        {    
            // Return our highest quality 32bit format

            // Since we use RGB888 (the default for 32 bit), there is
            // no reason to use BI_BITFIELDS to specify the RGB
            // masks. Also, not everything supports BI_BITFIELDS
            pvi->bmiHeader.biCompression = BI_RGB;
            pvi->bmiHeader.biBitCount    = 32;
            break;
        }
    }

    // Adjust the parameters common to all formats
    pvi->bmiHeader.biSize       = sizeof(BITMAPINFOHEADER);
    pvi->bmiHeader.biWidth      = m_videoWidth;
    pvi->bmiHeader.biHeight     = m_videoHeight;
    pvi->bmiHeader.biPlanes     = 1;
    pvi->bmiHeader.biSizeImage  = GetBitmapSize(&pvi->bmiHeader);
    pvi->bmiHeader.biClrImportant = 0;

    SetRectEmpty(&(pvi->rcSource)); // we want the whole image area rendered.
    SetRectEmpty(&(pvi->rcTarget)); // no particular destination rectangle

    pmt->SetType(&MEDIATYPE_Video);
    pmt->SetFormatType(&FORMAT_VideoInfo);
    pmt->SetTemporalCompression(FALSE);

    // Work out the GUID for the subtype from the header info.
    const GUID SubTypeGUID = GetBitmapSubtype(&pvi->bmiHeader);
    pmt->SetSubtype(&SubTypeGUID);
    pmt->SetSampleSize(pvi->bmiHeader.biSizeImage);

    return NOERROR;

} // GetMediaType

void CPin::SetVideoWidth(int width)
{
	m_videoWidth = width;
}

void CPin::SetVideoHeight(int height)
{
	m_videoHeight = height;
}

void CPin::SetFps(int fps)
{
	m_fps = fps;
	m_rtFrameLength = UNITS / m_fps;
}

void CPin::SetFile(WCHAR *file)
{
	m_doc.Clear();
	m_parser.Load(file, m_doc);
	m_timeline.Init(&m_doc);
}

void CPin::ShowStat(BOOL show)
{
	m_painter.ShowStat(show);
}

void CPin::Prev()
{
	m_timeline.Prev();
}

void CPin::Next()
{
	m_timeline.Next();
}

void CPin::ClearAll()
{
	CAutoLock cAutoLockShared(&m_cSharedState);
	m_doc.Clear();
}

//
// CheckMediaType
//
// We will accept 8, 16, 24 or 32 bit video formats, in any
// image size that gives room to bounce.
// Returns E_INVALIDARG if the mediatype is not acceptable
//
HRESULT CPin::CheckMediaType(const CMediaType *pMediaType)
{
    CheckPointer(pMediaType,E_POINTER);

    if((*(pMediaType->Type()) != MEDIATYPE_Video) ||   // we only output video
        !(pMediaType->IsFixedSize()))                  // in fixed size samples
    {                                                  
        return E_INVALIDARG;
    }

    // Check for the subtypes we support
    const GUID *SubType = pMediaType->Subtype();
    if (SubType == NULL)
        return E_INVALIDARG;

	if ( *SubType != MEDIASUBTYPE_RGB32)
	{
		return E_INVALIDARG;
	}


    // Get the format area of the media type
    VIDEOINFO *pvi = (VIDEOINFO *) pMediaType->Format();

    if(pvi == NULL)
        return E_INVALIDARG;

    // Check if the image width & height have changed
    if(    pvi->bmiHeader.biWidth   != m_videoWidth || 
     //  abs(pvi->bmiHeader.biHeight) != m_iImageHeight)
	   pvi->bmiHeader.biHeight != m_videoHeight)
    {
        // If the image width/height is changed, fail CheckMediaType() to force
        // the renderer to resize the image.
        return E_INVALIDARG;
    }

    // Don't accept formats with negative height, which would cause the desktop
    // image to be displayed upside down.
    if (pvi->bmiHeader.biHeight < 0)
        return E_INVALIDARG;

    return S_OK;  // This format is acceptable.

} // CheckMediaType


//
// DecideBufferSize
//
// This will always be called after the format has been sucessfully
// negotiated. So we have a look at m_mt to see what size image we agreed.
// Then we can ask for buffers of the correct size to contain them.
//
HRESULT CPin::DecideBufferSize(IMemAllocator *pAlloc,
                                      ALLOCATOR_PROPERTIES *pProperties)
{
    CheckPointer(pAlloc,E_POINTER);
    CheckPointer(pProperties,E_POINTER);

    CAutoLock cAutoLock(m_pFilter->pStateLock());
    HRESULT hr = NOERROR;

    VIDEOINFO *pvi = (VIDEOINFO *) m_mt.Format();
    pProperties->cBuffers = 1;
    pProperties->cbBuffer = pvi->bmiHeader.biSizeImage;

    ASSERT(pProperties->cbBuffer);

    // Ask the allocator to reserve us some sample memory. NOTE: the function
    // can succeed (return NOERROR) but still not have allocated the
    // memory that we requested, so we must check we got whatever we wanted.
    ALLOCATOR_PROPERTIES Actual;
    hr = pAlloc->SetProperties(pProperties,&Actual);
    if(FAILED(hr))
    {
        return hr;
    }

    // Is this allocator unsuitable?
    if(Actual.cbBuffer < pProperties->cbBuffer)
    {
        return E_FAIL;
    }

    // Make sure that we have only 1 buffer (we erase the ball in the
    // old buffer to save having to zero a 200k+ buffer every time
    // we draw a frame)
    ASSERT(Actual.cBuffers == 1);
    return NOERROR;

} // DecideBufferSize


//
// SetMediaType
//
// Called when a media type is agreed between filters
//
HRESULT CPin::SetMediaType(const CMediaType *pMediaType)
{
    CAutoLock cAutoLock(m_pFilter->pStateLock());

    // Pass the call up to my base class
    HRESULT hr = CSourceStream::SetMediaType(pMediaType);

    if(SUCCEEDED(hr))
    {
        VIDEOINFO * pvi = (VIDEOINFO *) m_mt.Format();
        if (pvi == NULL)
            return E_UNEXPECTED;

        switch(pvi->bmiHeader.biBitCount)
        {
            case 32:    // RGB32
                // Save the current media type and bit depth
                m_MediaType = *pMediaType;
            //    m_nCurrentBitDepth = pvi->bmiHeader.biBitCount;
                hr = S_OK;
                break;

            default:
                // We should never agree any other media types
                ASSERT(FALSE);
                hr = E_INVALIDARG;
                break;
        }
    } 

    return hr;

} // SetMediaType


HRESULT CPin::OnThreadStartPlay()
{
	m_frameno = 0;
	m_painter.Init(&m_mt);
	m_timeline.Reset();
	return S_OK;
}

// This is where we insert the DIB bits into the video stream.
// FillBuffer is called once for every sample in the stream.
HRESULT CPin::FillBuffer(IMediaSample *pSample)
{
	BYTE *pData;
    long cbData;

    CheckPointer(pSample, E_POINTER);

    CAutoLock cAutoLockShared(&m_cSharedState);

    // Access the sample's data buffer
    pSample->GetPointer(&pData);
    cbData = pSample->GetSize();
	
	// Set the timestamps that will govern playback frame rate.
	// If this file is getting written out as an AVI,
	// then you'll also need to configure the AVI Mux filter to 
	// set the Average Time Per Frame for the AVI Header.
    // The current time is the sample's start.
    REFERENCE_TIME rtStart = m_frameno * m_rtFrameLength;
    REFERENCE_TIME rtStop  = rtStart + m_rtFrameLength;

	int n = m_timeline.GetSlideNo(rtStart);
	m_painter.Draw( m_doc.GetSlide(n), m_frameno, rtStart, pData, cbData);

    pSample->SetTime(&rtStart, &rtStop); 
	m_frameno++;

	// Set TRUE on every sample for uncompressed frames
    pSample->SetSyncPoint(TRUE);

    return S_OK;
}
