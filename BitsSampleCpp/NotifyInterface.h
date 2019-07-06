// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
#pragma once


#define TWO_GB 2147483648    // 2GB
class CNotifyInterface : public IBackgroundCopyCallback
{
	LONG refCount;
	wil::unique_mutex mutex;

public:
	//Constructor, Destructor
	CNotifyInterface() 
	{
		refCount = 0;
		mutex.create();
	};
	~CNotifyInterface() 
	{
	};

	//IUnknown
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID *ppvObj);
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();

	//IBackgroundCopyCallback methods
	HRESULT __stdcall JobTransferred(IBackgroundCopyJob* pJob);
	HRESULT __stdcall JobError(IBackgroundCopyJob* pJob, IBackgroundCopyError* pError);
	HRESULT __stdcall JobModification(IBackgroundCopyJob* pJob, DWORD dwReserved);
};

