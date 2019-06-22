#pragma once


// Doc status: the notify interface stuff doesn't work. In particular, I don't get transferred callbacks?
// Doc status: the notify interface isn't updated to use the _com_ptr_t style.
#define TWO_GB 2147483648    // 2GB
class CNotifyInterface : public IBackgroundCopyCallback
{
	LONG m_lRefCount;

public:
	//Constructor, Destructor
	CNotifyInterface() { m_lRefCount = 1; };
	~CNotifyInterface() {};

	//IUnknown
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID *ppvObj);
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();

	//IBackgroundCopyCallback methods
	HRESULT __stdcall JobTransferred(IBackgroundCopyJob* pJob);
	HRESULT __stdcall JobError(IBackgroundCopyJob* pJob, IBackgroundCopyError* pError);
	HRESULT __stdcall JobModification(IBackgroundCopyJob* pJob, DWORD dwReserved);
};

