// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "NotifyInterface.h"


// https://docs.microsoft.com/en-us/windows/desktop/bits/registering-a-com-callback
// https://docs.microsoft.com/en-us/windows/desktop/api/bits/nn-bits-ibackgroundcopycallback

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
	return InterlockedIncrement(&refCount);
}

ULONG CNotifyInterface::Release()
{
	ULONG  updatedRefCount = InterlockedDecrement(&refCount);

	if (updatedRefCount == 0)
	{
		delete this;
	}

	return updatedRefCount;
}

HRESULT CNotifyInterface::JobTransferred(IBackgroundCopyJob* pJob)
{
	// BITS won't try to serialize the callbacks. The user of the code would probably prefer
	// that the output not be all interlaced together. This mutex means that when this method
	// is called while another version is still active that the second callback will wait until
	// the first callback has finished.
	auto releaseOnExit = mutex.acquire();

	//Add logic that will not block the callback thread. If you need to perform
	//extensive logic at this time, consider creating a separate thread to perform
	//the work.
	std::wcout << L"NotifyInterface::JobTransferred" << std::endl;

	RETURN_IF_FAILED(pJob->Complete());

	//If you do not return S_OK, BITS continues to call this callback.
	return S_OK;
}

HRESULT CNotifyInterface::JobError(IBackgroundCopyJob* pJob, IBackgroundCopyError* pError)
{
	// BITS won't try to serialize the callbacks. The user of the code would probably prefer
	// that the output not be all interlaced together. This mutex means that when this method
	// is called while another version is still active that the second callback will wait until
	// the first callback has finished.
	auto releaseOnExit = mutex.acquire();

	HRESULT errorCode = S_OK;
	BOOL isError = TRUE;

	std::wcout << L"NotifyInterface::JobError" << std::endl;
	//Use pJob and pError to retrieve information of interest. For example,
	//if the job is an upload reply, call the IBackgroundCopyError::GetError method 
	//to determine the context in which the job failed. If the context is 
	//BG_JOB_CONTEXT_REMOTE_APPLICATION, the server application that received the 
	//upload file failed.

	BG_ERROR_CONTEXT context;
	RETURN_IF_FAILED(pError->GetError(&context, &errorCode));

	//If the proxy or server does not support the Content-Range header or if
	//antivirus software removes the range requests, BITS returns BG_E_INSUFFICIENT_RANGE_SUPPORT.
	//This implementation tries to switch the job to foreground priority, so
	//the content has a better chance of being successfully downloaded.
	if (errorCode == BG_E_INSUFFICIENT_RANGE_SUPPORT)
	{
		wil::com_ptr_nothrow<IBackgroundCopyFile> file;
		RETURN_IF_FAILED(pError->GetFile(&file));

		BG_FILE_PROGRESS progress;
		RETURN_IF_FAILED(file->GetProgress(&progress));
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
			RETURN_IF_FAILED(pJob->SetPriority(BG_JOB_PRIORITY_FOREGROUND));
			RETURN_IF_FAILED(pJob->Resume());
			isError = FALSE;
		}
	}

	if (isError == TRUE)
	{
		wil::unique_cotaskmem_string jobName;
		wil::unique_cotaskmem_string errorDescription;

		RETURN_IF_FAILED(pJob->GetDisplayName(&jobName));
		RETURN_IF_FAILED(pError->GetErrorDescription(LANGIDFROMLCID(GetThreadLocale()), &errorDescription));

		if (&jobName && &errorDescription)
		{
			std::wcout << L"ERROR: job=" << jobName.get() << L" description=" << errorDescription.get() << std::endl;
		}
	}

	//If you do not return S_OK, BITS continues to call this callback.
	return S_OK;
}

HRESULT CNotifyInterface::JobModification(IBackgroundCopyJob* pJob, DWORD dwReserved)
{
	// BITS won't try to serialize the callbacks. The user of the code would probably prefer
	// that the output not be all interlaced together. This mutex means that when this method
	// is called while another version is still active that the second callback will wait until
	// the first callback has finished.
	auto releaseOnExit = mutex.acquire();

	BG_JOB_PROGRESS Progress;
	BG_JOB_STATE State;

	const WCHAR *JobStates[] = { L"Queued", L"Connecting", L"Transferring",
						   L"Suspended", L"Error", L"Transient Error",
						   L"Transferred", L"Acknowledged", L"Canceled"
	};
	wil::unique_cotaskmem_string jobName;
	RETURN_IF_FAILED(pJob->GetDisplayName(&jobName));

	RETURN_IF_FAILED(pJob->GetProgress(&Progress));

	RETURN_IF_FAILED(pJob->GetState(&State));

	std::wcout << L"NotifyInterface::JobModification New state=" << JobStates[State] << L" Job=" << jobName.get() << std::endl;
	//Do something with the progress and state information.
	//BITS generates a high volume of modification
	//callbacks. Use this callback with discretion. Consider creating a timer and 
	//polling for state and progress information.

	return S_OK;
}
