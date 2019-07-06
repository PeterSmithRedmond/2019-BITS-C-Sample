// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "BitsSampleMethods.h"

// doc code:
// Sample for https://docs.microsoft.com/en-us/windows/win32/bits/how-to-block-a-bits-job-from-downloading-over-an-expensive-connection
// This code is the "Step 5: Specify the transfer policy setting for the job"
// Next step: we don't need any of the steps except the one step. The full sample should be removed entirely.

HRESULT BitsSampleMethods::SpecifyTransferPolicy(IBackgroundCopyJob* job)
{
	// The IBackgroundCopyJob5 interface was added in BITS 5 as part of Windows 8
	wil::com_ptr_nothrow<IBackgroundCopyJob5> job5;
	RETURN_IF_FAILED(job->QueryInterface<IBackgroundCopyJob5>(&job5));

	BITS_JOB_PROPERTY_VALUE propval;
	propval.Dword = BITS_COST_STATE_USAGE_BASED
		| BITS_COST_STATE_OVERCAP_THROTTLED
		| BITS_COST_STATE_BELOW_CAP
		| BITS_COST_STATE_CAPPED_USAGE_UNKNOWN
		| BITS_COST_STATE_UNRESTRICTED;

	RETURN_IF_FAILED(job5->SetProperty(BITS_JOB_PROPERTY_ID_COST_FLAGS, propval));
	return S_OK;
}