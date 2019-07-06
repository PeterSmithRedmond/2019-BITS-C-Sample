// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "BitsSampleMethods.h"


// doc code:
// Sample for https://docs.microsoft.com/en-us/windows/win32/bits/polling-for-the-status-of-the-job
// This code replaces all of the snippet code

HRESULT BitsSampleMethods::PollJobState(IBackgroundCopyJob* job, BG_JOB_STATE* pState)
{
	const WCHAR *JobStates[] = { L"Queued", L"Connecting", L"Transferring",
					   L"Suspended", L"Error", L"Transient Error",
					   L"Transferred", L"Acknowledged", L"Canceled"
	};

	do
	{
		Sleep(100); // 100 milliseconds
		RETURN_IF_FAILED(job->GetState(pState));
		std::wcout << L"POLLING LOOP: STATE=" << JobStates[*pState] << L"\n";

		RETURN_IF_FAILED(DetermineJobProgress(job));

	} while (*pState != BG_JOB_STATE_TRANSFERRED &&
		*pState != BG_JOB_STATE_ERROR &&
		*pState != BG_JOB_STATE_ACKNOWLEDGED); 
	// If the job state is ACKNOWLEDGED then some other code must have completed the job already.

	return S_OK;
}


// doc code:
// Sample for https://docs.microsoft.com/en-us/windows/win32/bits/determining-the-progress-of-a-job// This code replaces all of the first snippet code
// The upload-reply code will remain untouched.

HRESULT BitsSampleMethods::DetermineJobProgress(IBackgroundCopyJob* job)
{
	BG_JOB_PROGRESS bitsProgress;
	RETURN_IF_FAILED(job->GetProgress(&bitsProgress));

	//Because the BytesTotal member can be 0 or BG_SIZE_UNKNOWN, you may not be able 
	//to determine a percentage value to display, such as 57%. It is best to display a 
	//string that shows the number of bytes transferred. For example, "123456 of 
	//999999" or "123456 of Unknown".
	if (bitsProgress.BytesTotal == BG_SIZE_UNKNOWN)
	{
		std::wcout << L"POLLING LOOP: Transferred " << bitsProgress.BytesTransferred << L" bytes of " << "unknown" << std::endl;
	}
	else
	{
		std::wcout << L"POLLING LOOP: Transferred " << bitsProgress.BytesTransferred << L" bytes of " << bitsProgress.BytesTotal << std::endl;
	}
	std::wcout << L"POLLING LOOP: Transferred " << bitsProgress.FilesTransferred << L" files of " << bitsProgress.FilesTotal << std::endl;

	return S_OK;
}
