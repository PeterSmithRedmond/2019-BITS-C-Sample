#include "pch.h"
#include "NotifyInterface.h"

// Doc status: do not review! This hasn't be updated to the modern style yet + it doesn't actually work.

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
	if (FAILED(hr))
	{
		//Handle error. BITS probably was unable to rename one or more of the 
		//temporary files. See the Remarks section of the IBackgroundCopyJob::Complete 
		//method for more details.
	}

	//If you do not return S_OK, BITS continues to call this callback.
	return S_OK;
}

HRESULT CNotifyInterface::JobError(IBackgroundCopyJob* pJob, IBackgroundCopyError* pError)
{
	HRESULT hr;
	BG_FILE_PROGRESS Progress;
	BG_ERROR_CONTEXT Context;
	HRESULT ErrorCode = S_OK;
	WCHAR* pszJobName = NULL;
	WCHAR* pszErrorDescription = NULL;
	BOOL IsError = TRUE;

	std::wcout << L"NotifyInterface::JobError" << std::endl;
	//Use pJob and pError to retrieve information of interest. For example,
	//if the job is an upload reply, call the IBackgroundCopyError::GetError method 
	//to determine the context in which the job failed. If the context is 
	//BG_JOB_CONTEXT_REMOTE_APPLICATION, the server application that received the 
	//upload file failed.

	hr = pError->GetError(&Context, &ErrorCode);

	//If the proxy or server does not support the Content-Range header or if
	//antivirus software removes the range requests, BITS returns BG_E_INSUFFICIENT_RANGE_SUPPORT.
	//This implementation tries to switch the job to foreground priority, so
	//the content has a better chance of being successfully downloaded.
	if (BG_E_INSUFFICIENT_RANGE_SUPPORT == ErrorCode)
	{
		IBackgroundCopyFile *pFile;
		hr = pError->GetFile(&pFile);
		hr = pFile->GetProgress(&Progress);
		if (BG_SIZE_UNKNOWN == Progress.BytesTotal)
		{
			//The content is dynamic, do not change priority. Handle as an error.
		}
		else if (Progress.BytesTotal > TWO_GB)
		{
			// BITS requires range requests support if the content is larger than 2 GB.
			// For these scenarios, BITS uses 2 GB ranges to download the file,
			// so switching to foreground priority will not help.

		}
		else
		{
			hr = pJob->SetPriority(BG_JOB_PRIORITY_FOREGROUND);
			hr = pJob->Resume();
			IsError = FALSE;
		}

		pFile->Release();
	}

	if (TRUE == IsError)
	{
		hr = pJob->GetDisplayName(&pszJobName);
		hr = pError->GetErrorDescription(LANGIDFROMLCID(GetThreadLocale()), &pszErrorDescription);

		if (pszJobName && pszErrorDescription)
		{
			//Do something with the job name and description. 
		}

		CoTaskMemFree(pszJobName);
		CoTaskMemFree(pszErrorDescription);
	}

	//If you do not return S_OK, BITS continues to call this callback.
	return S_OK;
}

HRESULT CNotifyInterface::JobModification(IBackgroundCopyJob* pJob, DWORD dwReserved)
{
	HRESULT hr;
	WCHAR* pszJobName = NULL;
	BG_JOB_PROGRESS Progress;
	BG_JOB_STATE State;

	const WCHAR *JobStates[] = { L"Queued", L"Connecting", L"Transferring",
						   L"Suspended", L"Error", L"Transient Error",
						   L"Transferred", L"Acknowledged", L"Canceled"
	};

	hr = pJob->GetDisplayName(&pszJobName);
	if (SUCCEEDED(hr))
	{
		hr = pJob->GetProgress(&Progress);
		if (SUCCEEDED(hr))
		{
			hr = pJob->GetState(&State);
			if (SUCCEEDED(hr))
			{
				std::wcout << L"NotifyInterface::JobModification New state=" << JobStates[State] << std::endl;
				//Do something with the progress and state information.
				//BITS generates a high volume of modification
				//callbacks. Use this callback with discretion. Consider creating a timer and 
				//polling for state and progress information.
			}
		}
		CoTaskMemFree(pszJobName);
	}

	return S_OK;
}
