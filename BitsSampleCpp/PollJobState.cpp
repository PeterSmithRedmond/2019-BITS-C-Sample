// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "BitsSampleMethods.h"


HRESULT BitsSampleMethods::PollJobState(IBackgroundCopyJob* job, BG_JOB_STATE* pState)
{
	// Part 5: Determining the status of a job
// Method a: Poll for the status of a job
// https://docs.microsoft.com/en-us/windows/desktop/bits/polling-for-the-status-of-the-job

	const WCHAR *JobStates[] = { L"Queued", L"Connecting", L"Transferring",
					   L"Suspended", L"Error", L"Transient Error",
					   L"Transferred", L"Acknowledged", L"Canceled"
	};

	do
	{
		Sleep(100); // 100 milliseconds
		RETURN_IF_FAILED(job->GetState(pState));
		std::wcout << L"POLLING LOOP: STATE=" << JobStates[*pState] << L"\n";

		// Part 5d: Determining the progress of a job.
		// https://docs.microsoft.com/en-us/windows/desktop/bits/determining-the-progress-of-a-job
		// doc changes: removing all of the StringCchPrintf stuff in favor of plain std::wcout


		BG_JOB_PROGRESS bitsProgress;
		RETURN_IF_FAILED(job->GetProgress(&bitsProgress));
		//TODO: don't fail? Instead do something else? 

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

	} while (*pState != BG_JOB_STATE_TRANSFERRED &&
		*pState != BG_JOB_STATE_ERROR &&
		*pState != BG_JOB_STATE_ACKNOWLEDGED); // ACKNOWLEDGED: someone else completed the job for us!


	return S_OK;
}
