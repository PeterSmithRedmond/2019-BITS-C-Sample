// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "BitsSampleMethods.h"


HRESULT BitsSampleMethods::CompleteJob(IBackgroundCopyJob* job)
{
	// Part 6: Complete the job
	// https://docs.microsoft.com/en-us/windows/desktop/bits/completing-and-canceling-a-job
	// Doc changes: this section did not include documentation.
	// Doc changes: all this code is new.
	std::wcout << L"Complete the job" << std::endl;
	RETURN_IF_FAILED(job->Complete());

	return S_OK;
}
