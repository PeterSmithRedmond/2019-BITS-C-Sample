// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "NotifyInterface.h"


// https://docs.microsoft.com/en-us/windows/desktop/bits/registering-a-com-callback
// https://docs.microsoft.com/en-us/windows/desktop/api/bits/nn-bits-ibackgroundcopycallback

//TODO: WIL library might well have something for the boilerplate already.
HRESULT CNotifyInterface::QueryInterface(REFIID riid, LPVOID* ppvObj)
{
	if (riid == __uuidof(IUnknown) || riid == __uuidof(IBackgroundCopyCallback))
	{
		*ppvObj = this;
	}
	else
	{
		*ppvObj = NULL;
		return E_NOINTERFACE;
	}

	AddRef();
	return NOERROR;
}

ULONG CNotifyInterface::AddRef()
{
	return InterlockedIncrement(&m_lRefCount);
}

ULONG CNotifyInterface::Release()
{
	ULONG  ulCount = InterlockedDecrement(&m_lRefCount);

	if (0 == ulCount)
	{
		delete this;
	}

	return ulCount;
}

HRESULT CNotifyInterface::JobTransferred(IBackgroundCopyJob* pJob)
{
	HRESULT hr;

	//Add logic that will not block the callback thread. If you need to perform
	//extensive logic at this time, consider creating a separate thread to perform
	//the work.
	std::wcout << L"NotifyInterface::JobTransferred" << std::endl;

	hr = pJob->Complete();
	IFFAILRETURN(hr);

	//If you do not return S_OK, BITS continues to call this callback.
	return S_OK;
}

//doc: TODO: this needs to be updated with smart pointers.
HRESULT CNotifyInterface::JobError(IBackgroundCopyJob* pJob, IBackgroundCopyError* pError)
{
	HRESULT hr;
	HRESULT errorCode = S_OK;
	BOOL isError = TRUE;

	std::wcout << L"NotifyInterface::JobError" << std::endl;
	//Use pJob and pError to retrieve information of interest. For example,
	//if the job is an upload reply, call the IBackgroundCopyError::GetError method 
	//to determine the context in which the job failed. If the context is 
	//BG_JOB_CONTEXT_REMOTE_APPLICATION, the server application that received the 
	//upload file failed.

	BG_ERROR_CONTEXT context;
	hr = pError->GetError(&context, &errorCode);

	//If the proxy or server does not support the Content-Range header or if
	//antivirus software removes the range requests, BITS returns BG_E_INSUFFICIENT_RANGE_SUPPORT.
	//This implementation tries to switch the job to foreground priority, so
	//the content has a better chance of being successfully downloaded.
	if (errorCode == BG_E_INSUFFICIENT_RANGE_SUPPORT)
	{
		wil::com_ptr_nothrow<IBackgroundCopyFile> file;
		hr = pError->GetFile(&file);
		IFFAILRETURN(hr);

		BG_FILE_PROGRESS progress;
		hr = file->GetProgress(&progress);
		IFFAILRETURN(hr);
		if (progress.BytesTotal == BG_SIZE_UNKNOWN)
		{
			//The content is dynamic, do not change priority. Handle as an error.
		}
		else if (progress.BytesTotal > TWO_GB)
		{
			// BITS requires range requests support if the content is larger than 2 GB.
			// For these scenarios, BITS uses 2 GB ranges to download the file,
			// so switching to foreground priority will not help.

		}
		else
		{
			hr = pJob->SetPriority(BG_JOB_PRIORITY_FOREGROUND);
			hr = pJob->Resume();
			//TODO: why doesn't the code call IFFAILRETURN()?
			isError = FALSE;
		}
	}

	if (isError == TRUE)
	{
		wil::unique_cotaskmem_string jobName;
		wil::unique_cotaskmem_string errorDescription;

		hr = pJob->GetDisplayName(&jobName);
		hr = pError->GetErrorDescription(LANGIDFROMLCID(GetThreadLocale()), &errorDescription);

		if (&jobName && &errorDescription)
		{
			std::wcout << L"ERROR: job=" << &jobName << L" description=" << &errorDescription << std::endl;
		}
	}

	//If you do not return S_OK, BITS continues to call this callback.
	return S_OK;
}

HRESULT CNotifyInterface::JobModification(IBackgroundCopyJob* pJob, DWORD dwReserved)
{
	//TODO: doc change: this callback will be called concurrently. Should we demonstrate re-entrant-proof techniques?
	HRESULT hr;
	BG_JOB_PROGRESS Progress;
	BG_JOB_STATE State;

	const WCHAR *JobStates[] = { L"Queued", L"Connecting", L"Transferring",
						   L"Suspended", L"Error", L"Transient Error",
						   L"Transferred", L"Acknowledged", L"Canceled"
	};
	wil::unique_cotaskmem_string jobName;
	hr = pJob->GetDisplayName(&jobName);
	IFFAILRETURN(hr);

	hr = pJob->GetProgress(&Progress);
	IFFAILRETURN(hr);

	hr = pJob->GetState(&State);
	IFFAILRETURN(hr);

	std::wcout << L"NotifyInterface::JobModification New state=" << JobStates[State] << L" Job=" << &jobName << std::endl;
	//Do something with the progress and state information.
	//BITS generates a high volume of modification
	//callbacks. Use this callback with discretion. Consider creating a timer and 
	//polling for state and progress information.

	//TODO: the output can be comingled. We should do a lock here. 
	// Team recommendation for lock is --- do a Bing search :-)
	return hr;
}
