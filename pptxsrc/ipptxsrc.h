#ifndef __IPPTXSRC__
#define __IPPTXSRC__

#ifdef __cplusplus
extern "C" {
#endif

	DEFINE_GUID(IID_IPptxSrc, 
	0x1a967730, 0x1637, 0x47b4, 0x9e, 0x1d, 0x17, 0x8b, 0x17, 0x58, 0x97, 0x88);

    DECLARE_INTERFACE_(IPptxSrc, IUnknown)
    {
       	STDMETHOD(SetVideoWidth) (THIS_
					int width
					) PURE;

		STDMETHOD(SetVideoHeight) (THIS_
					int height
					) PURE;

		STDMETHOD(SetFps) (THIS_
					int fps
					) PURE;

		STDMETHOD(SetFile) (THIS_
					WCHAR *file
					) PURE;

		STDMETHOD(ShowStat) (THIS_
					BOOL show
					) PURE;
		
		STDMETHOD(Prev) () PURE;

		STDMETHOD(Next) () PURE;

		STDMETHOD(ClearAll) () PURE;
    };

#ifdef __cplusplus
}
#endif

#endif // __IPPTXSRC__

