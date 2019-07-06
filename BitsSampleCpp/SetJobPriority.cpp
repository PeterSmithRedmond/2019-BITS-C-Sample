// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "BitsSampleMethods.h"

// doc code:
// Sample for https://docs.microsoft.com/en-us/windows/win32/bits/setting-and-retrieving-the-properties-of-a-job
// This code is the first part of the larger snippet. That larger snippet should be trimmed down;
// the code for setting the notify flags is already part of a different snippet (the SetNotifyInterface snippet)
// We won't document using the SetReplyFileName any more


HRESULT BitsSampleMethods::SetJobPriority(IBackgroundCopyJob* job)
{
	// The IBackgroundCopyJob5 interface was added in BITS 5 as part of Windows 8
	wil::com_ptr_nothrow<IBackgroundCopyJob4> job4;
	RETURN_IF_FAILED(job->QueryInterface<IBackgroundCopyJob4>(&job4));

	RETURN_IF_FAILED (job4->SetPriority(BG_JOB_PRIORITY_HIGH));
	return S_OK;
}