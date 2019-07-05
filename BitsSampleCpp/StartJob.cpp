// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "BitsSampleMethods.h"


HRESULT BitsSampleMethods::StartJob(IBackgroundCopyJob* job)
{

	// https://docs.microsoft.com/en-us/windows/desktop/bits/completing-and-canceling-a-job
	// Doc changes: this section did not include documentation.
	// Doc changes: all this code is new.
	std::wcout << L"Resume the job to start it" << std::endl;
	RETURN_IF_FAILED(job->Resume());


	return S_OK;
}
